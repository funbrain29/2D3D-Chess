// (your name here)
// Chess animation starter kit.

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
using namespace std;
#include "glut.h"
#include "graphics.h"
#include "pieces.h"


// Global Variables
// Some colors you can use, or make your own and add them
// here and in graphics.h
GLfloat brownMaterial[] = {099.0/255.0, 070.0/255.0, 045.0/255.0, 1.0};
GLfloat whiteMaterial[] = {242.0/255.0, 225.0/255.0, 195.0/255.0, 1.0};
GLfloat blackMaterial[] = {195.0/255.0, 160.0/255.0, 130.0/255.0, 1.0};
GLfloat wMaterial[]		= {194.0/255.0, 209.0/255.0, 204.0/255.0, 1.0};
GLfloat bMaterial[]		= { 0.2f, 0.2f, 0.2f, 1.0f};

double screen_x = 1000;
double screen_y = 1000;

enum piece_number{bpawn=100,bking,bqueen,brook,bbishop,bknight,wpawn,wking,wqueen,wrook,wbishop,wknight};
int LASTMODE = 0;
int MODE = 0;
bool ROTATE = false;
double MODETIME = 0.0;
double ANIMTIME = 1000000; //Animation Start Time



double GetTime()
{
	static clock_t start_time = clock();
	clock_t current_time = clock();
	double total_time = double(current_time - start_time) / CLOCKS_PER_SEC;
	return total_time;
}

// Outputs a string of text at the specified location.
void text_output(double x, double y, const char *string)
{
	void *font = GLUT_BITMAP_9_BY_15;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
	
	int len, i;
	glRasterPos2d(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) 
	{
		glutBitmapCharacter(font, string[i]);
	}

    glDisable(GL_BLEND);
}

// Given the three triangle points x[0],y[0],z[0],
//		x[1],y[1],z[1], and x[2],y[2],z[2],
//		Finds the normal vector n[0], n[1], n[2].
void FindTriangleNormal(double x[], double y[], double z[], double n[])
{
	// Convert the 3 input points to 2 vectors, v1 and v2.
	double v1[3], v2[3];
	v1[0] = x[1] - x[0];
	v1[1] = y[1] - y[0];
	v1[2] = z[1] - z[0];
	v2[0] = x[2] - x[0];
	v2[1] = y[2] - y[0];
	v2[2] = z[2] - z[0];
	
	// Take the cross product of v1 and v2, to find the vector perpendicular to both.
	n[0] = v1[1]*v2[2] - v1[2]*v2[1];
	n[1] = -(v1[0]*v2[2] - v1[2]*v2[0]);
	n[2] = v1[0]*v2[1] - v1[1]*v2[0];

	double size = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	n[0] /= -size;
	n[1] /= -size;
	n[2] /= -size;
}

void SetPerspectiveView(int w, int h, double fov)
{
	glEnable(GL_LIGHTING);	// enable general lighting
	glEnable(GL_LIGHT0);	// enable the first light.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double aspectRatio = (GLdouble) w/(GLdouble) h;
	gluPerspective( 
	/* field of view in degree */ fov,
	/* aspect ratio */ aspectRatio,
	/* Z near */ 100, /* Z far */ 50000.0);
	glMatrixMode(GL_MODELVIEW);
}

