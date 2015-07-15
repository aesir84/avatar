#include "stdafx.h"

#include "colorcube_app.h"

namespace avatar
{

	namespace
	{

		std::vector<QVector3D> const vertexPositionBufferData =
		{
			{ -1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ -1.0f, -1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ 1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f, -1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ 1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f, -1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f }
		};

		std::vector<QVector3D> const vertexColorBufferData = {
			{ 0.583f, 0.771f, 0.014f },
			{ 0.609f, 0.115f, 0.436f },
			{ 0.327f, 0.483f, 0.844f },
			{ 0.822f, 0.569f, 0.201f },
			{ 0.435f, 0.602f, 0.223f },
			{ 0.310f, 0.747f, 0.185f },
			{ 0.597f, 0.770f, 0.761f },
			{ 0.559f, 0.436f, 0.730f },
			{ 0.359f, 0.583f, 0.152f },
			{ 0.483f, 0.596f, 0.789f },
			{ 0.559f, 0.861f, 0.639f },
			{ 0.195f, 0.548f, 0.859f },
			{ 0.014f, 0.184f, 0.576f },
			{ 0.771f, 0.328f, 0.970f },
			{ 0.406f, 0.615f, 0.116f },
			{ 0.676f, 0.977f, 0.133f },
			{ 0.971f, 0.572f, 0.833f },
			{ 0.140f, 0.616f, 0.489f },
			{ 0.997f, 0.513f, 0.064f },
			{ 0.945f, 0.719f, 0.592f },
			{ 0.543f, 0.021f, 0.978f },
			{ 0.279f, 0.317f, 0.505f },
			{ 0.167f, 0.620f, 0.077f },
			{ 0.347f, 0.857f, 0.137f },
			{ 0.055f, 0.953f, 0.042f },
			{ 0.714f, 0.505f, 0.345f },
			{ 0.783f, 0.290f, 0.734f },
			{ 0.722f, 0.645f, 0.174f },
			{ 0.302f, 0.455f, 0.848f },
			{ 0.225f, 0.587f, 0.040f },
			{ 0.517f, 0.713f, 0.338f },
			{ 0.053f, 0.959f, 0.120f },
			{ 0.393f, 0.621f, 0.362f },
			{ 0.673f, 0.211f, 0.457f },
			{ 0.820f, 0.883f, 0.371f },
			{ 0.982f, 0.099f, 0.879f }
		};

	}

	ColorCubeApp::ColorCubeApp()
		: m_verticesBuffer(QOpenGLBuffer::VertexBuffer)
		, m_colorBuffer(QOpenGLBuffer::VertexBuffer)
		, m_shaderProgram(this)
		, m_angle(0.0f)
	{
		QSurfaceFormat requiredFormat;

		//
		// Set the required OpenGL surface format, i.e.
		// depth buffer size, OpenGL version, etc.
		//
		requiredFormat.setDepthBufferSize(24);
		requiredFormat.setVersion(3, 3);
		setFormat(requiredFormat);
	}

	void ColorCubeApp::uninitializeApp()
	{
		RiftApp::uninitializeApp();
	}

	void ColorCubeApp::initializeApp()
	{
		RiftApp::initializeApp();

		glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

		glEnable(GL_DEPTH_TEST); // enable depth test
		glDepthFunc(GL_LESS); // accept a fragment if it is closer to the camera than the former one
		glEnable(GL_CULL_FACE); // cull triangles which normal is not towards the camera

		m_vao.create();
		m_vao.bind();

		m_verticesBuffer.create();
		m_verticesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_verticesBuffer.bind();
		m_verticesBuffer.allocate(vertexPositionBufferData.data(), vertexPositionBufferData.size() * sizeof(QVector3D));
		m_verticesBuffer.release();

		m_colorBuffer.create();
		m_colorBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_colorBuffer.bind();
		m_colorBuffer.allocate(vertexColorBufferData.data(), vertexColorBufferData.size() * sizeof(QVector3D));
		m_colorBuffer.release();

		initShaderProgram();

		m_shaderProgram.bind();

		m_verticesBuffer.bind();
		m_shaderProgram.enableAttributeArray(m_vertexPosAttr);
		m_shaderProgram.setAttributeBuffer(m_vertexPosAttr, GL_FLOAT, 0, 3);
		m_verticesBuffer.release();

		m_colorBuffer.bind();
		m_shaderProgram.enableAttributeArray(m_vertexColAttr);
		m_shaderProgram.setAttributeBuffer(m_vertexColAttr, GL_FLOAT, 0, 3);
		m_colorBuffer.release();

		m_shaderProgram.release();
		m_vao.release();

		setAnimating(true);
	}

	void ColorCubeApp::renderScene(ovrEyeType renderedEye, QMatrix4x4 const & view, QMatrix4x4 const & projection, float timeDelta)
	{
		Q_UNUSED(renderedEye);

		QMatrix4x4 model;
		model.scale(2.0f);

		m_angle = static_cast<GLfloat>(fmod(m_angle + (timeDelta * 5.0), 360.0));

		model.rotate(m_angle, QVector3D(1.0f, 0.0f, 0.0f));
		model.rotate(m_angle, QVector3D(0.0f, 1.0f, 0.0f));

		QMatrix4x4 const mvp = projection * view * model;

		m_shaderProgram.bind();
		m_shaderProgram.setUniformValue(m_mvpUniform, mvp);

		m_vao.bind();

		glDrawArrays(GL_TRIANGLES, 0, 36);

		m_vao.release();
		m_shaderProgram.release();
	}

	void ColorCubeApp::initShaderProgram()
	{
		QString const vertexShaderSource("#version 330 core\n"
			"layout(location = 0) in vec3 vertexPosition;\n"
			"layout(location = 1) in vec3 vertexColor;\n"
			"out vec3 fragmentColor;\n"
			"uniform mat4 MVP;\n"
			"void main()\n"
			"{\n"
			"    gl_Position = MVP * vec4(vertexPosition, 1);\n"
			"    fragmentColor = vertexColor;\n"
			"}\n");

		QString const fragmentShaderSource("#version 330 core\n"
			"in vec3 fragmentColor;\n"
			"out vec3 color;\n"
			"void main()\n"
			"{\n"
			"    color = fragmentColor;\n"
			"}\n");

		m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
		m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
		m_shaderProgram.link();

		m_vertexPosAttr = m_shaderProgram.attributeLocation("vertexPosition");
		m_vertexColAttr = m_shaderProgram.attributeLocation("vertexColor");
		m_mvpUniform = m_shaderProgram.uniformLocation("MVP");
	}

}
