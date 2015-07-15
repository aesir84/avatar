#pragma once

namespace avatar
{

	/// \brief A class that manages OpenGL interaction
	///
	/// The class defines the basics for windowing and rendering using OpenGL.
	/// Windowing system is provided by publicly deriving from QWindow class.
	/// By protectively inheriting QOpenGLFunctions interface the class as well
	/// as its subclasses gain access to raw OpenGL API functions.
	///
	class OpenGlApp : public QWindow, protected QOpenGLFunctions
	{

		Q_OBJECT

	public:

		explicit OpenGlApp(QWindow * parent = nullptr);
		~OpenGlApp();

	public slots:

		/// \brief Uninitialization method
		///
		/// The method is made a public slot to allow attaching signals to it,
		/// i.e. QCoreApplication::aboutToQuit()
		/// When subclasses override this method, they should still call the superclass
		/// version at the end to properly release resources.
		///
		virtual void uninitializeApp();

	protected:

		void setAnimating(bool isAnimating);

	protected:

		/// \brief Initialization method
		///
		/// The method contains actions required to initialize the functionality of the class.
		/// The method is called right before the window gets exposed and rendered on the screen.
		/// The implementation of this method is very class specific, but it is important
		/// that when subclasses override this method, they still call the superclass
		/// version in the first place to gain the proper initialization.
		///
		virtual void initializeApp();

		/// \brief A method to perform resizing actions
		///
		/// This method is called from resizeEvent().
		/// \sa resizeEvent()
		///
		virtual void resizeApp(int width, int height);

		virtual void startFrame();

		/// \brief Rendering logic between startFrame() and finishFrame()
		///
		/// Any onscreen or offscreen drawing must take place in this method.
		/// The method is pure virtual and is not implemented in this class.
		/// Thus, the derived classes are totally responsible for the drawing.
		///
		virtual void renderFrame() = 0;

		virtual void finishFrame();

	protected:

		/// \brief Reimplemented method from QObject super class
		///
		virtual bool event(QEvent * ev) override final;

		/// \brief Reimplemented method from QWindow super class
		///
		/// The exposeEvent() is the notification to the window that its exposure,
		/// meaning visibility, on the screen has changed.
		/// This method is triggered the first time the window is about to be shown
		/// and each time the window is restored after being minimized.
		/// It is the place where OpenGL initialization and an initial resizing can take place.
		/// The initialization is performed only if it hasn't been already done.
		/// Therefore, there is a check provided by isAppInitialized() method.
		/// \sa isAppInitialized()
		/// \sa initializeApp()
		/// \sa resizeApp()
		///
		virtual void exposeEvent(QExposeEvent * ev) override final;

		/// \brief Resizing logic trigger
		///
		/// The method triggers the resizing logic of the app.
		/// There are constraints though:
		/// 1. the window must be exposed
		/// 2. OpenGL must be already initialized
		/// If these two requirements are not fulfilled,
		/// then the method does nothing.
		/// The first requirement is needed, because the resize event
		/// comes when the window is closed, and it seems rather strange
		/// to apply any resizing logic when the app is about to be destroyed.
		/// The second requirement is imposed by the fact,
		/// that in Qt the resize event will always come before the expose event
		/// when the app is launched. Keeping in mind, that the whole OpenGL initialization
		/// is performed in terms of exposeEvent() method, it is crucial to place such a requirement.
		/// Otherwise OpenGL resizing logic will fail to call gl* functions.
		///
		virtual void resizeEvent(QResizeEvent * ev) override final;

	private:

		/// \brief A method for scheduling the rendering for a later time
		///
		/// The renderLater() function simply puts an update request event on the event loop,
		/// which leads to renderNow() being called once the event gets processed.
		///
		void renderLater();

		/// \brief Actual rendering method
		///
		/// If the window is exposed, then a typical rendering scheme is executed,
		/// starting with startFrame() method,
		/// then performing the rendering itself with render(),
		/// and finalizing the whole thing with finishFrame() method.
		/// The method is also responsible for animation - if the flag m_isAnimating
		/// has been set by the call to setAnimating(), then the method will renderLater()
		/// at the end of its execution.
		/// \sa startFrame()
		/// \sa render()
		/// \sa finishFrame()
		///
		void renderNow();

		bool isAppInitialized() const;

	private:

		/// \brief Both flags deal with the animation of the rendered scene
		///
		bool m_isUpdatePending;
		bool m_isAnimating;

		/// \brief OpenGL context
		///
		/// The context object is stored as a pointer to delay the context construction.
		/// The whole OpenGL initialization (context, frame buffers, textures, etc) must
		/// be performed in initializeGl() method, which is called only when the window
		/// gets actually exposed on the screen for the first time.
		///
		std::unique_ptr<QOpenGLContext> m_context;

	};

}
