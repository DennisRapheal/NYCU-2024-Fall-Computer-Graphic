#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>

Model* Model::fromObjectFile(const char* obj_file) {
  Model* m = new Model();

  std::ifstream ObjFile(obj_file);

  if (!ObjFile.is_open()) {
    std::cout << "Can't open File !" << std::endl;
    return NULL;
  }

  /* TODO#1: Load model data from OBJ file
   *         You only need to handle v, vt, vn, f
   *         Other fields you can directly ignore
   *         Fill data into m->positions, m->texcoords m->normals and m->numVertex
   *         Data format:
   *           For positions and normals
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | ...
   *         | face 1                                                       | face 2               ...
   *         | v1x  | v1y  | v1z  | v2x  | v2y  | v2z  | v3x  | v3y  | v3z  | v1x  | v1y  | v1z  | ...
   *         | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | ...
   *           For texcoords
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | ...
   *         | face 1                                  | face 2        ...
   *         | v1x  | v1y  | v2x  | v2y  | v3x  | v3y  | v1x  | v1y  | ...
   * Note:
   *        OBJ File Format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
   *        Vertex per face = 3 or 4
   */
	std::vector<glm::vec3> tempPositions; // Store all positions from v lines
	std::vector<glm::vec2> tempTexCoords;   // Store all normals from vn lines
	std::vector<glm::vec3> tempNormals; // Store all texture coordinates from vt lines

	std::string line;
	while (std::getline(ObjFile, line)) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;

    if (prefix == "v") {
      glm::vec3 position;
      iss >> position.x >> position.y >> position.z;
      tempPositions.push_back(position);
    } else if (prefix == "vt") {
      glm::vec2 texCoord;
      iss >> texCoord.x >> texCoord.y;
      tempTexCoords.push_back(texCoord);
    } else if (prefix == "vn") {
      glm::vec3 normal;
      iss >> normal.x >> normal.y >> normal.z;
      tempNormals.push_back(normal);
    } else if (prefix == "f") {
      std::string vertexData;
      while (iss >> vertexData) { // Process face data
        std::istringstream viss(vertexData);
        std::string posIndex, texIndex, normIndex;

        if (std::getline(viss, posIndex, '/') &&
            std::getline(viss, texIndex, '/') &&
            std::getline(viss, normIndex, '/')) {
          // Convert indices from 1-based to 0-based
          int posIdx = std::stoi(posIndex) - 1;
          int texIdx = std::stoi(texIndex) - 1;
          int normIdx = std::stoi(normIndex) - 1;

          // Add positions, texture coordinates, and normals to the model
          m->positions.push_back(tempPositions[posIdx].x);
          m->positions.push_back(tempPositions[posIdx].y);
          m->positions.push_back(tempPositions[posIdx].z);

          m->texcoords.push_back(tempTexCoords[texIdx].x);
          m->texcoords.push_back(tempTexCoords[texIdx].y);

          m->normals.push_back(tempNormals[normIdx].x);
          m->normals.push_back(tempNormals[normIdx].y);
          m->normals.push_back(tempNormals[normIdx].z);
        }
      }
    }
  }

  // Set the number of vertices in the model
  m->numVertex = static_cast<int>(m->positions.size());

  ObjFile.close();

  return m;
}
