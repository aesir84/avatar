#pragma once

#include "rift_app.h"

namespace avatar
{

	class TexCubeApp : public RiftApp
	{

		Q_OBJECT

	public:

		TexCubeApp();

	public slots:

		virtual void uninitializeApp() override;

	protected:

		virtual void initializeApp() override final;
		virtual void renderScene(ovrEyeType renderedEye, QMatrix4x4 const & view, QMatrix4x4 const & projection, float timeDelta) override final;

	private:

		void initShaderProgram();

	private:

		QOpenGLVertexArrayObject m_vao;

		QOpenGLBuffer m_verticesBuffer;
		QOpenGLBuffer m_uvsBuffer;

		QOpenGLTexture m_texture;

		QOpenGLShaderProgram m_shaderProgram;

		GLuint m_vertexPosAttr;
		GLuint m_vertexUvAttr;
		GLuint m_mvpUniform;

		float m_angle;

	};

}