// example 9, 10
void SetOrthographicView(int w, int h)
{
	glDisable(GL_LIGHT0);	// disable the first light.
	glDisable(GL_LIGHTING);	// disable general lighting
	double dw = w;
	double dh = h;
	double l, r, b, t;
	double ratio;
	if (w > h) {
		ratio = 9000 / dh;
		double extraToAdd = (dw-dh) * ratio / 2.0;
		t = 8500.0;
		b = -500.0;
		l = -8500.0 - extraToAdd;
		r = 500.0 + extraToAdd;
	}
	else if (w < h) {
		ratio = 9000 / dw;
		double extraToAdd = (dh-dw) * ratio / 2.0;
		t = 8500.0 + extraToAdd;
		b = -500.0 - extraToAdd;
		l = -8500.0;
		r = 500.0;
	}
	else {
		t = 8500.0;
		b = -500.0;
		l = -8500.0;
		r = 500.0;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(l,r,b,t,-50000,50000);
	glMatrixMode(GL_MODELVIEW);
}

void DrawPieceObj(const char filename[], float scale, bool flipNormals) {

	// Try to open the given file.
	fstream in;
	in.open(filename, ios::in);
	if(!in)
	{
		cerr << "Error. Could not open " << filename << endl;
		exit(1);
	}

	vector<vector<double>> vertices;
	vector<vector<double>> verticeNormals;

	string line;
	if (in.is_open()) {
		while (getline(in, line)) {
			istringstream iss(line);
			string str;
			vector<string> words;
			while (getline(iss, str, ' ')) {
				words.push_back(str);
			}
				if (words[0] == "v") {
					// Vertice
					float x = stof(words[1]);
					float y = stof(words[2]);
					float z = stof(words[3]);
					vector<double> v; 
					v.push_back(x);
					v.push_back(y);
					v.push_back(z);
					vertices.push_back(v);
				}
				else if (words[0] == "vn") {
					// Vertice Normal
					float x = stof(words[1]);
					float y = stof(words[2]);
					float z = stof(words[3]);
					vector<double> v; 
					v.push_back(x);
					v.push_back(y);
					v.push_back(z);
					verticeNormals.push_back(v);
				}
				else if (words[0] == "f") {
					// Face line
					glBegin(GL_POLYGON);

					for (unsigned int i = 1; i < words.size(); i++) {
						// for each word in face line
						istringstream iss(words[i]);
						string str;
						vector<string> facenums;

						while (getline(iss, str, '/')) {
							facenums.push_back(str);
						}
						vector<double> vn = verticeNormals[stoi(facenums[2]) - 1];
						vector<double> v = vertices[stoi(facenums[0]) - 1];
						float xn = vn[0] * scale;
						float yn = vn[1] * scale;
						float zn = vn[2] * scale;
						if (flipNormals) {
							xn *= -1;
							yn *= -1;
							zn *= -1;
						}
						
						double size = sqrt(xn*xn + yn*yn + zn*zn);
						xn /= -size;
						yn /= -size;
						zn /= -size;

						glNormal3d(xn,yn,zn);
						float x = v[0] * scale;
						float y = v[1] * scale;
						float z = v[2] * scale;

						glVertex3d(x,y,z);

					}
					glEnd();
				}
		}
	}


}

// Loads the given data file and draws it at its default position.
// Call glTranslate before calling this to get it in the right place.
void DrawPiece(const char filename[])
{
	// Try to open the given file.
	char buffer[200];
	ifstream in(filename);
	if(!in)
	{
		cerr << "Error. Could not open " << filename << endl;
		exit(1);
	}

	double x[100], y[100], z[100]; // stores a single polygon up to 100 vertices.
	int done = false;
	int verts = 0; // vertices in the current polygon
	int polygons = 0; // total polygons in this file.
	do
	{
		in.getline(buffer, 200); // get one line (point) from the file.
		int count = sscanf(buffer, "%lf, %lf, %lf", &(x[verts]), &(y[verts]), &(z[verts]));
		done = in.eof();
		if(!done)
		{
			if(count == 3) // if this line had an x,y,z point.
			{
				verts++;
			}
			else // the line was empty. Finish current polygon and start a new one.
			{
				if(verts>=3)
				{
					glBegin(GL_POLYGON);
					double n[3];
					FindTriangleNormal(x, y, z, n);
					glNormal3dv(n);
					for(int i=0; i<verts; i++)
					{
						glVertex3d(x[i], y[i], z[i]);
					}
					glEnd(); // end previous polygon
					polygons++;
					verts = 0;
				}
			}
		}
	}
	while(!done);

	if(verts>0)
	{
		cerr << "Error. Extra vertices in file " << filename << endl;
		exit(1);
	}

}

// NOTE: Y is the UP direction for the chess pieces.
double eye[3] = {4000, 8000, -7000}; // pick a nice vantage point.
double at[3]  = {4000, 0,     4000};
//
// GLUT callback functions
//

// As t goes from t0 to t1, set v between v0 and v1 accordingly.
void Interpolate(double t, double t0, double t1,
	double & v, double v0, double v1)
{
	double ratio = (t - t0) / (t1 - t0);
	if (ratio < 0)
		ratio = 0;
	if (ratio > 1)
		ratio = 1;
	v = v0 + (v1 - v0)*ratio;
}

void QuadraticInterpolate(double t, double t0, double t1,
	double x0, double y0, double z0,
	double x1, double y1, double z1,
	double x2, double y2, double z2,
	double& x, double& y, double& z
	) {
	double ratio = (t - t0) / (t1 - t0);
	if (ratio < 0)
		ratio = 0;
	if (ratio > 1)
		ratio = 1;
	x = (1 - ratio) * (1 - ratio) * x0 + 2 * (1 - ratio) * ratio * x1 + ratio * ratio * x2;
	y = (1 - ratio) * (1 - ratio) * y0 + 2 * (1 - ratio) * ratio * y1 + ratio * ratio * y2;
	z = (1 - ratio) * (1 - ratio) * z0 + 2 * (1 - ratio) * ratio * z1 + ratio * ratio * z2;
}

void drawBoard() {
// draw board

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i % 2 == 0 && j % 2 != 0) || (i % 2 != 0 && j % 2 == 0) ) {
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blackMaterial);
				glColor3d(blackMaterial[0], blackMaterial[1], blackMaterial[2]);
			}
			else {
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, whiteMaterial);
				glColor3d(whiteMaterial[0], whiteMaterial[1], whiteMaterial[2]);
			}

			int x = i * 1000;
			int y = 0;
			int z = j * 1000;

			glBegin(GL_POLYGON);
			glNormal3d(0, 1, 0);

			glVertex3d(x,      y, z     );
			glVertex3d(x+1000, y, z     );
			glVertex3d(x+1000, y, z+1000);
			glVertex3d(x     , y, z+1000);

			glEnd();
		}
	}

	// Draw Edges

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, brownMaterial);
	glColor3d(brownMaterial[0], brownMaterial[1], brownMaterial[2]);

	// north top edge
	glBegin(GL_POLYGON);
	glNormal3d(0, 1, 0);
	glVertex3d(-250, 0, 8000);
	glVertex3d(8250, 0, 8000);
	glVertex3d(8250, 0, 8250);
	glVertex3d(-250, 0, 8250);
	glEnd();


	// east top edge
	glBegin(GL_POLYGON);
	glNormal3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 8000);
	glVertex3d(-250, 0, 8000);
	glVertex3d(-250, 0, 0);
	glEnd();

	// south top edge
	glBegin(GL_POLYGON);
	glNormal3d(0, 1, 0);
	glVertex3d(-250, 0, 0);
	glVertex3d(8250, 0, 0);
	glVertex3d(8250, 0, -250);
	glVertex3d(-250, 0, -250);
	glEnd();

	// west top edge
	glBegin(GL_POLYGON);
	glNormal3d(0, 1, 0);
	glVertex3d(8000, 0, 0);
	glVertex3d(8250, 0, 0);
	glVertex3d(8250, 0, 8000);
	glVertex3d(8000, 0, 8000);
	glEnd();

	glEnd();
	
	// north bottom edge
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glVertex3d(-250, 0, 8250);
	glVertex3d(-250, -250, 8250);
	glVertex3d(8250, -250, 8250);
	glVertex3d(8250, 0, 8250);
	glEnd();

	// east bottom edge
	glBegin(GL_POLYGON);
	glNormal3d(-1, 0, 0);
	glVertex3d(-250, 0, -250);
	glVertex3d(-250, -250, -250);
	glVertex3d(-250, -250, 8250);
	glVertex3d(-250, 0, 8250);
	glEnd();

	// south bottom edge
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, -1);
	glVertex3d(-250, 0, -250);
	glVertex3d(8250, 0, -250);
	glVertex3d(8250, -250, -250);
	glVertex3d(-250, -250, -250);
	glEnd();

	// west bottom edge
	glBegin(GL_POLYGON);
	glNormal3d(1, 0, 0);
	glVertex3d(8250, 0, -250);
	glVertex3d(8250, -250, -250);
	glVertex3d(8250, -250, 8250);
	glVertex3d(8250, 0, 8250);
	glEnd();

	// bottom of board
	glBegin(GL_POLYGON);
	glNormal3d(0, -1, 0);
	glVertex3d(-250, -250, -250);
	glVertex3d(-250, -250, 8250);
	glVertex3d(8250, -250, 8250);
	glVertex3d(8250, -250, -250);
	glEnd();
}

