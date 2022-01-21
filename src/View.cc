
#include "View.h"
#include <iostream>

using namespace std;

void View::initializeWindow(){
	int i=0;
	glutInit(&i, NULL);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("toons");
}

void View::assignCallBacks(void reshape(int, int),
							void display(),
							void keyboard(unsigned char, int, int),
							void mouse(int, int, int, int),
							void motion(int, int)){
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
}

void View::kickOff(){
	glutMainLoop();
}

void View::refresh(){
	glutPostRedisplay();
}

void View::updateDisplay(){
	glutSwapBuffers();
}