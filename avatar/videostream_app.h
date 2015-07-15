#pragma once

#include "rift_app.h"

namespace avatar
{

	class VideoStreamApp : public RiftApp
	{

		Q_OBJECT

	public:

		VideoStreamApp();
		~VideoStreamApp();

	protected:

		virtual void initializeApp() override;
		virtual void renderScene(ovrEyeType renderedEye, QMatrix4x4 const & view, QMatrix4x4 const & projection, float timeDelta) override final;

		virtual void bindEyeTexture(ovrEyeType renderedEye) = 0;
		virtual void releaseEyeTexture(ovrEyeType renderedEye) = 0;

	private:

		void initShaderProgram();

	private:

		QOpenGLVertexArrayObject m_vao;
		QOpenGLBuffer m_verticesPositionBuffer;

		QOpenGLShaderProgram m_shaderProgram;

		GLuint m_vertexPositionAttribute;

	};

}