// This callback function gets called by the Glut
// system whenever it decides things need to be redrawn.
void display(void)
{
	double t = GetTime();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double x, y, z, d = 0;

	/*
	QuadraticInterpolate(t, 1.0, 10.0,
		4000, 8000, -7000,
		-5000, 8000, -5000,
		-7000, 8000, 4000,
		x, y, z);

	eye[0] = x;
	eye[1] = y;
	eye[2] = z;
	*/


	d = sqrt((eye[0] - at[0]) * (eye[0] - at[0]) + (eye[1] - at[1]) * (eye[1] - at[1]) + (eye[2] - at[2] * (eye[2] - at[2])));

	glLoadIdentity();

	// Camera Variables
	double rotationTime = 3.0;
	double animLength = 1.0;

	switch (MODE) // Display Depending on Mode
	{
	case 0: // Perspective
		if (t > rotationTime && ROTATE) {
			SetPerspectiveView(screen_x,screen_y,45);
			gluLookAt(4000 + d * cos(t-rotationTime), eye[1], 4000 + d * sin(t-rotationTime), at[0], at[1], at[2], 0, 1, 0); // Y is up!
		}
		else {
			if (LASTMODE == 2 || LASTMODE == 1) {
				QuadraticInterpolate(t, MODETIME, MODETIME+animLength,
				4000, 46700, 4000,
				4000, 20000, 0,
				eye[0], eye[1], eye[2],
				x, y, z);
				Interpolate(t, MODETIME, MODETIME+animLength, d, 11, 45);
				SetPerspectiveView(screen_x,screen_y,d);
				gluLookAt(x, y, z, at[0], at[1], at[2], 0, 1, 0); // Y is up!
			}
			else if (LASTMODE == 0 ) {
				SetPerspectiveView(screen_x,screen_y,45);
				gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], 0, 1, 0); // Y is up!
			}
		}
		break;
	case 1: // Orthographic
		if (LASTMODE == 0 && t < MODETIME + animLength && !ROTATE) {
			QuadraticInterpolate(t, MODETIME, MODETIME + animLength,
				eye[0], eye[1], eye[2],
				4000, 20000, 0,
				4000, 46700, 4000,
				x, y, z);
			Interpolate(t, MODETIME, MODETIME + animLength, d, 45, 11);
			SetPerspectiveView(screen_x, screen_y, d);
			gluLookAt(x, y, z, at[0], at[1], at[2], 0, 1, 0); // Y is up!
		}
		else {
			SetOrthographicView(screen_x, screen_y);
			gluLookAt(1, 1, 1,
				1, 0, 1,
				0, 0, 1); // Z is up!
		}
		break;
	case 2: // Birds eye Perspective
		if (LASTMODE == 0 && !ROTATE) {
			QuadraticInterpolate(t, MODETIME, MODETIME + animLength,
				eye[0], eye[1], eye[2],
				4000, 20000, 0,
				4000, 46700, 3999,
				x, y, z);
			Interpolate(t, MODETIME, MODETIME + animLength, d, 45, 11);
			SetPerspectiveView(screen_x, screen_y, d);
			gluLookAt(x, y, z, at[0], at[1], at[2], 0, 1, 0); // Y is up!
		}
		else {
			SetPerspectiveView(screen_x, screen_y, 11);
			gluLookAt(4000, 46700, 3999, at[0], at[1], at[2], 0, 1, 0); // Y is up!
		}
		
		break;
	}
	


	// setup piece variables
	double A = ANIMTIME;

	
	// Set the color for one side (white), and draw its 16 pieces.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wMaterial);
	glColor3d(wMaterial[0], wMaterial[1], wMaterial[2]);

	
	Piece wPawn0(wpawn,0000,0,1000);
	wPawn0.Draw();

	Piece wPawn1(wpawn,1000,0,1000);
	wPawn1.Draw();

	Piece wPawn2(wpawn,2000,0,1000);
	wPawn2.Draw();

	// Move 4 W e4
	QuadraticInterpolate(t, A+6, A+7,
		3000, 0000, 1000,
		3000, 2000, 2000,
		3000, 0000, 3000,
		x, y, z);
	Piece wPawn3(wpawn,x,y,z);
	wPawn3.Draw();

	// Move 1 W d4 
	QuadraticInterpolate(t, A, A+1,
		4000, 0, 1000,
		4000, 2000, 2000,
		4000, 0, 3000,
		x, y, z);
	Piece wPawn4(wpawn,x,y,z);
	wPawn4.Draw();

	Piece wPawn5(wpawn,5000,0,1000);
	wPawn5.Draw();

	Piece wPawn6(wpawn,6000,0,1000);
	wPawn6.Draw();

	Piece wPawn7(wpawn,7000,0,1000);
	wPawn7.Draw();

	Piece wRook0(wrook,0000,0,0000);
	wRook0.Draw();

	Piece wKnight0(wknight,1000,0,0000);
	wKnight0.Draw();

	Piece wBishop0(wbishop,2000,0,0000);
	wBishop0.Draw();

	Piece wKing(wking,3000,0,0000);
	wKing.Draw();

	// Move 5 W qh5
	QuadraticInterpolate(t, A + 8, A + 9,
		4000, 0000, 0000,
		2000, 1000, 2000,
		0000, 0000, 4000,
		x, y, z);
	Piece wQueen(wqueen,x,y,z);
	wQueen.Draw();

	
	// Move 2 W bg5
	QuadraticInterpolate(t, A + 2, A + 3,
		5000, 0000, 0000,
		3000, 1000, 2000,
		1000, 0000, 4000,
		x, y, z);
	// Move 3 W bh4
	if (t > A + 4) {
	QuadraticInterpolate(t, A+4, A+5,
		1000, 0000, 4000,
		0500, 1000, 3500,
		0000, 0000, 3000,
		x, y, z);
	}
	// Bishop Taken Move 4 B gxh4
	if (t > A + 7.8) {
		z = -60000;
	}
	Piece wBishop1(wbishop,x,y,z);
	wBishop1.Draw();

	Piece wKnight1(wknight,6000,0,0000);
	wKnight1.Draw();

	Piece wRook1(wrook,7000,0,0000);
	wRook1.Draw();

	// Set the color for the other side (black), and draw its 16 pieces.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bMaterial);
	glColor3d(bMaterial[0], bMaterial[1], bMaterial[2]);

	// Move 2 B p h6
	QuadraticInterpolate(t, A+3, A+4,
		0000, 0000, 6000,
		0000, 2000, 5500,
		0000, 0000, 5000,
		x, y, z);
	Piece bPawn0(bpawn,x,y,z);
	bPawn0.Draw();

	// Move 3 B p g5
	QuadraticInterpolate(t, A+5, A+6,
		1000, 0000, 6000,
		1000, 2000, 5000,
		1000, 0000, 4000,
		x, y, z);
	// Move 4 B gxh4
	if (t > A + 7) {
		QuadraticInterpolate(t, A + 7, A + 8,
			1000, 0000, 4000,
			0500, 2000, 3500,
			0000, 0000, 3000,
			x, y, z);
	}
	Piece bPawn1(bpawn,x,y,z);
	bPawn1.Draw();

	// Move 1 B p f5
	QuadraticInterpolate(t, A+1, A+2,
		2000, 0, 6000,
		2000, 2000, 5000,
		2000, 0, 4000,
		x, y, z);
	Piece bPawn2(bpawn,x,y,z);
	bPawn2.Draw();
	
	Piece bPawn3(bpawn,3000,0,6000);
	bPawn3.Draw();

	Piece bPawn4(bpawn,4000,0,6000);
	bPawn4.Draw();

	Piece bPawn5(bpawn,5000,0,6000);
	bPawn5.Draw();

	Piece bPawn6(bpawn,6000,0,6000);
	bPawn6.Draw();

	Piece bPawn7(bpawn,7000,0,6000);
	bPawn7.Draw();

	Piece bRook0(brook,0000,0,7000);
	bRook0.Draw();

	Piece bKnight0(bknight,1000,0,7000);
	bKnight0.Draw();

	Piece bBishop0(bbishop,2000,0,7000);
	bBishop0.Draw();

	Piece bKing(bking,3000,0,7000);
	bKing.Draw();

	Piece bQueen(bqueen,4000,0,7000);
	bQueen.Draw();

	Piece bBishop1(bbishop,5000,0,7000);
	bBishop1.Draw();

	Piece bKnight1(bknight,6000,0,7000);
	bKnight1.Draw();

	Piece bRook1(brook,7000,0,7000);
	bRook1.Draw();
	/*
	Piece wBishop(wbishop, 3000, 0, 1000);
	wBishop.Draw();

	QuadraticInterpolate(t, 1.0, 8.0,
		1000, 0, 4000,
		3000, 100, 4000,
		5000, 0, 4000,
		x, y, z);
	Piece wQueen(wqueen, x,y,z);
	wQueen.Draw();

	/*

	QuadraticInterpolate(t, A+1, A+3,
		1, 1, 1,
		1, 1.5, 1,
		1, 2, 1,
		x, y, z);
	glPushMatrix();
	glTranslatef(4000+500, 0, 4000+500);
    glRotated(180, 0, 1, 0);
	glScaled(x,y,z);
	glCallList(brook);
	glPopMatrix();

	QuadraticInterpolate(t, A+4, A+6,
		0, 1, 1,
		45, 1.5, 1,
		90, 2, 1,
		x, y, z);
	glPushMatrix();
	glTranslatef(3000+500, 0, 3000+500);
    glRotated(x, 0, 0, 1);
	glCallList(bking);
	glPopMatrix();




	Interpolate(t, 4.0, 6.0, x, 4000, 2000);
	glPushMatrix();
	glTranslatef(x, 0, 8000);
	glCallList(king);
	glPopMatrix();

	for(int x=1000; x<=8000; x+=1000)
	{
		glPushMatrix();
		glTranslatef(x, 0, 7000);
		glCallList(pawn);
		glPopMatrix();
	}

*/

	drawBoard();

	GLfloat light_position[] = {1,2,-.1f, 0}; // light comes FROM this vector direction.
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // position first light

	glutSwapBuffers();
	glutPostRedisplay();
}


