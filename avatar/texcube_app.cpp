#include "stdafx.h"

#include "texcube_app.h"

#include "opengl_utils.h"

namespace avatar
{

	TexCubeApp::TexCubeApp()
		: m_verticesBuffer(QOpenGLBuffer::VertexBuffer)
		, m_uvsBuffer(QOpenGLBuffer::VertexBuffer)
		, m_texture(QOpenGLTexture::Target2D)
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

	void TexCubeApp::uninitializeApp()
	{
		RiftApp::uninitializeApp();
	}

	void TexCubeApp::initializeApp()
	{
		RiftApp::initializeApp();

		glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

		glEnable(GL_DEPTH_TEST); // enable depth test
		glDepthFunc(GL_LESS); // accept a fragment if it is closer to the camera than the former one
		glEnable(GL_CULL_FACE); // cull triangles which normal is not towards the camera

		opengl_utils::ObjectData data;
		opengl_utils::deserializeObjectData("cube_tex.bin", data);

		m_vao.create();
		m_vao.bind();

		m_verticesBuffer.create();
		m_verticesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_verticesBuffer.bind();
		m_verticesBuffer.allocate(data.vertices.data(), data.vertices.size() * sizeof(QVector3D));
		m_verticesBuffer.release();

		m_uvsBuffer.create();
		m_uvsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_uvsBuffer.bind();
		m_uvsBuffer.allocate(data.uvs.data(), data.uvs.size() * sizeof(QVector2D));
		m_uvsBuffer.release();

		initShaderProgram();

		m_shaderProgram.bind();

		m_verticesBuffer.bind();
		m_shaderProgram.enableAttributeArray(m_vertexPosAttr);
		m_shaderProgram.setAttributeBuffer(m_vertexPosAttr, GL_FLOAT, 0, 3);
		m_verticesBuffer.release();

		m_uvsBuffer.bind();
		m_shaderProgram.enableAttributeArray(m_vertexUvAttr);
		m_shaderProgram.setAttributeBuffer(m_vertexUvAttr, GL_FLOAT, 0, 2);
		m_uvsBuffer.release();

		m_shaderProgram.release();
		m_vao.release();

		//
		// Setup the texture using a PNG image.
		// Note that the QImage is mirrored vertically to account for the fact,
		// that OpenGL and QImage use opposite directions for the y axis.
		//
		m_texture.setData(QImage("cube_map.png").mirrored());
		m_texture.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		m_texture.setMagnificationFilter(QOpenGLTexture::Linear);

		setAnimating(true);
	}

	void TexCubeApp::renderScene(ovrEyeType renderedEye, QMatrix4x4 const & view, QMatrix4x4 const & projection, float timeDelta)
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

		m_texture.bind();

		m_vao.bind();

		glDrawArrays(GL_TRIANGLES, 0, 36);

		m_vao.release();

		m_texture.release();

		m_shaderProgram.release();
	}

	void TexCubeApp::initShaderProgram()
	{
		QString const vertexShaderSource("#version 330 core\n"
		"layout(location = 0) in vec3 vertexPosition;\n"
		"layout(location = 1) in vec2 vertexUV;\n"
		"out vec2 UV;\n"
		"uniform mat4 MVP;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = MVP * vec4(vertexPosition, 1);\n"
		"    UV = vertexUV;\n"
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

		m_vertexPosAttr = m_shaderProgram.attributeLocation("vertexPosition");
		m_vertexUvAttr = m_shaderProgram.attributeLocation("vertexUV");
		m_mvpUniform = m_shaderProgram.uniformLocation("MVP");
	}

}
