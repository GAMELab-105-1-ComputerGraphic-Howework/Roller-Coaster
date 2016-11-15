#include "TrackPoint.h"


TrackPoint::TrackPoint(const Pnt3f _pos, const Pnt3f _up, Pnt3f _front)
	: pos(_pos), up(_up), front(_front)
{
	up.normalize();
	front.normalize();
	right = front * up;
}

TrackPoint::~TrackPoint()
{
}
