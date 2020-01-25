#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include "GMath.h"

namespace Granite
{
	class ModelLoader 
	{
	public:
		static Mesh Load()
		{
			std::ifstream file("Jess.obj");

			Mesh mesh;

			std::vector<FVector3> vertices = {};
			std::vector<FVector3> textureCoords = {};
			std::vector<FVector3> normals = {};

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

		static void _LoadData(std::string line, std::vector<FVector3> &storage)
		{
			int phase = 0;
			float storageData[3] = {};
			FVector3 tempStorageVector; // TODO: Remove after fVector classification
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
		static void _LoadPoly(std::string line, const std::vector<FVector3> &vertices, const std::vector<FVector3>& textureCoords, const std::vector<FVector3>& normals, Mesh &mesh)
		{
			int dataCountIndex = 0;
			std::stringstream lineData(line);
			std::string data;
			std::string value;
			lineData >> data;

			FVector3 vertexIndices[3];
			FVector3 textureIndices[3];
			FVector3 normalIndices[3];

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
							vertexIndices[dataCountIndex].SetData(vertices.at(std::stoi(value) - 1));
							break;
						case 1:
							textureIndices[dataCountIndex].SetData(textureCoords.at(std::stoi(value) - 1));
							break;
						case 2:
							normalIndices[dataCountIndex].SetData(normals.at(std::stoi(value) - 1));
							break;
						}
					}
					++phase;
				}
				++dataCountIndex;
			}

			mesh.AddPolygon(vertexIndices, textureIndices, normalIndices);
		}
	};
}