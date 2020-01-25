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
		// zapravo vraca mesh!
		static Mesh Load()
		{
			std::ifstream file("Jess.obj");

			Mesh mesh;

			std::vector<FVector3> vertices = {};

			if (file.is_open())
			{
				std::string line;

				while (getline(file, line))
				{
					std::string dataType = line.substr(0, line.find(" "));

					// vertex
					if (dataType == "v")
					{
						_LoadVertex(line, vertices);
					}
					// texture coordinate
					else if (dataType == "vt")
					{

					}
					// vertex normals
					else if (dataType == "vn")
					{

					}
					// parameter space vertices
					else if (dataType == "vp")
					{

					}
					// polygonal face element
					else if (dataType == "f")
					{
						_LoadPoly(line, vertices, mesh);
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

		static void _LoadVertex(std::string line, std::vector<FVector3> &vertices)
		{
			int phase = 0;
			float vectorData[3] = {};
			FVector3 vertexVector;
			std::stringstream lineData(line);
			std::string data;
			lineData >> data;

			while (lineData >> data)
			{
				vectorData[phase++] = std::stof(data);
			}

			vertexVector.SetData(vectorData[0], vectorData[1], vectorData[2]);
			vertices.push_back(vertexVector);
		}

		// v/vt/vn
		static void _LoadPoly(std::string line, const std::vector<FVector3> &vertices, Mesh &mesh)
		{
			int dataCountIndex = 0;
			std::stringstream lineData(line);
			std::string data;
			std::string value;
			lineData >> data;

			Triangle vertexIndices;
			Triangle textureIndices;
			Triangle normalIndices;
			int pos = 0;

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
							vertexIndices.vertices[dataCountIndex].SetData(vertices.at(std::stoi(value) - 1));
							break;
						case 1:
							break;
						case 2:
							break;
						}
					}
					++phase;
				}
				++dataCountIndex;
			}

			mesh.polygonVertices.push_back(vertexIndices);
		}
	};
}