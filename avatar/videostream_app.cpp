#include "stdafx.h"

#include "videostream_app.h"

#include "exception.h"
#include "imagesystem_typedefs.h"

namespace avatar
{

	namespace
	{
		// A simple square which consists of two triangles.
		//
		QVector<QVector3D> const verticesPositionBufferData = {
			{ -1.0f, -1.0f, 0.0f  },
			{  1.0f, -1.0f, 0.0f  },
			{ -1.0f,  1.0f, 0.0f  },
			{ -1.0f,  1.0f, 0.0f  },
			{  1.0f, -1.0f, 0.0f  },
			{  1.0f,  1.0f, 0.0f  },
		};
	}

	VideoStreamApp::VideoStreamApp()
		: m_verticesPositionBuffer(QOpenGLBuffer::VertexBuffer)
		, m_shaderProgram(this)
	{

		//
		// Set the required OpenGL surface format, i.e. depth buffer size, OpenGL version, etc.
		//

		QSurfaceFormat requiredFormat;

		requiredFormat.setDepthBufferSize(24);
		requiredFormat.setVersion(3, 3);
		setFormat(requiredFormat);
	}

	void VideoStreamApp::raiseVideoStreamException(QString const & exceptionDescription)
	{
		throw Exception("Video stream error", exceptionDescription);
	}

	void VideoStreamApp::initializeApp()
	{
		RiftApp::initializeApp();

		m_vao.create();
		m_vao.bind();

		m_verticesPositionBuffer.create();
		m_verticesPositionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_verticesPositionBuffer.bind();
		m_verticesPositionBuffer.allocate(verticesPositionBufferData.constData(), verticesPositionBufferData.size() * sizeof(QVector3D));
		m_verticesPositionBuffer.release();

		initShaderProgram();

		m_shaderProgram.bind();

		m_verticesPositionBuffer.bind();
		m_shaderProgram.enableAttributeArray(m_vertexPositionAttribute);
		m_shaderProgram.setAttributeBuffer(m_vertexPositionAttribute, GL_FLOAT, 0, 3);
		m_verticesPositionBuffer.release();

		m_shaderProgram.release();
		m_vao.release();

		setAnimating(true);

		qRegisterMetaType<ImagePtr>("ImagePtr");
	}

	void VideoStreamApp::renderScene(ovrEyeType renderedEye, QMatrix4x4 const & view, QMatrix4x4 const & projection, float timeDelta)
	{
		Q_UNUSED(view);
		Q_UNUSED(projection);
		Q_UNUSED(timeDelta);

		m_shaderProgram.bind();
		bindEyeTexture(renderedEye);
		m_vao.bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_vao.release();
		releaseEyeTexture(renderedEye);
		m_shaderProgram.release();
	}

	void VideoStreamApp::initShaderProgram()
	{
		QString const vertexShaderSource("#version 330 core\n"
			"layout(location = 0) in vec3 vertexPosition;\n"
			"out vec2 UV;\n"
			"void main()\n"
			"{\n"
			"    gl_Position = vec4(vertexPosition, 1);\n"
			"    UV = (vertexPosition.xy + vec2(1, 1)) / 2.0;\n"
			"}\n");

		QString const fragmentShaderSource("#version 330 core\n"
			"in vec2 UV;\n"
			"out vec3 color;\n"
			"uniform sampler2D textureSampler;\n"
			"void main()\n"
			"{\n"
			"    color = texture2D(textureSampler, UV).rgb;\n"
			"}\n");

		m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
		m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
		m_shaderProgram.link();

		m_vertexPositionAttribute = m_shaderProgram.attributeLocation("vertexPosition");
	}

}
