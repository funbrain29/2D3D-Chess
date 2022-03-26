#pragma once

#include "glut.h"


class Piece
{
private:
	double mX, mY, mZ;
	int mPieceNumber;

public:
	Piece(int pn, double x, double y, double z);
	void Draw();

	double getX() { return mX; }
	double getY() { return mY; }
	double getZ() { return mZ; }
	
	double setX(double x) { mX = x; }
	double setY(double y) { mY = y; }
	double setZ(double z) { mZ = z; }
};

