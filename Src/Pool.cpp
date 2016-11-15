#include "Pool.h"
#include <iostream>

Pool::Pool()
{
	// initialize uniform
	POINTS_NUM = 1000;
	POOL_SIZE = 1200;
	waveCount = 20;
	amplitude = 2.0f;
	t = 0;

	float step = POOL_SIZE / POINTS_NUM;
	for (int i = 0; i <= POINTS_NUM; i++){
		for (int j = 0; j <= POINTS_NUM; j++){
			vertices << (QVector3D(i * step - POOL_SIZE / 2 - step / 2, 0.0f, j * step - POOL_SIZE / 2 + step / 2));
			vertices << (QVector3D(i * step - POOL_SIZE / 2 + step / 2, 0.0f, j * step - POOL_SIZE / 2 + step / 2));
			vertices << (QVector3D(i * step - POOL_SIZE / 2 - step / 2, 0.0f, j * step - POOL_SIZE / 2 - step / 2));

		 
			vertices << (QVector3D(i * step - POOL_SIZE / 2 + step / 2, 0.0f, j * step - POOL_SIZE / 2 + step / 2));
			vertices << (QVector3D(i * step - POOL_SIZE / 2 - step / 2, 0.0f, j * step - POOL_SIZE / 2 - step / 2));
			vertices << (QVector3D(i * step - POOL_SIZE / 2 + step / 2, 0.0f, j * step - POOL_SIZE / 2 - step / 2));
		}
	}


	InitShader("./Shaders/EricPoolVS.glsl", "./Shaders/EricPoolFS.glsl");

	vao.create();
	vvbo.create();

	shaderProgram->bind();
	{
		vao.bind();
		{
			vvbo.bind();
			{
				vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
				vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

				shaderProgram->enableAttributeArray(0);
				shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
			}
			vvbo.release();
		}
		vao.release();
	}
	shaderProgram->release();
	
}


Pool::~Pool()
{
}


void Pool::InitShader(QString vertexShaderPath, QString fragmentShaderPath){
	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	shaderProgram->link();
}

void Pool::Paint(glm::mat4 projection, glm::mat4 modelView, glm::vec3 lightPos, glm::vec3 viewPos)
{
	t += 0.003f;
	if (t >= 1.0f)
	{
		t = 0;
	}

	GLfloat MV[4][4];
	GLfloat P[4][4];
	GLfloat NM[4][4];

	bool different = false;


	NormalMat = glm::transpose(glm::inverse(modelView));
	
	DimensionTransformation(glm::value_ptr(modelView), MV);
	DimensionTransformation(glm::value_ptr(projection), P);
	DimensionTransformation(glm::value_ptr(NormalMat), NM);

	shaderProgram->bind();
	{
		vao.bind();
		{
			shaderProgram->setUniformValue("modelView", MV);
			shaderProgram->setUniformValue("projection", P);
			shaderProgram->setUniformValue("normalMat", NM);

			shaderProgram->setUniformValue("lightPos", lightPos.x, lightPos.y, lightPos.z);
			shaderProgram->setUniformValue("lightColor", 1.0f, 1.0f, 1.0f);
			shaderProgram->setUniformValue("waveCount", waveCount);
			shaderProgram->setUniformValue("poolSize", POOL_SIZE);
			shaderProgram->setUniformValue("viewPos", viewPos.x, viewPos.y, viewPos.z);
			shaderProgram->setUniformValue("amplitude", amplitude);
			shaderProgram->setUniformValue("t", t);

			glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		}
		vao.release();
	}
	shaderProgram->release();
}

void Pool::DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j < 4; j++)
	for (int k = 0; k < 4; k++)
	{
		target[j][k] = source[i];
		i++;
	}
}