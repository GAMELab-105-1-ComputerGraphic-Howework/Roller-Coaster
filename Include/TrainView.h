#ifndef TRAINVIEW_H  
#define TRAINVIEW_H  
#include <QtOpenGL/QGLWidget>  
#include <QtGui/QtGui>  
#include <QtOpenGL/QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.H"
#include "Utilities/3DUtils.H"
#include "Utilities/Pnt3f.H"
#include "Track.H"
#include "GameLabModel.h"
#include "Square.h"
#include "Pool.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class AppMain;
class CTrack;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


struct MountainInfo
{
	float mountainWidthScale;
	float mountainLengthScale;
	float mountainHeightScale;
	float mountainXTraslate;
	float mountainZTraslate;
	float mountainYTraslate;
	float mountainRotateAngle;
	float birdFlyRadius;
	float birdFlyHeight;
};

struct CloudInfo {
	float cloudSpeed;
	float cloudPos;
	float cloudHorPos;
	float cloudHeight;
	float cloudScale;
};

class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{  
	Q_OBJECT  
public:  
	explicit TrainView(QWidget *parent = 0);  
	~TrainView();  

public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(bool doingShadows=false);

	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);

public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;  // simple - just remember which cube is selected

	CTrack*			m_pTrack;		// The track of the entire scene

	int camera;
	int curve;
	int track;
	bool isrun;

	// Eric
	bool doingFirst;

	GLuint textureIDList[10];

	int SCENE_SIZE;

	void onSomethingChange();
	void registerTexture(GLuint i);
	void drawSkyBox();
	void drawMyFloor(float size);
	void culTrainData();
	void initSetting();
	void initializeGL();
	void initializeTexture();

	Pnt3f fighterPos;
	Pnt3f fighterNextPos;
	Pnt3f fighterOrient;

	Pnt3f fighterUp;
	Pnt3f fighterRight;
	Pnt3f fighterFront;

	Pnt3f nightHawkPos;
	Pnt3f nightHawkNextPos;
	Pnt3f nightHawkOrient;

	Pnt3f nightHawkUp;
	Pnt3f nightHawkRight;
	Pnt3f nightHawkFront;

	Pnt3f train1Pos;
	Pnt3f train1NextPos;
	Pnt3f train1Orient;

	Pnt3f train1Up;
	Pnt3f train1Right;
	Pnt3f train1Front;

	Pnt3f train2Pos;
	Pnt3f train2NextPos;
	Pnt3f train2Orient;

	Pnt3f train2Up;
	Pnt3f train2Right;
	Pnt3f train2Front;

	float trainT;
	int trainControlPoint;
	int trainPoint;

	bool somethingChange;


	int birdAngle;

	int cloudPos;

	float speedAndDir;

	static const int mountainCount = 10;
	static const int cloudCount = 20;


	vector<MountainInfo> mountainInfos;
	CloudInfo cloudInfos[cloudCount];


	GameLabModel * Fighter;
	GameLabModel * Mountain;
	GameLabModel * Dove;
	GameLabModel * Eevee;
	GameLabModel * Train1;
	GameLabModel * Train2;
	GameLabModel * Cloud;
	GameLabModel * Heli;
	GameLabModel * Mig29;

	Square * square;
	Pool * pool;

	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];

	QVector<QOpenGLTexture*> Textures;
};  
#endif // TRAINVIEW_H  