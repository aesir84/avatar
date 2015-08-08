#include "stdafx.h"

#include "opengl_app.h"

namespace avatar
{

	OpenGlApp::OpenGlApp(QWindow * parent)
		: QWindow(parent)
		, m_isUpdatePending(false)
		, m_isAnimating(false)
	{
		// The window's surface type must be set to QSurface::OpenGLSurface
		// to indicate that the window is to be used for OpenGL rendering.
		//
		setSurfaceType(QWindow::OpenGLSurface);
	}

	OpenGlApp::~OpenGlApp()
	{}

	bool OpenGlApp::isAppInitialized() const
	{
		return m_context != nullptr;
	}

	void OpenGlApp::initializeApp()
	{
		Q_ASSERT(!isAppInitialized());

		m_context = std::make_unique<QOpenGLContext>(this);

		// By calling QWindow::requestedFormat() method we assume,
		// that the QWindow's format has already been properly set.
		//
		m_context->setFormat(requestedFormat());
		m_context->create();

		m_context->makeCurrent(this); // initializeOpenGLFunctions() requires the current context to be set
		initializeOpenGLFunctions();
	}

	void OpenGlApp::uninitializeApp()
	{ }

	void OpenGlApp::resizeApp(int, int)
	{ }

	void OpenGlApp::setAnimating(bool isAnimating)
	{
		m_isAnimating = isAnimating;

		if (isAnimating)
		{
			renderLater();
		}
	}

	void OpenGlApp::renderLater()
	{
		if (!m_isUpdatePending)
		{
			m_isUpdatePending = true;

			// Attention.
			// The event must be allocated on the heap since the post event
			// queue will take ownership of the event and delete it once it has been posted.
			// It is not safe to access the event after it has been posted.
			//
			QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
		}
	}

	void OpenGlApp::renderNow()
	{
		if (!isExposed())
		{
			return;
		}

		startFrame();
		renderFrame();
		finishFrame();

		if (m_isAnimating)
		{
			renderLater();
		}
	}

	void OpenGlApp::startFrame()
	{
		// It is important to make the context current
		// before beginning to render a new frame.
		//
		m_context->makeCurrent(this);
	}

	void OpenGlApp::finishFrame()
	{
		// Swap the back and front buffers of the surface.
		// Method swapBuffers() is called to finish the current frame.
		//
		m_context->swapBuffers(this);
	}

	bool OpenGlApp::event(QEvent * ev)
	{
		switch (ev->type())
		{
			case QEvent::UpdateRequest:
			{
				m_isUpdatePending = false;
				renderNow();

				return true;
			}
			default:
			{
				return QWindow::event(ev);
			}
		}
	}

	void OpenGlApp::exposeEvent(QExposeEvent * ev)
	{
		Q_UNUSED(ev);

		if (isExposed())
		{
			if (!isAppInitialized())
			{
				initializeApp();
				resizeApp(width(), height());
			}
			renderNow();
		}
	}

	void OpenGlApp::resizeEvent(QResizeEvent * ev)
	{
		if (isExposed() && isAppInitialized())
		{
			resizeApp(ev->size().width(), ev->size().height());
		}
	}

}