// This callback function gets called by the Glut
// system whenever a key is pressed.
void keyboard(unsigned char c, int x, int y)
{
	switch (c) 
	{
		case 27: // escape character means to quit the program
			exit(0);
			break;
		case '1':
			LASTMODE = MODE;
			MODE = 0;
			MODETIME = GetTime();
			break;
		case '2':
			LASTMODE = MODE;
			MODE = 1;
			MODETIME = GetTime();
			break;
		case '3':
			LASTMODE = MODE;
			MODE = 2;
			MODETIME = GetTime();
			break;
		case 'r':
			ROTATE = !ROTATE;
			break;
		case ' ':
			ANIMTIME = GetTime();
			break;
		default:
			return; // if we don't care, return without glutPostRedisplay()
	}

	glutPostRedisplay();
}

// This callback function gets called by the Glut
// system whenever the window is resized by the user.
void reshape(int w, int h)
{
	screen_x = w;
	screen_y = h;

	// Set the pixel resolution of the final picture (Screen coordinates).
	glViewport(0, 0, w, h);

	if (MODE == 0) {
		SetPerspectiveView(w, h, 45);
	}
	else {
		SetOrthographicView(w, h);
	}

}

// This callback function gets called by the Glut
// system whenever any mouse button goes up or down.
void mouse(int mouse_button, int state, int x, int y)
{
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) 
	{
	}
	glutPostRedisplay();
}

