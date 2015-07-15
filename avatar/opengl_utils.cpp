#include "stdafx.h"

#include "opengl_utils.h"

#include "exception.h"

namespace avatar
{

	namespace opengl_utils
	{

		namespace
		{

			namespace error_caption
			{
				QString const fileIo("File IO error");
				QString const fileParsing("File parsing error");
			}

			std::vector<QVector3D> getVertices(QTextStream & stream)
			{
				std::vector<QVector3D> vertices;

				size_t lineNumber = 1;

				while (!stream.atEnd())
				{
					QString line = stream.readLine();
					QStringList const lineTokens = line.split(" ");

					QString const vertexCoordsLineKey("v");

					if (lineTokens.isEmpty() || lineTokens[0] != vertexCoordsLineKey)
					{
						continue;
					}

					int const vertexCoordsLineKeyCount = 1;
					int const vertexCoordsCount = 3;

					int const vertexCoordsLineTokenCount = vertexCoordsLineKeyCount + vertexCoordsCount;

					if (lineTokens.size() != vertexCoordsLineTokenCount)
					{
						throw Exception(error_caption::fileParsing, QString("invalid vertex definition at line %1").arg(lineNumber));
					}

					std::vector<float> vertexCoords(vertexCoordsCount);

					for (int vertexCoordsIter = 0, lineTokensIter = 1; vertexCoordsIter != vertexCoordsCount; ++vertexCoordsIter, ++lineTokensIter)
					{
						bool isFloatValid = false;
						float const vertexCoord = lineTokens[lineTokensIter].toFloat(&isFloatValid);

						if (!isFloatValid)
						{
							throw Exception(error_caption::fileParsing, QString("invalid vertex coordinate definition at line %1").arg(lineNumber));
						}

						vertexCoords[vertexCoordsIter] = vertexCoord;
					}

					vertices.push_back(QVector3D(vertexCoords[0], vertexCoords[1], vertexCoords[2]));
					++lineNumber;
				}

				stream.seek(0);
				return vertices;
			}

			std::vector<QVector2D> getUvs(QTextStream & stream)
			{
				std::vector<QVector2D> uvs;

				size_t lineNumber = 1;

				while (!stream.atEnd())
				{
					QString line = stream.readLine();
					QStringList const lineTokens = line.split(" ");

					QString const uvCoordsLineKey("vt");

					if (lineTokens.isEmpty() || lineTokens[0] != uvCoordsLineKey)
					{
						continue;
					}

					int const uvCoordsLineKeyCount = 1;
					int const uvCoordsCount = 2;

					int const uvCoordsLineTokenCount = uvCoordsLineKeyCount + uvCoordsCount;

					if (lineTokens.size() != uvCoordsLineTokenCount)
					{
						throw Exception(error_caption::fileParsing, QString("invalid UV definition at line %1").arg(lineNumber));
					}

					std::vector<float> uvCoords(uvCoordsCount);

					for (int uvCoordsIter = 0, lineTokensIter = 1; uvCoordsIter != uvCoordsCount; ++uvCoordsIter, ++lineTokensIter)
					{
						bool isFloatValid = false;
						float const uvCoord = lineTokens[lineTokensIter].toFloat(&isFloatValid);

						if (!isFloatValid)
						{
							throw Exception(error_caption::fileParsing, QString("invalid UV coordinate definition at line %1").arg(lineNumber));
						}

						uvCoords[uvCoordsIter] = uvCoord;
					}

					uvs.push_back(QVector2D(uvCoords[0], uvCoords[1]));
					++lineNumber;
				}

				stream.seek(0);
				return uvs;
			}

			typedef std::pair<int, int> Indices;

			std::vector<Indices> getIndices(QTextStream & stream)
			{
				std::vector<Indices> indices;

				size_t lineNumber = 1;

				while (!stream.atEnd())
				{
					QString line = stream.readLine();
					QStringList const lineTokens = line.split(" ");

					QString const faceLineKey("f");

					if (lineTokens.isEmpty() || lineTokens[0] != faceLineKey)
					{
						continue;
					}

					int const faceLineKeyCount = 1;
					int const triangleFacePointsCount = 3;

					int const faceLineTokenCount = faceLineKeyCount + triangleFacePointsCount;

					if (lineTokens.size() != faceLineTokenCount)
					{
						throw Exception(error_caption::fileParsing, QString("invalid face definition at line %1").arg(lineNumber));
					}

					for (int triangleFacePointsIter = 0, lineTokensIter = 1; triangleFacePointsIter != triangleFacePointsCount; ++triangleFacePointsIter, ++lineTokensIter)
					{
						QStringList const facePointCoords = lineTokens[lineTokensIter].split("/");
						int const facePointCoordsCount = 2;

						if (facePointCoords.size() != facePointCoordsCount)
						{
							throw Exception(error_caption::fileParsing, QString("invalid face coordinates at line %1").arg(lineNumber));
						}

						bool isIntegerValid = false;

						int const vertexIdx = facePointCoords[0].toInt(&isIntegerValid);

						if (!isIntegerValid)
						{
							throw Exception(error_caption::fileParsing, QString("invalid face vertex index at line %1").arg(lineNumber));
						}

						int const uvIdx = facePointCoords[1].toInt(&isIntegerValid);

						if (!isIntegerValid)
						{
							throw Exception(error_caption::fileParsing, QString("invalid face UV index at line %1").arg(lineNumber));
						}

						//
						// Keep in mind, that in an OBJ file the indeces start from 1. Not from 0.
						// That's why, we need to correct that by subtracting 1 from the corresponding index.
						//
						indices.push_back(std::make_pair(vertexIdx - 1, uvIdx - 1));
					}

					++lineNumber;
				}
				stream.seek(0);
				return indices;
			}

		}

		void deserializeObjectData(QString const & fileName, ObjectData & data)
		{
			QFile file(fileName);

			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				throw Exception(error_caption::fileIo, QString("unable to open the file %1").arg(fileName));
			}

			QTextStream stream(&file);

			std::vector<QVector3D> tempVertices = getVertices(stream);
			std::vector<QVector2D> tempUvs = getUvs(stream);
			std::vector<Indices> indices = getIndices(stream);

			for (auto const & idx : indices)
			{
				data.vertices.push_back(tempVertices[idx.first]);
				data.uvs.push_back(tempUvs[idx.second]);
			}
		}

	}

}