#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include "GMath/GMath.h"
#include "GMath/Mesh.h"
#include "GMath/FVector3.h"

namespace Granite
{
	class ModelLoader 
	{
	public:
		static GMath::Mesh Load()
		{
			std::ifstream file("Jess.obj");

			GMath::Mesh mesh;

			std::vector<GMath::FVector3> vertices = {};
			std::vector<GMath::FVector3> textureCoords = {};
			std::vector<GMath::FVector3> normals = {};

			if (file.is_open())
			{
				std::string line;

				while (getline(file, line))
				{
					std::string dataType = line.substr(0, line.find(" "));

					// vertex
					if (dataType == "v")
					{
						_LoadData(line, vertices);
					}
					// texture coordinate
					else if (dataType == "vt")
					{
						_LoadData(line, textureCoords);
					}
					// vertex normals
					else if (dataType == "vn")
					{
						_LoadData(line, normals);
					}
					// parameter space vertices
					else if (dataType == "vp")
					{
						
					}
					// polygonal face element
					else if (dataType == "f")
					{
						_LoadPoly(line, vertices, textureCoords, normals, mesh);
					}
					// line element
					else if (dataType == "l")
					{

					}
				}
			}

			return mesh;
		}
	private:

		static void _LoadData(std::string line, std::vector<GMath::FVector3> &storage)
		{
			int phase = 0;
			float storageData[3] = {};
			GMath::FVector3 tempStorageVector; // TODO: Remove after fVector classification
			std::stringstream lineData(line);
			std::string data;
			lineData >> data;

			while (lineData >> data)
			{
				storageData[phase++] = std::stof(data);
			}

			tempStorageVector.SetData(storageData[0], storageData[1], storageData[2]);
			storage.push_back(tempStorageVector);
		}

		// v/vt/vn
		static void _LoadPoly(std::string line, const std::vector<GMath::FVector3> &vertices, const std::vector<GMath::FVector3>& textureCoords, const std::vector<GMath::FVector3>& normals, GMath::Mesh &mesh)
		{
			int dataCountIndex = 0;
			std::stringstream lineData(line);
			std::string data;
			std::string value;
			lineData >> data;

			std::vector<GMath::FVector3> vertexIndices;
			std::vector<GMath::FVector3> textureIndices;
			std::vector<GMath::FVector3> normalIndices;

			while (lineData >> data)
			{
				std::stringstream sData(data);
				int phase = 0;
				
				while (getline(sData, value, '/'))
				{
					if (value != "")
					{
						switch (phase)
						{
							case 0:
								vertexIndices.push_back(vertices.at(std::stoi(value) - 1));
								break;
							case 1:
								textureIndices.push_back(textureCoords.at(std::stoi(value) - 1));
								break;
							case 2:
								normalIndices.push_back(normals.at(std::stoi(value) - 1));
								break;
						}
					}
					++phase;
				}
				++dataCountIndex;
			}

			// triangulate
			while (dataCountIndex >= 3)
			{
				GMath::FVector3 vertexIndicesArr[3];
				GMath::FVector3 textureIndicesArr[3];
				GMath::FVector3 normalIndicesArr[3];

				int triangleFirst = dataCountIndex - 3;
				int triangleLast = dataCountIndex - 1;

				for (int vi = dataCountIndex - 1, ai = 2; ai > 0; --vi, --ai)
				{
					vertexIndicesArr[ai] = vertexIndices[vi];
					textureIndicesArr[ai] = textureIndices[vi];
					normalIndicesArr[ai] = normalIndices[vi];
				}

				vertexIndicesArr[0] = vertexIndices[0];
				textureIndicesArr[0] = textureIndices[0];
				normalIndicesArr[0] = normalIndices[0];

				mesh.AddPolygon(vertexIndicesArr, textureIndicesArr, normalIndicesArr);

				--dataCountIndex;;
			}
		}
	};
}