#pragma once

#include "Utilities/Pnt3f.H"

class TrackPoint
{
public:

	TrackPoint(const Pnt3f _pos, const Pnt3f _up, Pnt3f _front);

	~TrackPoint();

public:
	Pnt3f pos;
	Pnt3f up;
	Pnt3f front;
	Pnt3f right;


	int rail_type;
	int draw_tile;

	double distance;   // distance between this point to next point on the track.
};