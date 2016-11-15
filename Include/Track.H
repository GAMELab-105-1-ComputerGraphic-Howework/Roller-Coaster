/************************************************************************
     File:        Track.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Container for the "World"

						This provides a container for all of the "stuff" 
						in the world.

						It could have been all global variables, or it could 
						have just been
						contained in the window. The advantage of doing it 
						this way is that
						we might have multiple windows looking at the same 
						world. But, I don't	think we'll actually do that.

						See the readme for commentary on code style

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/
#pragma once

#include <vector>
#include <iostream>
#include "TrackPoint.h"

#include <QtOpenGL/QGLWidget> 
#include <QtGui/QtGui>  
#include <QtOpenGL/QtOpenGL>  
#include <GL/GLU.h>
#include <QtGui/QOpenGLFunctions_4_3_Core>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.H"
#include "Utilities/3DUtils.H"

using std::vector; // avoid having to say std::vector all of the time

// make use of other data structures from this project
#include "ControlPoint.H"

class CTrack {
	public:		
		// Constructor
		CTrack();

	public:
		// when we want to clear the control points, we really "reset" them 
		// to have 4 default positions (since we should never have fewer
		// than 4 points)
		void resetPoints();


		// read and write to files
		void readPoints(const char* filename);
		void writePoints(const char* filename);

		void getTrainNextPos(float speedAndDir);
		int getTrainNextTmpI(float speed, int tmp_i);

		TrackPoint * getTrain1Pos();
		TrackPoint * getTrain2Pos();
		TrackPoint * getNightHawkPos();
		TrackPoint * getFighterPos();

		void computeTrackPoints(int curve);

		Pnt3f getQt(float t, Pnt3f gm[4]);
		Pnt3f getVt(float t, Pnt3f gm[4]);

		void drawTrack(bool doingShadows);
		void drawWood(Pnt3f pos, Pnt3f front, Pnt3f up, bool doingShadows);
		void drawQuad(Pnt3f rt, Pnt3f rb, Pnt3f lb, Pnt3f lt, Pnt3f normal, int r, int g, int b, bool doingShadows);
		void drawCube(Pnt3f pos, Pnt3f front, Pnt3f up, Pnt3f right, int r, int g, int b, bool doingShadows);
		void drawPillar(Pnt3f pos, bool doingShadows);
		void getGM(Pnt3f gPos[4], Pnt3f gOrient[4], float mat[4][4]);

		int train1ControlPoint;
		int train1Point;

		int train2ControlPoint;
		int train2Point;

		int nightHawkControlPoint;
		int nightHawkPoint;

		int fighterControlPoint;
		int fighterPoint;

		float m_MaxHeight;

		float m_MinHeight;

		Pnt3f gmPos[4];
		Pnt3f gmOrient[4];

	public:
		// rather than have generic objects, we make a special case for these few
		// objects that we know that all implementations are going to need and that
		// we're going to have to handle specially
		vector<ControlPoint> points;

		//###################################################################
		// TODO: you might want to do this differently
		//###################################################################
		// the state of the train - basically, all I need to remember is where
		// it is in parameter space
		float trainU;


private:
	vector<vector<TrackPoint *>> track_pts;
	int TIME_DIVISION;
};


