#ifndef CONTROLLER_H
#define CONTROLLER_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

#include "View.h"
#include "Model.h"
// #include "utils/FastTrackball.h"

struct furMeta {
	float x;
	float y;

	double ax, ay;
	double bx, by;
	double cx, cy;
	double dx, dy;

	float xn;
	float yn;
} typedef furMeta;

namespace Controller{

	void init();
	void loadPly(string s);
	
	void display();
	void reshape(int x, int y);
	void keyboard(unsigned char c, int x, int y);
	void mouse(int b, int s, int x, int y);
	void motion(int x, int y);

	void drawAxes();
	void drawMesh();
	void drawModel(int);
	void drawBoundingBox();
	void drawLights();

	// void setParams(200,10.5,0.15,{0.7,0.5,0.3},1);
	void setParams(int s, float d, float l, int r, int f);
	void setFur(string c);
	void prettyFur();
	void makeFur();
	
	void snapDesire(vector<GLfloat>,vector<GLfloat>,vector<GLfloat>);
	void snap(vector<GLfloat>,vector<GLfloat>,vector<GLfloat>);
	vector<furMeta> readPrecursor();
	int writeImage(char* filename, int width, int height, float *buffer, char* title);
};

#endif
