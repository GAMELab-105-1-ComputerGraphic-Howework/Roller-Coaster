#include "TrainView.h"  

#include <Windows.h>
#include <MMSystem.h>


using namespace std;

TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();
	somethingChange = true;
	doingFirst = true;
	SCENE_SIZE = 1200;
	speedAndDir = 0;
	birdAngle = 0;
}  

TrainView::~TrainView()  
{}  

void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);

	// BGM
	PlaySound(TEXT(".\\Sounds\\Sound1.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
}

void TrainView::paintGL()
{
	/*if(doingFirst == true){
		initSetting();
	}*/

	if(somethingChange == true){
		m_pTrack->computeTrackPoints(curve);
		somethingChange = false;
	}
	else
		culTrainData();


	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	// Set up the view port
	glViewport(0,0,width(),height());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]		= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	setupFloor();
	glDisable(GL_LIGHTING);
	//drawFloor(200,10);

	// drawMyFloor(500);
	drawSkyBox();


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
	setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == 0){
		arcball.setProjection(false);
		update();
	// Or we use the top cam
	}else if (this->camera == 1) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
		glMatrixMode(GL_PROJECTION); //車頭的視角
		gluPerspective(45, aspect, 1, 10000);
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();

		Pnt3f trainPoint = train1Pos + train1Up * 20 - train1Front * 40;
		Pnt3f target = trainPoint + train1Front;

		gluLookAt(trainPoint.x, trainPoint.y, trainPoint.z, target.x, target.y, target.z, train1Up.x, train1Up.y, train1Up.z);
		update();
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::
	drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != 2) {
		for(size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################

	m_pTrack->drawTrack(doingShadows);

	// fighter
	{
		if (!doingShadows)
			glColor3ub(0xff, 0xff, 0xff);
		glPushMatrix();
		{
			glTranslatef(20.0f, 75.0f, 5.0f);
			glRotatef(50.0f, 1.0f, 1.0f, 0.0f);
			glTranslatef(fighterPos.x, fighterPos.y, fighterPos.z);
			Fighter->Render(glm::vec3(fighterPos.x, fighterPos.y, fighterPos.z), glm::vec3(fighterUp.x, fighterUp.y, fighterUp.z), glm::vec3(fighterFront.x, fighterFront.y, fighterFront.z));

		}
		glPopMatrix();
	}

	// bird & mountain
	{
		birdAngle++;	
		if (birdAngle >= 360)
			birdAngle = 0;

		float PI = 3.1415926f;
		float angle = birdAngle * PI / 180.0f;

		Pnt3f dis = Pnt3f(cos(angle), 0.0f, sin(angle));
		dis.normalize();
		Pnt3f normal = dis * Pnt3f(0.0f, 1.0f, 0.0f);

		for (int i = 0; i < mountainInfos.size(); i++) {
			MountainInfo mInfo = mountainInfos[i];
			if (!doingShadows)
				glColor3ub(0xff, 0xff, 0xff);
			glPushMatrix();
			{
				glTranslatef(mInfo.mountainXTraslate, 0.0f, mInfo.mountainZTraslate);
				glRotatef(mInfo.mountainRotateAngle, 0.0f, 1.0f, 0.0f);
				glScalef(mInfo.mountainWidthScale, mInfo.mountainHeightScale, mInfo.mountainLengthScale);
				Mountain->Render(glm::vec3(0, 0, 0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0));
			}
			glPopMatrix();

			if (!doingShadows)
				glColor3ub(0xff, 0xff, 0xff);
			glPushMatrix();
			{
				Pnt3f radius = dis * (mInfo.birdFlyRadius);
				glTranslatef(mInfo.mountainXTraslate, mInfo.birdFlyHeight, mInfo.mountainZTraslate);
				glRotatef(mInfo.mountainRotateAngle, 0.0f, 1.0f, 0.0f);
				Dove->Render(glm::vec3(radius.x, radius.y, radius.z), glm::vec3(0.0, 1.0, 0.0), glm::vec3(normal.x, normal.y, normal.z));
			}
			glPopMatrix();
		}
	}

	// clouds
	{
		for (int i = 0; i < cloudCount; i++) {
			glPushMatrix();
			{
				if (!doingShadows)
					glColor3ub(0xff, 0xff, 0xff);
				cloudInfos[i].cloudPos += cloudInfos[i].cloudSpeed;
				if (cloudInfos[i].cloudPos > 1000)
					cloudInfos[i].cloudPos -= 2000;
				else if (cloudInfos[i].cloudPos < -1000)
					cloudInfos[i].cloudPos += 2000;

				glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
				glTranslatef(0, 100, 0);
				glTranslatef(cloudInfos[i].cloudHorPos, cloudInfos[i].cloudHeight, cloudInfos[i].cloudPos);
				glScalef(cloudInfos[i].cloudScale, cloudInfos[i].cloudScale, cloudInfos[i].cloudScale);
				Cloud->Render(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
			}
			glPopMatrix();
		}
	}

	Pnt3f trainpp;
	// train1 & train2 & heli
	{
		if (!doingShadows)
			glColor3ub(0xff, 0, 0);
		trainpp = train1Pos + train1Up * 2;
		Train1->Render(glm::vec3(trainpp.x, trainpp.y, trainpp.z), glm::vec3(train1Up.x, train1Up.y, train1Up.z), glm::vec3(train1Front.x, train1Front.y, train1Front.z));

		trainpp = train2Pos + train2Up * 2;
		Train1->Render(glm::vec3(trainpp.x, trainpp.y, trainpp.z), glm::vec3(train2Up.x, train2Up.y, train2Up.z), glm::vec3(train2Front.x, train2Front.y, train2Front.z));

		if (!doingShadows)
			glColor3ub(0xff, 0xff, 0xff);

		trainpp = trainpp + train2Up * 5;
		Eevee->Render(glm::vec3(trainpp.x, trainpp.y, trainpp.z), glm::vec3(train2Up.x, train2Up.y, train2Up.z), glm::vec3(train2Front.x, train2Front.y, train2Front.z));

		glPushMatrix();
		{
			Pnt3f tmpFront = train2Front;
			tmpFront.y = 0;
			tmpFront.normalize();
			Pnt3f tmpUp = Pnt3f(0.0f, 1.0f, 0.0f);
			Pnt3f tmpRight = tmpUp * tmpFront;
			if (!doingShadows)
				glColor3ub(0xff, 0xff, 0xff);
			trainpp = trainpp - 20 * tmpFront + 20 * tmpUp + (-10) * tmpRight;
			Heli->Render(glm::vec3(trainpp.x, trainpp.y, trainpp.z), glm::vec3(0, 1, 0), glm::vec3(tmpFront.x, tmpFront.y, tmpFront.z));
		}
		glPopMatrix();
	}

	// nighthawk
	{
		if (!doingShadows)
			glColor3ub(0xff, 0xff, 0xff);
		glPushMatrix();
		{
			glTranslatef(0.0f, 50.0f, 0.0f);
			glRotatef(120.0f, 0.0f, 1.0f, 0.0f);
			Mig29->Render(glm::vec3(nightHawkPos.x, nightHawkPos.y, nightHawkPos.z), glm::vec3(nightHawkUp.x, nightHawkUp.y, nightHawkUp.z), glm::vec3(nightHawkFront.x, nightHawkFront.y, nightHawkFront.z));
		}
		glPopMatrix();
	}

	//////
	glm::mat4 modelView;
	glm::mat4 projection;

	glPushMatrix();
	{
		glTranslatef(0.0f, 5.0f, 0.0f);
		//Get modelview matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, value_ptr(modelView));
		//Get projection matrix
		glGetFloatv(GL_PROJECTION_MATRIX, value_ptr(projection));
	}
	glPopMatrix();



	glm::vec4 eye(arcball.eyeX, arcball.eyeY, arcball.eyeZ, 1.0);
	float MM[4][4];
	glm::mat4 mm;
	arcball.getMatrix(MM);
	for(int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++)
		{
			mm[i][j] = MM[i][j];
		}
	}
	eye = eye * mm;

	std::cout << eye.x << eye.y << eye.z << std::endl;
	Pnt3f trainPoint = train1Pos + train1Up * 20 - train1Front * 40;

	pool->Paint(projection, modelView, glm::vec3(600, 300, -600), glm::vec3(0.0f, 0.0f, 0.0f));
	

	
}


void TrainView::
	doPick(int mx, int my)
	//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}

void TrainView::onSomethingChange(){
	somethingChange = true;
}

QString filePathList[10] = {
	"./Textures/skybox.png",
	"./Textures/grass.png"
};

void TrainView::
	registerTexture(GLuint i) {

	QImage img;
	if (!img.load(filePathList[i])) {
		std::cout << i << "error" << std::endl;
		qWarning("Texture Loading Error!");
	}
	QImage GL_formatted_image = QGLWidget::convertToGLFormat(img);
	glGenTextures(1, &textureIDList[i]);
	glBindTexture(GL_TEXTURE_2D, textureIDList[i]);
	//generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GL_formatted_image.width(),
		GL_formatted_image.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits());
	//texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void TrainView::
	drawSkyBox(){
	// parameters:

	int BOUND = SCENE_SIZE / 2;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureIDList[0]);

	//左
	glBegin(GL_QUADS);
	glColor3ub(0xff, 0xff, 0xff);
	glTexCoord2f(0, 0.3333);
	glVertex3d(-BOUND, -BOUND, -BOUND);
	glTexCoord2f(0.25, 0.3333);
	glVertex3d(-BOUND, -BOUND, BOUND);
	glTexCoord2f(0.25, 0.6666);
	glVertex3d(-BOUND, BOUND, BOUND);
	glTexCoord2f(0, 0.6666);
	glVertex3d(-BOUND, BOUND, -BOUND);
	glEnd();

	//前
	glBegin(GL_QUADS);
	glColor3ub(0xff, 0xff, 0xff);
	glTexCoord2f(0.25, 0.6666);
	glVertex3d(-BOUND, BOUND, BOUND);
	glTexCoord2f(0.25, 0.3333);
	glVertex3d(-BOUND, -BOUND, BOUND);
	glTexCoord2f(0.5, 0.3333);
	glVertex3d(BOUND, -BOUND, BOUND);
	glTexCoord2f(0.5, 0.6666);
	glVertex3d(BOUND, BOUND, BOUND);
	glEnd();

	//右
	glBegin(GL_QUADS);
	glColor3ub(0xff, 0xff, 0xff);
	glTexCoord2f(0.75, 0.3333);
	glVertex3d(BOUND, -BOUND, -BOUND);
	glTexCoord2f(0.5, 0.3333);
	glVertex3d(BOUND, -BOUND, BOUND);
	glTexCoord2f(0.5, 0.6666);
	glVertex3d(BOUND, BOUND, BOUND);
	glTexCoord2f(0.75, 0.6666);
	glVertex3d(BOUND, BOUND, -BOUND);
	glEnd();

	//後
	glBegin(GL_QUADS);
	glColor3ub(0xff, 0xff, 0xff);
	glTexCoord2f(1, 0.6666);
	glVertex3d(-BOUND, BOUND, -BOUND);
	glTexCoord2f(1, 0.3333);
	glVertex3d(-BOUND, -BOUND, -BOUND);
	glTexCoord2f(0.75, 0.3333);
	glVertex3d(BOUND, -BOUND, -BOUND);
	glTexCoord2f(0.75, 0.6666);
	glVertex3d(BOUND, BOUND, -BOUND);
	glEnd();

	//上
	glBegin(GL_QUADS);
	glColor3ub(0xff, 0xff, 0xff);
	glTexCoord2f(0.252, 0.666);
	glVertex3d(-BOUND, BOUND, BOUND);
	glTexCoord2f(0.498, 0.666);
	glVertex3d(BOUND, BOUND, BOUND);
	glTexCoord2f(0.498, 0.9999);
	glVertex3d(BOUND, BOUND, -BOUND);
	glTexCoord2f(0.252, 0.9999);
	glVertex3d(-BOUND, BOUND, -BOUND);
	glEnd();


	// 下
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.25, 0);
	glVertex3d(-BOUND, -BOUND, -BOUND);
	glTexCoord2f(0.5, 0);
	glVertex3d(BOUND, -BOUND, -BOUND);
	glTexCoord2f(0.5, 0.3333);
	glVertex3d(BOUND, -BOUND, BOUND);
	glTexCoord2f(0.25, 0.3333);
	glVertex3d(-BOUND, -BOUND, BOUND);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void TrainView::
	drawMyFloor(float size) {
		// parameters:
		float maxX = size / 2, maxY = size / 2;
		float minX = -size / 2, minY = -size / 2;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureIDList[1]);


		glBegin(GL_QUADS);
		glColor4fv(floorColor1);
		glNormal3f(0, 1, 0);
		glTexCoord2f(0, 0);
		glVertex3d(minX, 0, minY);
		glTexCoord2f(0, 1);
		glVertex3d(minX, 0, 0);
		glTexCoord2f(1, 1);
		glVertex3d(0, 0, 0);
		glTexCoord2f(1, 0);
		glVertex3d(0, 0, minY);

		glNormal3f(0, 1, 0);
		glTexCoord2f(0, 0);
		glVertex3d(minX, 0, 0);
		glTexCoord2f(0, 1);
		glVertex3d(minX, 0, maxY);
		glTexCoord2f(1, 1);
		glVertex3d(0, 0, maxY);
		glTexCoord2f(1, 0);
		glVertex3d(0, 0, 0);

		glNormal3f(0, 1, 0);
		glTexCoord2f(0, 0);
		glVertex3d(0, 0, minY);
		glTexCoord2f(0, 1);
		glVertex3d(0, 0, 0);
		glTexCoord2f(1, 1);
		glVertex3d(maxX, 0, 0);
		glTexCoord2f(1, 0);
		glVertex3d(maxX, 0, minY);


		glNormal3f(0, 1, 0);
		glTexCoord2f(0, 0);
		glVertex3d(0, 0, 0);
		glTexCoord2f(0, 1);
		glVertex3d(0, 0, maxY);
		glTexCoord2f(1, 1);
		glVertex3d(maxX, 0, maxY);
		glTexCoord2f(1, 0);
		glVertex3d(maxX, 0, 0);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

}

