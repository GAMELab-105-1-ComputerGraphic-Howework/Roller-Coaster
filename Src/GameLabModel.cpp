#include "GameLabModel.h"

GameLabModel::GameLabModel(GLchar* path, glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 scale, glm::vec3 offset, bool justSolid)
{
	this->justSolid = justSolid;
	initTransform(pos, up, front, scale, offset);
	this->loadModel(path);
}

void GameLabModel::loadModel(string path)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->processNode(scene->mRootNode, scene);
}

void GameLabModel::processNode(aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}

}

GameLabMesh GameLabModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normals
		if (mesh->HasNormals() == true) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = 1 - mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (justSolid == false) {
		// Process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. Specular maps
			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
	}

	// Return a mesh object created from the extracted mesh data
	return GameLabMesh(vertices, indices, textures, this->justSolid, mesh->HasNormals());
}

vector<Texture> GameLabModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{

	vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.id = loadTextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void GameLabModel::Render(glm::vec3 pos, glm::vec3 up, glm::vec3 front)
{
	static const float M_PI = 3.1415926f;
	glPushMatrix();
	{

		glTranslatef(pos.x, pos.y, pos.z);


		float dot_r, rad;
		float angle1, angle2;

		glm::vec3 axis1, axis2;

		glm::vec3 tmp;
		glm::vec3 w_up(0, 1, 0);
		glm::vec3 w_front(0, 0, 1);

		dot_r = glm::dot(w_up, up) / glm::length(up) / glm::length(w_up);
		if (dot_r != 1.0f) {
			rad = acos(dot_r);

			axis1 = glm::cross(w_up, up);

			if (glm::length(axis1) == 0.0f)
				axis1 = w_front;
			else
				axis1 = glm::normalize(axis1);

			angle1 = 180.0 / M_PI * rad;

			glRotatef(angle1, axis1.x, axis1.y, axis1.z);

			// change prepare
			front = rotateVector(-rad, front, axis1);
		}

		dot_r = glm::dot(w_front, front) / glm::length(front) / glm::length(w_front);
		if (dot_r != 1.0f) {
			rad = acos(dot_r);
			axis2 = glm::cross(w_front, front);

			if (glm::length(axis2) == 0.0f)
				axis2 = w_up;
			else
				axis2 = glm::normalize(axis2);

			angle2 = 180.0f / M_PI * rad;

			glRotatef(angle2, axis2.x, axis2.y, axis2.z);
		}
		
		glMultMatrixf(glm::value_ptr(transform));
		for (int i = 0; i < this->meshes.size(); i++) {
			this->meshes[i].draw();
		}

	}
	glPopMatrix();
}

glm::vec3 GameLabModel::rotateVector(float rad, glm::vec3 vector, glm::vec3 axis) {
	glm::mat4 transform;
	transform = glm::rotate(transform, rad, axis);

	glm::vec4 tmp(vector, 0);
	tmp = transform * tmp;

	return glm::vec3(tmp.x, tmp.y, tmp.z);
}

void GameLabModel::initTransform(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 scale, glm::vec3 offset) {
	static const float M_PI = 3.1415926f;

	transform = glm::translate(transform, pos);

	float dot_r, rad;
	float angle1, angle2;

	glm::vec3 axis1, axis2;

	glm::vec3 tmp;
	glm::vec3 w_up(0, 1, 0);
	glm::vec3 w_front(0, 0, 1);

	dot_r = glm::dot(w_up, up);
	if (dot_r < 0.99f) {

		rad = acos(dot_r / glm::length(up) / glm::length(w_up));
		axis1 = glm::cross(w_up, up);

		cout << "dot_r1 " << dot_r << endl;
		cout << "Axis1: (" << axis1.x << ", " << axis1.y << ", " << axis1.z << ") glm::length1  " << glm::length(axis1) << endl;

		if (glm::length(axis1) < 0.01f) {
			axis1 = w_front;
			cout << "1 len if active" << endl;
		}
		else
			axis1 = glm::normalize(axis1);
		cout << "Axis1: (" << axis1.x << ", " << axis1.y << ", " << axis1.z << ") glm::length1  " << glm::length(axis1) << endl;
		cout << "angle1:  " << rad << endl;
		transform = glm::rotate(transform, rad, axis1);
		
		// prepare
		front = rotateVector(-rad, front, axis1);
	}

	dot_r = dot(w_front, front);
	if (dot_r < 0.99f) {
		rad = acos(dot_r / glm::length(front) / glm::length(w_front));
		axis2 = glm::cross(w_front, front);

		cout << "dot_r2 " << dot_r << endl;
		cout << "Axis2: (" << axis2.x << ", " << axis2.y << ", " << axis2.z << ") glm::length2:  " << glm::length(axis2) << endl;

		if (glm::length(axis2) < 0.01f) {
			cout << "2 len if active" << endl;
			axis2 = w_up;
		}else
			axis2 = glm::normalize(axis2);
		cout << "Axis2: (" << axis2.x << ", " << axis2.y << ", " << axis2.z << ") glm::length2:  " << glm::length(axis2) << endl;
		cout << "angle2:  " << rad << endl;
		transform = glm::rotate(transform, rad, axis2);
	}
	transform = glm::scale(transform, scale);
	transform = glm::translate(transform, offset);
}