#include "meshes/loader.h"

#include <fstream>
#include <sstream>

///
/// TODO: Reformat this file for readability and logic
/// Separate the relevent parts into their own file since the process is incredibly long
/// And confusing.
///

namespace splr
{
	/**
	 * Load the data inside of an obj file into vectors of vertices.
	 * @param path - location of the file
	 * @param finalVerts - Storage for the vertex position
	 * @param finalUVs - Storage for the vertex 2D coordinates
	 * @param finalNormals - Storage for the vertex normals
	 * @return if the reading of the file was succesfull
	 */
	bool loadObj(std::string path, MeshData &mesh)
	{
		std::vector<unsigned int> vertIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> tempVerts;
		std::vector<glm::vec2> tempUV;
		std::vector<glm::vec3> tempNormals;

		uint8_t metAllTypes = 0;

		std::ifstream file(path);
		if (!file.is_open())
		{
			std::cerr << "ERROR: The given OBJ file cannot be found." << std::endl;
			return false;
		}

		std::string token;

		while (file >> token)
		{
			// Vertex position
			if (token == "v")
			{
				metAllTypes |= 1;
				glm::vec3 vertex;
				file >> vertex.x;
				file >> vertex.y;
				file >> vertex.z;
				tempVerts.push_back(vertex);
			}
			// Vertex texture coordinates
			else if (token == "vt")
			{
				metAllTypes |= 2;
				glm::vec2 uv;
				file >> uv.x;
				file >> uv.y;
				tempUV.push_back(uv);
			}
			// Vertex normal coordinates
			else if (token == "vn")
			{
				metAllTypes |= 4;
				glm::vec3 normal;
				file >> normal.x;
				file >> normal.y;
				file >> normal.z;
				tempNormals.push_back(normal);
			}
			// indices
			else if (token == "f")
			{
				if (metAllTypes != 0b111)
				{
					std::cerr << "ERROR: Check if the UVs and the Normals "
							  << "of the mesh are defined in the OBJ file." << std::endl;
					return false;
				}
				std::string verts;
				unsigned int vertexIndex, uvIndex, normalIndex;
				std::stringstream ss;

				for (int i = 0; i < 3; i++)
				{
					file >> verts;
					ss = std::stringstream(verts);

					std::getline(ss, token, '/');
					vertexIndex = std::stoi(token);

					std::getline(ss, token, '/');
					uvIndex = std::stoi(token);

					std::getline(ss, token, '/');
					normalIndex = std::stoi(token);

					vertIndices.push_back(vertexIndex);
					uvIndices.push_back(uvIndex);
					normalIndices.push_back(normalIndex);
				}
			}
		}

		for (int i = 0; i < vertIndices.size(); i++)
		{
			mesh.append(Vertex(
				tempVerts[vertIndices[i] - 1],
				tempUV[uvIndices[i] - 1],
				tempNormals[normalIndices[i] - 1]));
		}

		file.close();

		return true;
	}
}