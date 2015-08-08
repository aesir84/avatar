#pragma once

#include "opengl_app.h"

namespace ovr = OVR;

namespace avatar
{

	/// \brief A class for Oculus SDK interaction
	///
	/// The aim of this class is to interact with Oculus SDK
	/// in order to perform a number of different tasks including
	/// the initialization and shutdown of Oculus runtime environment,
	/// as well as rendering to the Rift.
	///
	class RiftApp : public OpenGlApp
	{

		Q_OBJECT

	public:

		/// \brief A class's constructor
		///
		/// The constructor makes basic OVR initializations.
		/// It also relies on the fact that no OpenGL initialization
		/// occurs in the constructor of the base class, meaning that no
		/// context creation, resolving of OpenGL function pointers, etc.
		/// should occur before the basic initialization of OVR.
		/// Under basic initialization of OVR the following things are meant:
		/// ovr_Initialize, ovrHmd_Create and ovrHmd_ConfigureTracking.
		///
		RiftApp();
		~RiftApp();

	public slots:

		virtual void uninitializeApp() override;

	protected:

		/// \brief Overriden initializeApp() method from OpenGlApp class
		///
		/// The method initializes and configures different kinds of OVR features.
		/// One thing worth noting is that currently a single texture buffer is
		/// shared by both eyes. In other words, the same texture is used,
		/// but with different viewports.
		///
		virtual void initializeApp() override;

		/// \brief Overriden resizeApp() method from OpenGlApp class
		///
		virtual void resizeApp(int width, int height) override;

		/// \brief Overriden startFrame() method from OpenGlApp class
		///
		/// The method starts the rendering of a frame.
		/// It is required to call the superclass's version of this
		/// method in the first place.
		///
		virtual void startFrame() override;

		/// \brief Overriden finishFrame() method from OpenGlApp class
		///
		/// The method finishes the rendering of a frame.
		/// When using ovrHmd_EndFrame() function, provided by OVR,
		/// there is no need to manually swap the buffers,
		/// since OVR does it in terms of ovrHmd_EndFrame() function call.
		/// Therefore, the superclass's version of this method should not
		/// be called.
		///
		virtual void finishFrame() override;

		/// \brief Overriden renderFrame() method from OpenGlApp class
		///
		/// The method overriding is final, because the current class
		/// needs to take the full control of OVR rendering details.
		/// In order to provide the means for actual OpenGL drawing,
		/// another pure virtual method is introduced - renderScene().
		/// \sa renderScene()
		///
		virtual void renderFrame() override final;

		/// \brief A method to perform the actual drawing
		///
		/// The method is pure virtual, so it is up to subclasses to
		/// define the implementation - to do the drawing work.
		/// This method is called from renderFrame() method.
		/// \sa renderFrame()
		///
		virtual void renderScene(ovrEyeType renderedEye, QMatrix4x4 const & view, QMatrix4x4 const & projection, float timeDelta) = 0;

	protected:

		virtual void keyReleaseEvent(QKeyEvent * ev) override;

	private:

		using EyeDescriptors = std::array<ovrEyeRenderDesc, ovrEye_Count>;
		using EyeTextures = std::array<ovrGLTexture, ovrEye_Count>;
		using EyePoses = std::array<ovrPosef, ovrEye_Count>;
		using EyeViewPorts = std::array<ovr::Recti, ovrEye_Count>;
		using EyeViewOffsets = std::array<ovrVector3f, ovrEye_Count>;
		using EyeProjectionMatrices = std::array<QMatrix4x4, ovrEye_Count>;

	private:

		void startOvr();
		bool isOvrStarted() const;
		void finishOvr();
		bool isOvrFinished() const;

		void setOvrGeometry();

		bool reconfigureOvrScreenRendering();

		/// \brief Configuration of offscreen rendering parameters
		///
		/// This helping method handles the creation of a framebuffer object
		/// and the configuration of textures that are needed to create this object.
		/// The method should be called once from initializeApp() method,
		/// since there are no plans to alter the textures' sizes or parameters later on.
		///
		bool configureOvrOffscreenRendering();

		/// \brief Methods to conveniently extract data from other data structures
		///
		EyeViewPorts getEyeViewPorts() const;
		EyeViewOffsets getEyeViewOffsets() const;

		HWND getWindowHandle();

		bool isOvrHealthAndSafetyWarningDisplayed() const;
		void dismissOvrHealthAndSafetyWarning();

	private:

		ovrHmd m_hmdDescriptor;

		EyeProjectionMatrices m_eyeProjections;
		EyeDescriptors m_eyeDescriptors;
		EyeTextures m_eyeTextures;
		EyePoses m_eyePoses;

		ovr::Vector3f const m_avatarPosition;

		std::unique_ptr<QOpenGLFramebufferObject> m_fbo;

		float m_frameTimeDelta;
		int m_framesCounter;
		double m_framesCounterLastTime;
		std::vector<double> m_frameTimes;

	};

}
