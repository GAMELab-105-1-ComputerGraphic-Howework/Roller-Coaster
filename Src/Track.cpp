/************************************************************************
     File:        Track.cpp

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

#include "Track.H"

float M_Cardinal[4][4] = {
	{ -1 / 2.0, 2 / 2.0, -1 / 2.0, 0 / 2.0 },
	{ 3 / 2.0, -5 / 2.0, 0 / 2.0, 2 / 2.0 },
	{ -3 / 2.0, 4 / 2.0, 1 / 2.0, 0 / 2.0 },
	{ 1 / 2.0, -1 / 2.0, 0 / 2.0, 0 / 2.0 }
};

float M_B_Spline[4][4] = {
	{ -1 / 6.0, 3 / 6.0, -3 / 6.0, 1 / 6.0 },
	{ 3 / 6.0, -6 / 6.0, 0 / 6.0, 4 / 6.0 },
	{ -3 / 6.0, 3 / 6.0, 3 / 6.0, 1 / 6.0 },
	{ 1 / 6.0, 0, 0, 0 }
};

//****************************************************************************
//
// * Constructor
//============================================================================
CTrack::
CTrack() : trainU(0)
//============================================================================
{
	resetPoints();

	train1ControlPoint = 3;
	train1Point = 0;
	
	train2ControlPoint = 0;
	train2Point = 0;
	
	nightHawkControlPoint = 1;
	nightHawkPoint = 0;
	
	fighterControlPoint = 2;
	fighterPoint = 0;

	
	m_MaxHeight = 220;
	
	m_MinHeight = 0.0f;
	
	TIME_DIVISION = 200;
}

//****************************************************************************
//
// * provide a default set of points
//============================================================================
void CTrack::
resetPoints()
//============================================================================
{

	points.clear();
	points.push_back(ControlPoint(Pnt3f(50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,50)));
	points.push_back(ControlPoint(Pnt3f(-50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,-50)));

	// we had better put the train back at the start of the track...
	trainU = 0.0;
}

//****************************************************************************
//
// * Handy utility to break a string into a list of words
//============================================================================
void breakString(char* str, std::vector<const char*>& words) 
//============================================================================
{
	// start with no words
	words.clear();

	// scan through the string, starting at the beginning
	char* p = str;

	// stop when we hit the end of the string
	while(*p) {
		// skip over leading whitespace - stop at the first character or end of string
		while (*p && *p<=' ') p++;

		// now we're pointing at the first thing after the spaces
		// make sure its not a comment, and that we're not at the end of the string
		// (that's actually the same thing)
		if (! (*p) || *p == '#')
		break;

		// so we're pointing at a word! add it to the word list
		words.push_back(p);

		// now find the end of the word
		while(*p > ' ') p++;	// stop at space or end of string

		// if its ethe end of the string, we're done
		if (! *p) break;

		// otherwise, turn this space into and end of string (to end the word)
		// and keep going
		*p = 0;
		p++;
	}
}

//****************************************************************************
//
// * The file format is simple
//   first line: an integer with the number of control points
//	  other lines: one line per control point
//   either 3 (X,Y,Z) numbers on the line, or 6 numbers (X,Y,Z, orientation)
//============================================================================
void CTrack::
readPoints(const char* filename)
//============================================================================
{
	FILE* fp = fopen(filename,"r");
	if (!fp) {
		printf("Can't Open File!\n");
	} 
	else {
		char buf[512];

		// first line = number of points
		fgets(buf,512,fp);
		size_t npts = (size_t) atoi(buf);

		if( (npts<4) || (npts>65535)) {
			printf("Illegal Number of Points Specified in File");
		} else {
			points.clear();
			// get lines until EOF or we have enough points
			while( (points.size() < npts) && fgets(buf,512,fp) ) {
				Pnt3f pos,orient;
				vector<const char*> words;
				breakString(buf,words);
				if (words.size() >= 3) {
					pos.x = (float) strtod(words[0],0);
					pos.y = (float) strtod(words[1],0);
					pos.z = (float) strtod(words[2],0);
				} else {
					pos.x=0;
					pos.y=0;
					pos.z=0;
				}
				if (words.size() >= 6) {
					orient.x = (float) strtod(words[3],0);
					orient.y = (float) strtod(words[4],0);
					orient.z = (float) strtod(words[5],0);
				} else {
					orient.x = 0;
					orient.y = 1;
					orient.z = 0;
				}
				orient.normalize();
				points.push_back(ControlPoint(pos,orient));
			}
		}
		fclose(fp);
	}
	trainU = 0;
}

//****************************************************************************
//
// * write the control points to our simple format
//============================================================================
void CTrack::
writePoints(const char* filename)
//============================================================================
{
	FILE* fp = fopen(filename,"w");
	if (!fp) {
		printf("Can't open file for writing");
	} else {
		fprintf(fp,"%d\n",points.size());
		for(size_t i=0; i<points.size(); ++i)
			fprintf(fp,"%g %g %g %g %g %g\n",
				points[i].pos.x, points[i].pos.y, points[i].pos.z, 
				points[i].orient.x, points[i].orient.y, points[i].orient.z);
		fclose(fp);
	}
}

void CTrack::
getGM(Pnt3f gPos[4], Pnt3f gOrient[4], float mat[4][4]) {

	for (int j = 0; j < 4; j++) {
		gmPos[j] = Pnt3f(0.0f, 0.0f, 0.0f);
		gmOrient[j] = Pnt3f(0.0f, 0.0f, 0.0f);
	}

	for (int j = 0; j < 4; j++) {

		for (int i = 0; i < 4; i++) {
			gmPos[j] = gmPos[j] + gPos[i] * mat[i][j];
			gmOrient[j] = gmOrient[j] + gOrient[i] * mat[i][j];
		}
	}
}

Pnt3f CTrack::
getQt(float t, Pnt3f gm[4]) {

	Pnt3f result = Pnt3f(0, 0, 0);

	float T[4] = {
		t * t * t, t * t, t, 1
	};

	for (int i = 0; i < 4; i++) {
		result = result + gm[i] * T[i];
	}

	return result;
}

Pnt3f CTrack::
getVt(float t, Pnt3f gm[4]) {

	Pnt3f result = Pnt3f(0, 0, 0);

	float T[4] = {
		3 * t * t, 2 * t, 1, 0
	};

	for (int i = 0; i < 4; i++) {
		result = result + gm[i] * T[i];
	}

	return result;
}

void CTrack::
computeTrackPoints(int curve){
	//清除所有的軌道點

	float maxHeight = 0.0f;
	float minHeight = 100000.0f;

	Pnt3f gPos[4];
	Pnt3f gOrient[4];

	// 刪除所有紀錄點
	if (track_pts.size() >= 0) {
		for each (vector<TrackPoint *> ptsPerCP in track_pts){
			for each (TrackPoint * pt in ptsPerCP) delete pt;
			ptsPerCP.clear();
		}
		track_pts.clear();
	}

	for (int i = 0; i < points.size(); i++)
	{
		track_pts.push_back(vector<TrackPoint *>());

		for (int j = 0; j < 4; j++) {
			int index = (i + (j - 1) + points.size()) % points.size();

			gPos[j] = points[index].pos;
			gOrient[j] = points[index].orient;
		}

		Pnt3f qt, orient_t, front_t, up_t, right_t;

		float t = 0.0f, persent = 1.0 / TIME_DIVISION;

		switch (curve) {
		case 1:	//cardinal
			getGM(gPos, gOrient, M_Cardinal);
			break;
		case 2:	//cubic
			getGM(gPos, gOrient, M_B_Spline);
			break;
		}

		//針對各 subdivision 進行點的運算
		for (int j = 0; j < TIME_DIVISION + 1; j++)
		{
			//取得 時間點(t) 的 位置(pos) 指向(orient) 面向(front)

			t = j * 1.0f / TIME_DIVISION;

			switch (curve) {
			case 0:	//線性
				qt = (1 - t) * gPos[1] + t * gPos[2];
				orient_t = (1 - t) * gOrient[1] + t * gOrient[2];
				front_t = gPos[2] - gPos[1];
				break;
			case 1:	//cardinal
			case 2:	//cubic
				qt = getQt(t, gmPos);
				orient_t = getQt(t, gmOrient);
				front_t = getVt(t, gmPos);
				break;
			}

			if (qt.y > maxHeight)
				maxHeight = qt.y;

			if (qt.y < minHeight)
				minHeight = qt.y;

			//取得 右 與 上
			right_t = front_t * orient_t;
			up_t = right_t * front_t;

			//上左右 方向正規化
			front_t.normalize();
			up_t.normalize();

			track_pts[i].push_back(new TrackPoint(qt, up_t, front_t));
		}
		m_MaxHeight = maxHeight - minHeight;
		m_MinHeight = minHeight; 
		m_MaxHeight += 10.0f;
	}
}

void CTrack::
getTrainNextPos(float speedAndDir){

	// train1 & train2
	TrackPoint * prePt = this->track_pts[train1ControlPoint][train1Point];

	int tmp_i = train1ControlPoint * TIME_DIVISION + train1Point;

	float maxSpeed = sqrtf(9.8f * m_MaxHeight * 2.0f);

	float speed = sqrtf((m_MaxHeight - (prePt->pos.y - m_MinHeight))) * 0.1f * speedAndDir;

	tmp_i = getTrainNextTmpI(speed, tmp_i);

	train1Point = tmp_i % TIME_DIVISION;
	train1ControlPoint = tmp_i / TIME_DIVISION;

	tmp_i = getTrainNextTmpI(30, tmp_i);

	train2Point = tmp_i % TIME_DIVISION;
	train2ControlPoint = tmp_i / TIME_DIVISION;

	// NightHawk

	prePt = this->track_pts[nightHawkControlPoint][nightHawkPoint];

	tmp_i = nightHawkControlPoint * TIME_DIVISION + nightHawkPoint;

	speed = sqrtf((m_MaxHeight - (prePt->pos.y - m_MinHeight))) * 0.1f * speedAndDir;

	tmp_i = getTrainNextTmpI(speed, tmp_i);

	nightHawkPoint = tmp_i % TIME_DIVISION;
	nightHawkControlPoint = tmp_i / TIME_DIVISION;

	// Fighter

	prePt = this->track_pts[fighterControlPoint][fighterPoint];

	tmp_i = fighterControlPoint * TIME_DIVISION + fighterPoint;

	speed = sqrtf((m_MaxHeight - (prePt->pos.y - m_MinHeight))) * 0.1f * speedAndDir;

	tmp_i = getTrainNextTmpI(speed, tmp_i);

	fighterPoint = tmp_i % TIME_DIVISION;
	fighterControlPoint = tmp_i / TIME_DIVISION;
}

TrackPoint * CTrack::
getTrain1Pos(){
	return this->track_pts[train1ControlPoint][train1Point];
}

TrackPoint * CTrack::
getTrain2Pos() {
	return this->track_pts[train2ControlPoint][train2Point];
}

TrackPoint * CTrack::
getNightHawkPos(){
	return this->track_pts[nightHawkControlPoint][nightHawkPoint];
}

TrackPoint * CTrack::
getFighterPos(){
	return this->track_pts[fighterControlPoint][fighterPoint];
}


void CTrack::
drawTrack(bool doingShadows){

	float distance_Wood = 0.0f;
	float distance_Pillar = 0.0f;
	for (int i = 0; i < track_pts.size(); i++){
		for (int j = 0; j < TIME_DIVISION; j++){

			TrackPoint * backPoint = track_pts[i][j];
			TrackPoint * frontPoint = track_pts[i][j + 1];

			float dis = (frontPoint->pos - backPoint->pos).length();

			distance_Wood += dis;
			distance_Pillar += dis;

			if (distance_Wood > 7.5f){
				drawWood(backPoint->pos, backPoint->front, backPoint->up, doingShadows);
				distance_Wood = 0.0f;
			}
			
			if (distance_Pillar > 20.0f){
				if (backPoint->up.dotProduct(Pnt3f(0.0f, 1.0f, 0.0f)) > 0.0f)
					drawPillar(backPoint->pos, doingShadows);
				distance_Pillar = 0.0f;
			}

			Pnt3f
				right_front_up		= frontPoint->pos + frontPoint->right * 2.5 + frontPoint->up * 0.5,
				right_front_down	= frontPoint->pos + frontPoint->right * 2.5 - frontPoint->up * 0.5,
				left_front_up		= frontPoint->pos + frontPoint->right * 1.5 + frontPoint->up * 0.5,
				left_front_down		= frontPoint->pos + frontPoint->right * 1.5 - frontPoint->up * 0.5,
				right_back_up		= backPoint->pos + backPoint->right * 2.5 + backPoint->up * 0.5,
				right_back_down		= backPoint->pos + backPoint->right * 2.5 - backPoint->up * 0.5,
				left_back_up		= backPoint->pos + backPoint->right * 1.5 + backPoint->up * 0.5,
				left_back_down		= backPoint->pos + backPoint->right * 1.5 - backPoint->up * 0.5;

			if (!doingShadows)
				glColor3ub(192, 192, 192);

			Pnt3f woodRight = (frontPoint->right + backPoint->right) * 0.5;
			Pnt3f woodUp = (frontPoint->up + backPoint->up) * 0.5;

			glBegin(GL_QUADS);
			glNormal3f(woodRight.x, woodRight.y, woodRight.z);
			glVertex3f(right_front_up.x, right_front_up.y, right_front_up.z);
			glVertex3f(right_front_down.x, right_front_down.y, right_front_down.z);
			glVertex3f(right_back_down.x, right_back_down.y, right_back_down.z);
			glVertex3f(right_back_up.x, right_back_up.y, right_back_up.z);
			glEnd();

			drawQuad(right_front_up, right_front_down, right_back_down, right_back_up, woodRight, 192, 192, 192, doingShadows);

			drawQuad(left_front_up, left_front_down, left_back_down, left_back_up, woodRight * (-1), 192, 192, 192, doingShadows);

			drawQuad(right_front_up, left_front_up, left_back_up, right_back_up, woodUp, 192, 192, 192, doingShadows);

			drawQuad(right_front_down, left_front_down, left_back_down, right_back_down, woodUp * (-1), 192, 192, 192, doingShadows);

			right_front_up = right_front_up - frontPoint->right * 4;
			right_front_down = right_front_down - frontPoint->right * 4;
			left_front_up = left_front_up - frontPoint->right * 4;
			left_front_down = left_front_down - frontPoint->right * 4;
			right_back_up = right_back_up - backPoint->right * 4;
			right_back_down = right_back_down - backPoint->right * 4;
			left_back_up = left_back_up - backPoint->right * 4;
			left_back_down = left_back_down - backPoint->right * 4;

			//左
			drawQuad(right_front_up, right_front_down, right_back_down, right_back_up, woodRight, 192, 192, 192, doingShadows);

			drawQuad(left_front_up, left_front_down, left_back_down, left_back_up, woodRight * (-1), 192, 192, 192, doingShadows);

			drawQuad(right_front_up, left_front_up, left_back_up, right_back_up, woodUp, 192, 192, 192, doingShadows);

			drawQuad(right_front_down, left_front_down, left_back_down, right_back_down, woodUp * (-1), 192, 192, 192, doingShadows);
		}
	}
}

void CTrack::
drawWood(Pnt3f pos, Pnt3f front, Pnt3f up, bool doingShadows) {

	//normalization
	front.normalize();
	up.normalize();
	Pnt3f right = front * up;

	//決定枕木大小
	right = right * 5;

	drawCube(pos, front, up, right, 0x84, 0x42, 0x00, doingShadows);
}

void CTrack::
drawQuad(Pnt3f rt, Pnt3f rb, Pnt3f lb, Pnt3f lt, Pnt3f normal, int r, int g, int b, bool doingShadows) {

	if (!doingShadows)
		glColor3ub(r, g, b);

	glBegin(GL_QUADS);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3d(rt.x, rt.y, rt.z);
	glVertex3d(rb.x, rb.y, rb.z);
	glVertex3d(lb.x, lb.y, lb.z);
	glVertex3d(lt.x, lt.y, lt.z);
	glEnd();
}

void CTrack::
drawCube(Pnt3f pos, Pnt3f front, Pnt3f up, Pnt3f right, int r, int g, int b, bool doingShadows) {
	Pnt3f rt, rb, lb, lt;
	Pnt3f normalUp, normalRight, normalFront;

	normalUp = up;
	normalUp.normalize();

	normalRight = right;
	normalRight.normalize();

	normalFront = front;
	normalFront.normalize();


	// *-|-*
	// |   |
	// - * -
	// |   |
	// *-|-*

	// 前面
	rt = pos + front + right + up;
	rb = rt - 2 * up;
	lb = rb - 2 * right;
	lt = lb + 2 * up;

	drawQuad(rt, rb, lb, lt, normalFront, r, g, b, doingShadows);

	//後面
	rt = rt - 2 * front;
	rb = rb - 2 * front;
	lb = lb - 2 * front;
	lt = lt - 2 * front;

	drawQuad(rt, rb, lb, lt, normalFront * (-1), r, g, b, doingShadows);

	//右面
	rt = pos + front + right + up;
	rb = rt - 2 * up;
	lb = rb - 2 * front;
	lt = lb + 2 * up;

	drawQuad(rt, rb, lb, lt, normalRight, r, g, b, doingShadows);

	//左面
	rt = rt - 2 * right;
	rb = rb - 2 * right;
	lb = lb - 2 * right;
	lt = lt - 2 * right;

	drawQuad(rt, rb, lb, lt, normalRight * (-1), r, g, b, doingShadows);

	//上面
	rt = pos + front + right + up;
	rb = rt - 2 * front;
	lb = rb - 2 * right;
	lt = lb + 2 * front;

	drawQuad(rt, rb, lb, lt, normalUp, r, g, b, doingShadows);

	//下面
	rt = rt - 2 * up;
	rb = rb - 2 * up;
	lb = lb - 2 * up;
	lt = lt - 2 * up;

	drawQuad(rt, rb, lb, lt, normalUp * (-1), r, g, b, doingShadows);
}

void CTrack::
drawPillar(Pnt3f pos, bool doingShadows) {

	Pnt3f rt, rb, lb, lt;

	pos = pos - Pnt3f(0, 2, 0);

	Pnt3f right(1, 0, 0);
	Pnt3f front(0, 0, 1);
	Pnt3f center(pos.x, pos.y / 2, pos.z);
	Pnt3f up = pos - center;

	drawCube(center, front, up, right, 0, 0, 0, doingShadows);
}

int CTrack::getTrainNextTmpI(float speed, int tmp_i) {

	TrackPoint * prePt = this->track_pts[tmp_i / TIME_DIVISION][tmp_i % TIME_DIVISION];

	float sumDistance = 0;
	while (true)
	{
		if (speed > 0) {
			if (tmp_i >= TIME_DIVISION * points.size()) {
				tmp_i %= TIME_DIVISION * points.size();
			}

			TrackPoint * tempPt = this->track_pts[tmp_i / TIME_DIVISION][tmp_i % TIME_DIVISION];
			sumDistance += (tempPt->pos - prePt->pos).length();
			prePt = tempPt;

			if (sumDistance > speed) break;

			tmp_i++;
		}
		else if (speed < 0) {
			if (tmp_i <= 0) {
				tmp_i = TIME_DIVISION * points.size() - 1;
			}

			TrackPoint * tempPt = this->track_pts[tmp_i / TIME_DIVISION][tmp_i % TIME_DIVISION];
			sumDistance += (tempPt->pos - prePt->pos).length();
			prePt = tempPt;

			if (sumDistance > -speed) break;

			tmp_i--;
		}
		else
			break;
	}
	return tmp_i;
}
