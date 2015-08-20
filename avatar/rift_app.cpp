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

	void RiftApp::resizeApp(int width, int height)
	{
		OpenGlApp::resizeApp(width, height);

		if (!reconfigureOvrScreenRendering())
		{
			throw Exception("App resizing logic", "failed to resize");
		}
	}

	void RiftApp::initializeApp()
	{
		OpenGlApp::initializeApp();

		if (!configureOvrOffscreenRendering())
		{
			throw Exception(error_caption::ovrInit, "failed to configure offscreen rendering");
		}

		if (!reconfigureOvrScreenRendering())
		{
			throw Exception(error_caption::ovrInit, "failed to configure screen rendering");
		}

		if (!ovrHmd_AttachToWindow(m_hmdDescriptor, getWindowHandle(), nullptr, nullptr))
		{
			throw Exception(error_caption::ovrInit, ovrHmd_GetLastError(m_hmdDescriptor));
		}


		//
		// Initialize eye projection matrices.
		//

		// These matrices remain constant, because neither of the parameters seems to change,
		// if one uses ovrMatrix4f_Projection function to create those matrices.
		// The runtime also does not seem to be affected, if these matrices are kept constant.
		//

		float const znear = 0.1f;
		float const zfar = 100.0f;

		m_eyeProjections[ovrEye_Left] = QMatrix4x4(&ovrMatrix4f_Projection(m_eyeDescriptors[ovrEye_Left].Fov, znear, zfar, ovrProjection_RightHanded).M[0][0]);
		m_eyeProjections[ovrEye_Right] = QMatrix4x4(&ovrMatrix4f_Projection(m_eyeDescriptors[ovrEye_Right].Fov, znear, zfar, ovrProjection_RightHanded).M[0][0]);


		//
		// Initialize runtime statistics variables.
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

		ovrTrackingState trackingState;
		ovrHmd_GetEyePoses(m_hmdDescriptor, 0, getEyeViewOffsets().data(), m_eyePoses.data(), &trackingState);

		// Provided the tracking is enabled, output some information about it.
		//
		if ((trackingState.StatusFlags & ovrStatus_OrientationTracked) != 0)
		{
			float yaw = 0.0f;
			float pitch = 0.0f;
			float roll = 0.0f;

			ovr::Posef const & headPose = trackingState.HeadPose.ThePose;
			headPose.Rotation.GetEulerAngles<ovr::Axis_Y, ovr::Axis_X, ovr::Axis_Z>(&yaw, &pitch, &roll);

			Q_EMIT orientationChanged(yaw, pitch, roll);
		}

		EyeViewPorts const eyeViewPorts = getEyeViewPorts();

		// Create a lambda to render an eye.
		//
		auto renderEyeTexture = [&](ovrEyeType eye)
		{
			// Get the eye, that is currently recommended for rendering by the SDK.
			//
			ovrEyeType currentEye = m_hmdDescriptor->EyeRenderOrder[eye];

			glViewport(eyeViewPorts[currentEye].x, eyeViewPorts[currentEye].y, eyeViewPorts[currentEye].w, eyeViewPorts[currentEye].h);

			// Rotate and position the view camera.
			//
			ovr::Posef const currentEyePose(m_eyePoses[currentEye]);
			ovr::Vector3f const up = currentEyePose.Rotation.Rotate(ovr::Vector3f(0.0f, 1.0f, 0.0f));
			ovr::Vector3f const forward = currentEyePose.Rotation.Rotate(ovr::Vector3f(0.0f, 0.0f, -1.0f));
			ovr::Vector3f const viewPos = m_avatarPosition + currentEyePose.Translation;

			QMatrix4x4 const view(&ovr::Matrix4f::LookAtRH(viewPos, viewPos + forward, up).M[0][0]);

			renderScene(currentEye, view, m_eyeProjections[currentEye], m_frameTimeDelta);
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

		if ((hmdDescriptor->HmdCaps & ovrHmdCap_ExtendDesktop) != 0)
		{
			throw Exception(error_caption::ovrStart, "extended display mode is not supported");
		}

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
		auto const & size = m_hmdDescriptor->Resolution;


		//
		// Since the only supported display mode is the direct one,
		// it is possible to make the window smaller.
		//

		Q_ASSERT((m_hmdDescriptor->HmdCaps & ovrHmdCap_ExtendDesktop) == 0);

		auto * scr = screen();

		// A sanity check.
		//
		Q_ASSERT(scr != nullptr);

		QSize scrSize(scr->size());

		// Decrease the size keeping the proportions.
		//
		int appWindowWidth = size.w / 2;
		int appWindowHeight = size.h / 2;

		while (appWindowWidth >= scrSize.width() || appWindowHeight >= scrSize.height())
		{
			appWindowWidth /= 2;
			appWindowHeight /= 2;
		}

		setGeometry(position.x, position.y, appWindowWidth, appWindowHeight);
	}

	bool RiftApp::reconfigureOvrScreenRendering()
	{

		//
		// Configure OVR main rendering parameters.
		//

		ovrGLConfig config;
		memset(&config, 0, sizeof(config));

		// Configure OVR to use OpenGL for rendering.
		//
		config.OGL.Header.API = ovrRenderAPI_OpenGL;

		// The size of the backbuffer is set to the size of the application window.
		// It is a more flexible approach allowing to set custom window sizes,
		// which are not bound to the HMD resolution.
		//
		config.OGL.Header.BackBufferSize = ovr::Sizei(width(), height());

		int const distortionCaps = ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive;

		return ovrHmd_ConfigureRendering(m_hmdDescriptor, &config.Config, distortionCaps, m_hmdDescriptor->DefaultEyeFov, m_eyeDescriptors.data());
	}

	bool RiftApp::configureOvrOffscreenRendering()
	{

		//
		// Retrieve eye textures' sizes recommended by OVR.
		//

		float const defaultPixelDensity = 1.0f;

		auto const defaultLeftEyeFov = m_hmdDescriptor->DefaultEyeFov[ovrEye_Left];
		auto const defaultRightEyeFov = m_hmdDescriptor->DefaultEyeFov[ovrEye_Right];

		auto const recommendedLeftEyeTextureSize = ovrHmd_GetFovTextureSize(m_hmdDescriptor, ovrEye_Left, defaultLeftEyeFov, defaultPixelDensity);
		auto const recommendedRightEyeTextureSize = ovrHmd_GetFovTextureSize(m_hmdDescriptor, ovrEye_Right, defaultRightEyeFov, defaultPixelDensity);


		//
		// Offscreen rendering uses a single texture, which is shared by both eyes.
		//

		// Calculate the size of a shared texture.
		//
		ovr::Sizei const fullTextureSize(recommendedLeftEyeTextureSize.w + recommendedRightEyeTextureSize.w, std::max(recommendedLeftEyeTextureSize.h, recommendedRightEyeTextureSize.h));

		// Create a frame buffer object using the offscreen rendering texture size.
		// The frame object also contains a depth buffer.
		//
		m_fbo = std::make_unique<QOpenGLFramebufferObject>(fullTextureSize.w, fullTextureSize.h, QOpenGLFramebufferObject::Depth);

		if (!m_fbo->isValid())
		{
			return false;
		}

		// Correct the texture sizes.
		// Do not draw more than needed or more than allowed due to hardware limitations.
		//
		ovr::Sizei const actualFullTextureSize(m_fbo->size().width(), m_fbo->size().height());
		ovr::Sizei const actualLeftEyeTextureSize = ovr::Sizei::Min(ovr::Sizei(actualFullTextureSize.w / 2, actualFullTextureSize.h), recommendedLeftEyeTextureSize);
		ovr::Sizei const actualRightEyeTextureSize = ovr::Sizei::Min(ovr::Sizei(actualFullTextureSize.w / 2, actualFullTextureSize.h), recommendedRightEyeTextureSize);


		//
		// Enable the texture sharing by setting the eyes' viewports accordingly.
		//

		// The eye viewports split the full texture size into two parts:
		// one for the left eye and
		// the other one for the right eye.
		//
		EyeViewPorts eyeViewPorts;
		eyeViewPorts[ovrEye_Left] = ovr::Recti(ovr::Vector2i(0), actualLeftEyeTextureSize);
		eyeViewPorts[ovrEye_Right] = ovr::Recti(ovr::Vector2i((actualFullTextureSize.w + 1) / 2, 0), actualRightEyeTextureSize);


		//
		// By the way, also initialize OVR eye textures.
		//

		auto initEyeTexture = [&](ovrEyeType eye) // create an eye texture initialization lambda
		{
			auto & textureHeader = m_eyeTextures[eye].OGL.Header;

			textureHeader.API = ovrRenderAPI_OpenGL;
			textureHeader.RenderViewport = eyeViewPorts[eye];
			textureHeader.TextureSize = actualFullTextureSize;

			m_eyeTextures[eye].OGL.TexId = m_fbo->texture();
		};
		rift_utils::foreach_eye(initEyeTexture); // run the lambda for each eye

		return true;
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
