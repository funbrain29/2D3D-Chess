#include "pieces.h"

Piece::Piece(int pn, double x, double y, double z) {
	mPieceNumber = pn;
	mX = x;
	mY = y;
	mZ = z;
}

void Piece::Draw() {
	glPushMatrix();
	glTranslatef(mX+500, mY, mZ+500);
    glRotated(180, 0, 1, 0);
	glCallList(mPieceNumber);
	glPopMatrix();
}