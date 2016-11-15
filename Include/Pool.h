#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtOpenGL/QtOpenGL> 
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QFileInfo>
#include <QOpenGLTexture>
#include <QImage>
#include <QDebug>
#include <QString>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Pool
{
private:
	// Shader
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;

	QVector<QVector3D> vertices;

	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;

	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);


	float t;
	float amplitude;
	int POINTS_NUM;
	float POOL_SIZE;
	int waveCount;
	QVector3D center;
	glm::mat4 NormalMat;
	glm::mat4 oldModelView;
	
public:
	Pool();
	~Pool();


	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void Paint(glm::mat4 projection, glm::mat4 modelView, glm::vec3 lightPos, glm::vec3 viewPos);

};