void TrainView::
	culTrainData() {
		//計算火車現在位置
		TrackPoint * pt = m_pTrack->getFighterPos();

		fighterPos = pt->pos;
		fighterRight = pt->right;
		fighterUp = pt->up;
		fighterFront = pt->front;

		pt = m_pTrack->getNightHawkPos();

		nightHawkPos = pt->pos;
		nightHawkRight = pt->right;
		nightHawkUp = pt->up;
		nightHawkFront = pt->front;

		pt = m_pTrack->getTrain1Pos();

		train1Pos = pt->pos;
		train1Right = pt->right;
		train1Up = pt->up;
		train1Front = pt->front;

		pt = m_pTrack->getTrain2Pos();

		train2Pos = pt->pos;
		train2Right = pt->right;
		train2Up = pt->up;
		train2Front = pt->front;

		if (isrun == true)
			m_pTrack->getTrainNextPos(speedAndDir);
}

void TrainView::
	initSetting(){
	Fighter = new GameLabModel("./Models/XA-20_Razorback_Strike_Fighter/XA-20_Razorback_Strike_Fighter.obj", glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(2, 2, 2));
	Mountain = new GameLabModel("./Models/wS free terrain 005/WS free terrain 005.obj", glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(10, 10, 10));
	Dove = new GameLabModel("./Models/DOve/DOVE.OBJ", glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(40, 40, 40), glm::vec3(), true);
	Eevee = new GameLabModel("./Models/Eevee/Eevee.obj", glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
	Train1 = new GameLabModel("./Models/Train/Train21.obj", glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(0.2, 0.2, 0.2));
	Cloud = new GameLabModel("./Models/Cloud/Cloud.obj", glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(10, 10, 10), glm::vec3(80, 100, -300), true);
	Heli = new GameLabModel("./Models/UH-60 Blackhawk/uh60.obj", glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 1, 1));
	Mig29 = new GameLabModel("./Models/Mig-29_Fulcrum/Mig-29_Fulcrum.obj", glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(3, 3, 3));

	for(int i = 0; i < 2; i ++){
		registerTexture(i);
	}
	doingFirst = false;

	for (int i = 0; i < mountainCount; i++) {
		MountainInfo tmp;
		mountainInfos.push_back(tmp);
		mountainInfos[i].mountainHeightScale = rand() % 100 / 100.0f * 3 + 1;
		mountainInfos[i].mountainWidthScale = rand() % 100 / 100.0f + 1;
		mountainInfos[i].mountainLengthScale = rand() % 100 / 100.0f + 1;
		mountainInfos[i].mountainRotateAngle = rand() % 360;
		mountainInfos[i].mountainXTraslate = rand() % 400 - 200;
		mountainInfos[i].mountainZTraslate = rand() % 400 - 200;
		mountainInfos[i].mountainYTraslate = rand() % 5 + 2.5;
		mountainInfos[i].birdFlyRadius = rand() % 50 + 20;
		mountainInfos[i].birdFlyHeight = rand() % 50 + 20;
	}

	// random PCG
	for (int i = 0; i < cloudCount; i++) {
		cloudInfos[i].cloudHeight = rand() % 200 + 100;
		cloudInfos[i].cloudPos = rand() % 2000 - 1000;
		cloudInfos[i].cloudScale = rand() % 100 / 100.0f * 0.7f + 0.3f;
		cloudInfos[i].cloudHorPos = rand() % 800 - 400;
		cloudInfos[i].cloudSpeed = rand() % 2 + 1;
	}

	for (int i = 0; i < this->m_pTrack->points.size(); i++) {
		for (int j = 0; j < mountainCount; j++) {

			Pnt3f dis = this->m_pTrack->points[i].pos - Pnt3f(mountainInfos[j].mountainXTraslate, 0.0f, mountainInfos[j].mountainZTraslate);

			if (dis.length() < 50.0f) {
				dis.normalize();
				this->m_pTrack->points[i].pos = this->m_pTrack->points[i].pos + 50 * dis;
			}
		}
	}

	for(int i = 0; i < this->m_pTrack->points.size(); i++)
		this->m_pTrack->points[i].pos.y += 30.0f;

	square = new Square();
	//Initialize the square object
	square->Init();

	pool = new Pool();

	initializeTexture();
}

void TrainView::initializeGL(){
	std::cout << "init " << std::endl;
	initializeOpenGLFunctions();
	initSetting();

}

void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("./Textures/skybox.png"));
	Textures.push_back(texture);
}