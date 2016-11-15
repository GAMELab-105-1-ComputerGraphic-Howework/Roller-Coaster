#pragma once
// Std. Includes
// GL Includes

#include "GameLabMesh.h"

using namespace std;

class GameLabModel
{
public:
	bool justSolid;
	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	GameLabModel(GLchar* path, glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 scale, glm::vec3 offset = glm::vec3(), bool justSolid = false);

	void Render(glm::vec3 pos, glm::vec3 up, glm::vec3 front);

private:
	/*  Model Data  */
	vector<GameLabMesh> meshes;
	string directory;
	vector<Texture> textures_loaded;
	glm::mat4 transform;
	
	void loadModel(string path);

	void processNode(aiNode* node, const aiScene* scene);

	GameLabMesh processMesh(aiMesh* mesh, const aiScene* scene);

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

	GLint loadTextureFromFile(const char* path, string directory)
	{
		string filename = string(path);
		filename = directory + '/' + filename;
		QImage img;
		if (!img.load(filename.c_str())) {
			qWarning("Texture Loading Error!");
		}
		QImage GL_formatted_image = QGLWidget::convertToGLFormat(img);

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		//generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GL_formatted_image.width(),
			GL_formatted_image.height(),
			0, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits());
		//texture parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		return textureID;
	}

	glm::vec3 rotateVector(float rad, glm::vec3 vector, glm::vec3 axis);

	void initTransform(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 scale, glm::vec3 offset = glm::vec3());
};

