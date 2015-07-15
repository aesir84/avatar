#pragma once

namespace avatar
{

	namespace opengl_utils
	{

		struct ObjectData
		{
			std::vector<QVector3D> vertices;
			std::vector<QVector2D> uvs;
		};

		void deserializeObjectData(QString const & fileName, ObjectData & data);

	}

}
