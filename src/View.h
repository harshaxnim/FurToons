#ifndef VIEW_H
#define VIEW_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

class View{
public:
	void initializeWindow();
	void assignCallBacks(void reshape(int, int),
							void display(),
							void keyboard(unsigned char, int, int),
							void mouse(int, int, int, int),
							void motion(int, int)
						);
	void kickOff();

	void updateDisplay();
	void refresh();
};

#endif