// Your initialization code goes here.
void InitializeMyStuff()
{

	// set material's specular properties
	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[] = {50.0};
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// set light properties
	GLfloat light_position[] = {(float)eye[0], (float)eye[1], (float)eye[2],1};
	GLfloat white_light[] = {1,1,1,1};
	GLfloat low_light[] = {.3f,.3f,.3f,1};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // position first light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light); // specify first light's color
	glLightfv(GL_LIGHT0, GL_SPECULAR, low_light);

	glEnable(GL_DEPTH_TEST); // turn on depth buffering
	glEnable(GL_LIGHTING);	// enable general lighting
	glEnable(GL_LIGHT0);	// enable the first light.

	double scale = 400;

	// white pieces
	glNewList(wpawn,GL_COMPILE);
	DrawPieceObj("pieces/wpawn.obj",scale,true);
	glEndList();

	glNewList(wrook,GL_COMPILE);
	DrawPieceObj("pieces/wrook.obj",scale,true);
    glRotated(90, 0, 1, 0);
	glEndList();

	glNewList(wknight,GL_COMPILE);
	DrawPieceObj("pieces/wknight.obj", scale,true);
    glRotated(90, 0, 1, 0);
	glEndList();

	glNewList(wbishop,GL_COMPILE);
	DrawPieceObj("pieces/wbishop.obj", scale,false);
    glRotated(90, 0, 1, 0);
	glEndList();

	glNewList(wqueen,GL_COMPILE);
	DrawPieceObj("pieces/wqueen.obj",scale,true);
    glRotated(90, 0, 1, 0);
	glEndList();

	glNewList(wking,GL_COMPILE);
	DrawPieceObj("pieces/wking.obj",scale,true);
    glRotated(90, 0, 1, 0);
	glEndList();

	// black pieces
	glNewList(bpawn,GL_COMPILE);
	DrawPieceObj("pieces/bpawn.obj",scale,true);
	glEndList();

	glNewList(brook,GL_COMPILE);
	DrawPieceObj("pieces/brook.obj",scale,true);
	glEndList();

	glNewList(bknight,GL_COMPILE);
	DrawPieceObj("pieces/bknight.obj", scale,true);
	glEndList();

	glNewList(bbishop,GL_COMPILE);
	DrawPieceObj("pieces/bbishop.obj", scale,false);
	glEndList();

	glNewList(bqueen,GL_COMPILE);
	DrawPieceObj("pieces/bqueen.obj",scale,true);
	glEndList();

	glNewList(bking,GL_COMPILE);
	DrawPieceObj("pieces/bking.obj",scale,true);
	glEndList();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_x, screen_y);
	glutInitWindowPosition(10, 10);

	int fullscreen = 0;
	if (fullscreen) 
	{
		glutGameModeString("800x600:32");
		glutEnterGameMode();
	} 
	else 
	{
		glutCreateWindow("Shapes");
	}

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);

	glClearColor(0,0,0,1);	
	InitializeMyStuff();

	glutMainLoop();

	return 0;
}
