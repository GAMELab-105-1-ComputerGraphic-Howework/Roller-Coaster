#include "GameLabMesh.h"

GameLabMesh::GameLabMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, bool justSolid, bool hasNormals)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->hasNormals = hasNormals;
	this->justSolid = justSolid;
}

void GameLabMesh::draw()
{

	glEnable(GL_TEXTURE_2D);
	bool hasTex = false;
	if (textures.size() > 0) {
		glBindTexture(GL_TEXTURE_2D, textures[0].id);
		hasTex = true;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	//glColor3f(1, 1, 1);
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (float *)&vertices[0]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (float *)&(vertices[0].TexCoords));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), (float *)&(vertices[0].Normal));

	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);



	//glColor3f(1, 1, 1);
	/*glBegin(GL_TRIANGLES);
	for (int j = 0; j < this->indices.size(); j += 3) {
		for (int k = 0; k < 3; k++) {
			Vertex v = this->vertices[this->indices[j + k]];
			
			if(justSolid != true)
				if(hasTex)
					glTexCoord2f(v.TexCoords.x, v.TexCoords.y);

			if(hasNormals == true)
				glNormal3f(v.Normal.x, v.Normal.y, v.Normal.z);
			glVertex3f(v.Position.x, v.Position.y, v.Position.z);
		}
	}
	glEnd();*/

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}
