#include "stdafx.h"

#include "rift_app.h"

#include "exception.h"
#include "rift_utils.h"
#include "scopeguard.h"

namespace avatar
{

	namespace
	{

		namespace error_caption
		{
			QString const ovrStart("OVR start error");
			QString const ovrInit("OVR initialization error");
		}

		namespace warning_caption
		{
			QString const ovrInit("OVR initialization warning");
		}

	}

	RiftApp::RiftApp()
		: m_hmdDescriptor(nullptr)
		, m_avatarPosition(0.0f, 1.9f, 5.0f)
		, m_frameTimeDelta(0.0f)
		, m_framesCounter(0)
		, m_framesCounterLastTime(0.0)
	{
		startOvr();
		setOvrGeometry();
	}

	RiftApp::~RiftApp()
	{
		if (!m_frameTimes.empty())
		{
			std::wcout << L"Average frame time is " << std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0) / static_cast<double>(m_frameTimes.size()) << L" ms/frame" << std::endl;
		}

		if (!isOvrFinished())
		{
			finishOvr();
		}
	}

	void RiftApp::uninitializeApp()
	{
		finishOvr();
		OpenGlApp::uninitializeApp();
	}

	void RiftApp::initializeApp()
	{
		// First of all, let the superclass perform its initialization.
		//
		OpenGlApp::initializeApp();

		// Initialize offscreen rendering texture size.
		//
		float const defaultPixelDensity = 1.0f;

		auto const defaultLeftEyeFov = m_hmdDescriptor->DefaultEyeFov[ovrEye_Left];
		auto const defaultRightEyeFov = m_hmdDescriptor->DefaultEyeFov[ovrEye_Right];

		auto const leftEyeTextureSize = ovrHmd_GetFovTextureSize(m_hmdDescriptor, ovrEye_Left, defaultLeftEyeFov, defaultPixelDensity);
		auto const rightEyeTextureSize = ovrHmd_GetFovTextureSize(m_hmdDescriptor, ovrEye_Right, defaultRightEyeFov, defaultPixelDensity);

		ovr::Sizei const fullTextureSize(leftEyeTextureSize.w + rightEyeTextureSize.w, std::max(leftEyeTextureSize.h, rightEyeTextureSize.h));

		// Create a frame buffer object using the offscreen rendering texture size.
		// The frame object also contains a depth buffer.
		//
		m_fbo = std::make_unique<QOpenGLFramebufferObject>(fullTextureSize.w, fullTextureSize.h, QOpenGLFramebufferObject::Depth);

		Q_ASSERT(m_fbo->isValid());

		// Initialize eye viewports.
		// The viewports split the full texture size into two parts:
		// one for the left eye and the other one for the right eye.
		//
		EyeViewPorts eyeViewPorts;

		eyeViewPorts[ovrEye_Left] = ovr::Recti(ovr::Vector2i(0), leftEyeTextureSize);
		eyeViewPorts[ovrEye_Right] = ovr::Recti(ovr::Vector2i((fullTextureSize.w + 1) / 2, 0), rightEyeTextureSize);

		// Initialize OVR eye textures.
		//
		auto initEyeTexture = [&](ovrEyeType eye) // create an eye initialization lambda
		{
			auto & textureHeader = m_eyeTextures[eye].OGL.Header;

			textureHeader.API = ovrRenderAPI_OpenGL;
			textureHeader.RenderViewport = eyeViewPorts[eye];
			textureHeader.TextureSize = fullTextureSize;

			m_eyeTextures[eye].OGL.TexId = m_fbo->texture();
		};
		rift_utils::foreach_eye(initEyeTexture); // run the lambda for each eye

		// Configure OVR to use OpenGL for rendering.
		//
		ovrGLConfig config;
		memset(&config, 0, sizeof(config));

		auto & header = config.OGL.Header;
		header.API = ovrRenderAPI_OpenGL;
		header.BackBufferSize = ovr::Sizei(m_hmdDescriptor->Resolution.w, m_hmdDescriptor->Resolution.h);

		int const distortionCaps = ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive;

		if (!ovrHmd_ConfigureRendering(m_hmdDescriptor, &config.Config, distortionCaps, m_hmdDescriptor->DefaultEyeFov, m_eyeDescriptors.data()))
		{
			throw Exception(error_caption::ovrInit, ovrHmd_GetLastError(m_hmdDescriptor));
		}

		// Finally, attach OVR to the current window.
		//
		if (!ovrHmd_AttachToWindow(m_hmdDescriptor, getWindowHandle(), nullptr, nullptr))
		{
			throw Exception(error_caption::ovrInit, ovrHmd_GetLastError(m_hmdDescriptor));
		}

		// Initialize the time for the frames counter.
		//
		m_framesCounterLastTime = ovr_GetTimeInSeconds();
	}

	void RiftApp::startFrame()
	{
		OpenGlApp::startFrame();

		auto const frameTiming = ovrHmd_BeginFrame(m_hmdDescriptor, 0);
		m_frameTimeDelta = frameTiming.DeltaSeconds;

		// Calculate and output computation speed.
		//
		auto framesCounterCurrentTime = ovr_GetTimeInSeconds();
		++m_framesCounter;

		if (framesCounterCurrentTime - m_framesCounterLastTime >= 1.0)
		{
			double frameTime = 1000.0 / static_cast<double>(m_framesCounter);

			std::wcout << frameTime << L" ms/frame" << std::endl;
			m_framesCounter = 0;
			m_framesCounterLastTime += 1.0;

			m_frameTimes.push_back(frameTime);
		}

		// All the rendering will go into the frame buffer object.
		//
		m_fbo->bind();
	}

	void RiftApp::finishFrame()
	{
		m_fbo->release();

		// Submit the eye images for SDK distortion processing.
		//
		ovrHmd_EndFrame(m_hmdDescriptor, m_eyePoses.data(), &m_eyeTextures[0].Texture);
	}

	void RiftApp::renderFrame()
	{
		// The buffers should always be cleared before rendering.
		//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		ovrHmd_GetEyePoses(m_hmdDescriptor, 0, getEyeViewOffsets().data(), m_eyePoses.data(), nullptr);
		EyeViewPorts const eyeViewPorts = getEyeViewPorts();

		// Create a lambda to render an eye.
		//
		auto renderEyeTexture = [&](ovrEyeType eye)
		{
			// Get the eye, that is currently recommended for rendering by the SDK.
			//
			ovrEyeType currentEye = m_hmdDescriptor->EyeRenderOrder[eye];

			glViewport(eyeViewPorts[currentEye].x, eyeViewPorts[currentEye].y, eyeViewPorts[currentEye].w, eyeViewPorts[currentEye].h);

			// Get the perspective projection.
			//
			float const nearClipping = 0.1f;
			float const farClipping = 1000.0f;
			QMatrix4x4 const projection(&ovrMatrix4f_Projection(m_eyeDescriptors[currentEye].Fov, nearClipping, farClipping, true).M[0][0]);

			// Rotate and position the view camera.
			//
			ovr::Posef const currentEyePose(m_eyePoses[currentEye]);
			ovr::Vector3f const up = currentEyePose.Rotation.Rotate(ovr::Vector3f(0.0f, 1.0f, 0.0f));
			ovr::Vector3f const forward = currentEyePose.Rotation.Rotate(ovr::Vector3f(0.0f, 0.0f, -1.0f));
			ovr::Vector3f const viewPos = m_avatarPosition + currentEyePose.Translation;

			QMatrix4x4 const view(&ovr::Matrix4f::LookAtRH(viewPos, viewPos + forward, up).M[0][0]);

			renderScene(currentEye, view, projection, m_frameTimeDelta);
		};

		// Run the lambda for each eye.
		//
		rift_utils::foreach_eye(renderEyeTexture);
	}

	void RiftApp::keyReleaseEvent(QKeyEvent * ev)
	{
		if (isOvrHealthAndSafetyWarningDisplayed())
		{
			dismissOvrHealthAndSafetyWarning();
		}
		else
		{
			if (ev->key() == Qt::Key_Escape)
			{
				close();
			}
		}
	}

	void RiftApp::startOvr()
	{
		Q_ASSERT(!isOvrStarted());

		if (!ovr_Initialize())
		{
			throw Exception(error_caption::ovrStart, QString(ovrHmd_GetLastError(nullptr)));
		}
		auto ovrGuard = guardScope([]{ ovr_Shutdown(); });

		int const defaultDevice = 0;
		ovrHmd hmdDescriptor = ovrHmd_Create(defaultDevice);

		if (hmdDescriptor == nullptr)
		{
			std::wcout << warning_caption::ovrInit.toStdWString() << L" >> description: failed to create HMD, launching debug mode" << std::endl;

			hmdDescriptor = ovrHmd_CreateDebug(ovrHmd_DK2);

			if (hmdDescriptor == nullptr)
			{
				throw Exception(error_caption::ovrStart, QString(ovrHmd_GetLastError(nullptr)));
			}
		}
		auto hmdDescriptorGuard = guardScope([hmdDescriptor]{ ovrHmd_Destroy(hmdDescriptor); });

		unsigned int const trackingCapabilityBits = ovrTrackingCap_Orientation | ovrTrackingCap_Position | ovrTrackingCap_MagYawCorrection;

		if (!ovrHmd_ConfigureTracking(hmdDescriptor, trackingCapabilityBits, trackingCapabilityBits))
		{
			throw Exception(error_caption::ovrStart, (ovrHmd_GetLastError(hmdDescriptor)));
		}

		hmdDescriptorGuard.dismiss();
		ovrGuard.dismiss();

		m_hmdDescriptor = hmdDescriptor;
	}

	bool RiftApp::isOvrStarted() const
	{
		return m_hmdDescriptor != nullptr;
	}

	void RiftApp::finishOvr()
	{
		Q_ASSERT(!isOvrFinished());

		ovrHmd_Destroy(m_hmdDescriptor);
		m_hmdDescriptor = nullptr;

		ovr_Shutdown();
	}

	bool RiftApp::isOvrFinished() const
	{
		return m_hmdDescriptor == nullptr;
	}

	void RiftApp::setOvrGeometry()
	{
		Q_ASSERT(isOvrStarted());

		auto const & position = m_hmdDescriptor->WindowsPos;
		auto const & resolution = m_hmdDescriptor->Resolution;

		setGeometry(position.x, position.y, resolution.w, resolution.h);
	}

	HWND RiftApp::getWindowHandle()
	{
		return reinterpret_cast<HWND>(winId());
	}

	RiftApp::EyeViewPorts RiftApp::getEyeViewPorts() const
	{
		EyeViewPorts const eyeViewPorts = { m_eyeTextures[0].OGL.Header.RenderViewport, m_eyeTextures[1].OGL.Header.RenderViewport };
		return eyeViewPorts;
	}

	RiftApp::EyeViewOffsets RiftApp::getEyeViewOffsets() const
	{
		EyeViewOffsets const eyeViewOffsets = { m_eyeDescriptors[0].HmdToEyeViewOffset, m_eyeDescriptors[1].HmdToEyeViewOffset };
		return eyeViewOffsets;
	}

	bool RiftApp::isOvrHealthAndSafetyWarningDisplayed() const
	{
		ovrHSWDisplayState hswDisplayState;
		ovrHmd_GetHSWDisplayState(m_hmdDescriptor, &hswDisplayState);

		return hswDisplayState.Displayed != 0;
	}

	void RiftApp::dismissOvrHealthAndSafetyWarning()
	{
		Q_ASSERT(isOvrHealthAndSafetyWarningDisplayed());

		ovrHmd_DismissHSWDisplay(m_hmdDescriptor);
	}

}
