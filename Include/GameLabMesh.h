#pragma once


#include <QtOpenGL\qgl.h>
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;
// GL Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};

class GameLabMesh {
private:

public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	bool justSolid;
	bool hasNormals;


	/*  Functions  */
	// Constructor
	GameLabMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, bool justSolid, bool hasNormals);

	// Render the mesh
	void draw();
};