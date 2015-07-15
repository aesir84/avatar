#pragma once

#include "rift_app.h"

namespace avatar
{

	class ColorCubeApp : public RiftApp
	{

		Q_OBJECT

	public:

		ColorCubeApp();

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
		QOpenGLBuffer m_colorBuffer;

		QOpenGLShaderProgram m_shaderProgram;

		GLuint m_vertexPosAttr;
		GLuint m_vertexColAttr;
		GLuint m_mvpUniform;

		float m_angle;

	};

}
