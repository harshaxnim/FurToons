#include "Controller.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <SOIL.h>

using namespace std;

View view;
Model model;
// Trackball trackball;

int verbose = 0;

namespace Controller{

	// Fur Params
	// Teddy
	// int SPREAD = 150; // 0 to 254 strict
	// float DENSITY = 1.5; // 0 to 10 ideal
	// float LENGTH = 0.7; // 0 to 1 ideal
	// float furColor[] = {0.7,0.5,0.3};
	// int fixRotate = 0;
	// int manFur = 0;

	// DEFAULTS  ///////////////// !!! DO NOT EDIT !!! ////////////////
	string furTex = "furs/mySpline.png";
	int SPREAD = 200; // 0 to 254 strict
	float DENSITY = 10.5; // 0 to 10 ideal
	float LENGTH = 0.15; // 0 to 1 ideal
	float SCALERATE = 0.015;
	float furColor[] = {0.7,0.5,0.3};
	int fixRotate = 1;
	int manFur = 1;
	
	void setParams(int s, float d, float l, int r, int ft){
		SPREAD = s; // 0 to 254 strict
		DENSITY = d; // 0 to 10 ideal
		LENGTH = l; // 0 to 1 ideal
		fixRotate = r;	
		manFur = ft;
	}

	void setFur(string c){
		furTex = c;
	}


	int fur = 1;
	int capture = 1;

	// Camera Params
	float viewx = 0;
	float viewy = 0;
	float viewz = 2;
	float fovy = 70;
	// Lighting Location
	float lx=0;
	float ly=0;
	float lz=0;
	// Motion
	float xAng, yAng;
	// FB Size
	int dw=1440, dh=900;
	GLfloat light[] = { 1, 1, 1, 0 };
	GLfloat position[] = { lx, ly, lz, 1 };

	Analysis res;

	GLuint VBO, IBO, TEX;
	// Secindary Image
	GLuint FBO, TCB, RBO;
	GLuint CBO;
	vector<Vertex> cn;


	void loadTex(){
		cout << "Loading " << furTex << endl;
		// Load/Push Textures
		int width, height;
		unsigned char* image;
		width=0;
		height=0;
		image = SOIL_load_image(furTex.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		if (image == NULL){
			cout << "Failed to Load Fur." << endl;
			exit(-1);
		}
		glGenTextures(1, &TEX);
		glBindTexture(GL_TEXTURE_2D, TEX);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	void init(){
		srand(time(0));
		view.initializeWindow();
		cout << "Running OpenGL " << glGetString(GL_VERSION) << endl << endl;

		view.assignCallBacks(reshape, display, keyboard, mouse, motion);

		glClearColor(1,1,1,1);
		
		glEnable (GL_DEPTH_TEST);
		// glEnable( GL_BLEND );
		// glClearDepth(1);
		// glDepthFunc(GL_LESS);

		glEnable(GL_LIGHT0);
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
		// glLightfv(GL_LIGHT0, GL_AMBIENT, light);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
		glLightfv(GL_LIGHT0, GL_POSITION, position);



		loadTex();

		//Create a new VBO and use the variable id to store the VBO id
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, model.vertexSet.size()*sizeof(Vertex), &model.vertexSet[0], GL_STATIC_DRAW);

		// To color the normals
		glGenBuffers(1, &CBO);
		glBindBuffer(GL_ARRAY_BUFFER, CBO);
		for ( Vertex v : model.vertexSet ){
			Vertex temp;
			temp.xn = v.xn;
			temp.yn = v.yn;
			temp.zn = v.zn;
			cn.push_back(temp);
		}
		glBufferData(GL_ARRAY_BUFFER, cn.size()*sizeof(Vertex), &cn[0], GL_DYNAMIC_DRAW);


		// Using Indices
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.faces.size()*sizeof(Face), &model.faces[0], GL_STATIC_DRAW);
		

		// Using additional frame-buffer-object
		// glGenFramebuffers(1, &FBO);
		// glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		// glGenTextures(1, &TCB);
		// glBindTexture(GL_TEXTURE_2D, TCB);

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dw, dh, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// glBindTexture(GL_TEXTURE_2D, 0);

		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TCB, 0);

		// glGenRenderbuffers(1, &RBO);
		// glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dw, dh);
		// glBindRenderbuffer(GL_RENDERBUFFER, 0);
		// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		// if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		// 	cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		// 	cout << "v: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
		// 	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
		// 		cout << "1" << endl;
		// 	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_UNSUPPORTED)
		// 		cout << "3" << endl;
		// 	// exit(1);
		// }
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
		view.kickOff();
	}

	void loadPly(string s){
		cout << "Loading " << s << endl;
		model.loadPly(s);
		res = model.getAnalysis();
	}
	
	void reshape(int x, int y){
		dw = x;
		dh = y;
		glViewport(0,0,x,y);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fovy, ((float)x)/y, 0.01, 100);

		view.refresh();
	}

	void display(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);

		glLoadIdentity();
		gluPerspective(fovy, ((float)dw)/dh, 0.01, 100);
		
		
		glMatrixMode(GL_MODELVIEW);
		
		glLoadIdentity();
		gluLookAt(viewx,viewy,viewz,   0, 0, 0,   0.0, 1.0, 0.0);

		// static float rotation_transform[4][4];
		// trackball.rotationMatrix( rotation_transform ) ;
		// glMultMatrixf( &rotation_transform[0][0] ) ;
		// glLightfv(GL_LIGHT0, GL_POSITION, position);
		
		// drawLights();
		
		// glPushMatrix();
		// glRotatef(-90,1,0,0);
		// glTranslatef(-res.xCnt,-res.yCnt,-res.zCnt);
		// // drawBoundingBox();
		// glPopMatrix();
		
		// drawAxes();
		// drawMesh();
		if(fixRotate) glRotatef(-90,1,0,0);
		glTranslatef(-res.xCnt,-res.yCnt,-res.zCnt);
		if(fur) prettyFur();

		// exit(0);
		if(fur) makeFur();
		else drawModel(0);
		view.updateDisplay();
	}

	void makeFur(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		drawModel(0);

		vector<furMeta> meta = readPrecursor();
		
		glColor3f(furColor[0],furColor[1],furColor[2]);
		// glColor3f(0.7,0.5,0.5);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		// glBegin(GL_LINES);
		// glBegin(GL_POINTS);
		glPointSize(4);
		// glBegin(GL_POINTS);



		if (manFur){
			glEnable ( GL_BLEND );
			glEnable(GL_TEXTURE_2D);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable (GL_DEPTH_TEST);
			
			for(furMeta f: meta){

				// glColor3f(furColor[0],furColor[1],furColor[2]);
				// glBegin(GL_LINES);
				// 	glVertex3f(f.x, f.y, 0);
				// 	glVertex3f(f.xn, f.yn, 0);
				// glEnd();

				glColor3f(1,1,1);
				// glBegin(GL_LINE_LOOP);
				glBegin(GL_QUADS);
				// glColor3f(0,0,0);
				// glVertex3f(f.ax, f.ay, 0);
				// glColor3f(1,0,0);
				// glVertex3f(f.bx, f.by, 0);
				// glColor3f(0,1,0);
				// glVertex3f(f.cx, f.cy, 0);
				// glColor3f(0,0,1);
				// glVertex3f(f.dx, f.dy, 0);

				// glColor4f(0,0,0,1);
				glTexCoord2f(0,0);
				glVertex3f(f.ax, f.ay, 0);

				// glColor4f(1,0,0);
				glTexCoord2f(0,1);
				glVertex3f(f.bx, f.by, 0);
				
				// glColor4f(0,1,0);
				glTexCoord2f(1,1);
				glVertex3f(f.cx, f.cy, 0);
				
				// glColor4f(0,0,1,1);
				glTexCoord2f(1,0);
				glVertex3f(f.dx, f.dy, 0);
				glEnd();
			}
			glDisable(GL_TEXTURE_2D);
			glEnable (GL_DEPTH_TEST);
		}
		else {
			for(furMeta f: meta){

				glColor3f(furColor[0],furColor[1],furColor[2]);
				glBegin(GL_LINES);
					glVertex3f(f.x, f.y, 0);
					glVertex3f(f.xn, f.yn, 0);
				glEnd();
			}
		}


		// glEnd();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	void prettyFur(){
		// cout << fur << endl;
		if(1){
			drawModel(1);
			if(verbose) cout << "capturing.. desire image" << endl;
			
			vector<GLfloat> red(dw*dh);
			vector<GLfloat> green(dw*dh);
			vector<GLfloat> blue(dw*dh);
			glReadPixels(0, 0, dw, dh, GL_RED, GL_FLOAT, &red[0]);
			glReadPixels(0, 0, dw, dh, GL_GREEN, GL_FLOAT, &green[0]);
			glReadPixels(0, 0, dw, dh, GL_BLUE, GL_FLOAT, &blue[0]);
			
			snapDesire(red, green, blue);

			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawModel(2);

			if(verbose) cout << "capturing.. directions" << endl;
			glReadPixels(0, 0, dw, dh, GL_RED, GL_FLOAT, &red[0]);
			glReadPixels(0, 0, dw, dh, GL_GREEN, GL_FLOAT, &green[0]);
			glReadPixels(0, 0, dw, dh, GL_BLUE, GL_FLOAT, &blue[0]);
			
			snap(red, green, blue);
			glClearColor(1,1,1,1);


			capture = 0;
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	float absolute(float a){
		return a>0?a:-a;
	}
	void cross(float* dof, Vertex set, Vertex* cn){
		cn->xn=(set.zn*dof[1])-(set.yn*dof[2]);
		cn->yn=(set.xn*dof[2])-(set.zn*dof[0]);
		cn->zn=(set.yn*dof[0])-(set.xn*dof[1]);
	}
	void matMult44x41(float* l, float* r, Vertex* col){
		col->hn = (l[3]*r[0])+(l[7]*r[1])+(l[11]*r[2])+(l[15]*r[3]);
		col->xn = ((((l[0]*r[0])+(l[4]*r[1])+(l[ 8]*r[2])+(l[12]*r[3]))/col->hn)+1)/2;
		col->yn = ((((l[1]*r[0])+(l[5]*r[1])+(l[ 9]*r[2])+(l[13]*r[3]))/col->hn)+1)/2;
		col->zn = ((((l[2]*r[0])+(l[6]*r[1])+(l[10]*r[2])+(l[14]*r[3]))/col->hn)+1)/2;
		 
		col->hn = 1;
	}

	float rmin=0.5, rmax=0.5;
	int imin, imax;
	void matMult33x31(float* l, float* r, Vertex* col, int i){
		col->xn = ( ( (l[0]*r[0]) + (l[4]*r[1]) + (l[ 8]*r[2]) ) +1 )/2;
		col->yn = ( ( (l[1]*r[0]) + (l[5]*r[1]) + (l[ 9]*r[2]) ) +1 )/2;
		// col->zn = 0;
		col->zn = ( ( (l[2]*r[0]) + (l[6]*r[1]) + (l[10]*r[2]) ) +.0 )/1;
		if (col->zn < rmin) {
			rmin = col->zn;
			imin = i;
		}
		else if (col->zn > rmax) {
			rmax = col->zn;
			imax = i;
		}
	}

	// void mean(float* l, float* r, Vertex* col){
	// 	col->xn = ((((l[0]*r[0])+(l[4]*r[1])+(l[ 8]*r[2])+(l[12]*r[3]))/col->hn)+1)/2;
	// 	col->yn = ((((l[1]*r[0])+(l[5]*r[1])+(l[ 9]*r[2])+(l[13]*r[3]))/col->hn)+1)/2;
	// 	col->zn = ((((l[2]*r[0])+(l[6]*r[1])+(l[10]*r[2])+(l[14]*r[3]))/col->hn)+1)/2;
	// }
	void drawModel(int p){
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);

		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)(3*sizeof(float)));

		if(p==0) glColor4f(1,0.7,0.3,1);
		if(p==1) glColor3f(1,1,1);
		if(p==2){

			GLfloat matrix[16]; 
			glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
			float DOF[3];
			DOF[0] = matrix[  2 ]; // x
			DOF[1] = matrix[  6 ]; // y
			DOF[2] = matrix[ 10 ]; // z
			if(verbose) cout << "x: [" << matrix[0] << " " << matrix[4] << " " << matrix[8]  << "]" << endl;
			if(verbose) cout << "y: [" << matrix[1] << " " << matrix[5] << " " << matrix[9]  << "]" << endl;
			if(verbose) cout << "z: [" << matrix[2] << " " << matrix[6] << " " << matrix[10] << "]" << endl;
			if(verbose) cout << "__" << endl;
			glBindBuffer(GL_ARRAY_BUFFER, CBO);
			if(verbose) cout << "SIZE " << cn.size() << endl;
			for (int i=0; i<cn.size(); i++){
				// cn[i].xn = absolute(DOF[0] - model.vertexSet[i].xn);
				// cn[i].yn = absolute(DOF[1] - model.vertexSet[i].yn);
				// cn[i].zn = absolute(DOF[2] - model.vertexSet[i].zn);

				// cn[i].xn = DOF[0] - model.vertexSet[i].xn;
				// cn[i].yn = DOF[1] - model.vertexSet[i].yn;
				// cn[i].zn = DOF[2] - model.vertexSet[i].zn;

				// cn[i].xn = model.vertexSet[i].xn;
				// cn[i].yn = model.vertexSet[i].yn;
				// cn[i].zn = model.vertexSet[i].zn;
				
				// cn[i].xn = matrix[0];
				// cn[i].yn = matrix[5];
				// cn[i].zn = matrix[10];

				// cross(DOF, model.vertexSet[i], &cn[i]);
				
				float normal[4] = {model.vertexSet[i].xn, \
					model.vertexSet[i].yn, model.vertexSet[i].zn, \
					model.vertexSet[i].hn};

				matMult33x31(matrix, normal, &cn[i], i);
				// matMult44x41(matrix, normal, &cn[i]);
			}
			glBufferData(GL_ARRAY_BUFFER, cn.size()*sizeof(Vertex), &cn[0], GL_DYNAMIC_DRAW);
			glDisableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer ( 3, GL_FLOAT, sizeof(Vertex), (void*)(3*sizeof(float)) );

			if(verbose) cout << "red range: " << rmin*255 << " at " << imin << " to " << rmax*255 << " at " << imax << endl;
		}

		if(p==1) glEnable (GL_LIGHTING);
		glDrawElements(GL_TRIANGLES, model.faces.size()*3, GL_UNSIGNED_INT, 0);
		glDisable (GL_LIGHTING);

		if(p==2) glDisableClientState(GL_COLOR_ARRAY);

	}
	void keyboard(unsigned char c, int x, int y){
		if (c == 27 || c == 81 || c == 113) { // ESC or Q
			printf("\nExit\n");
			exit(0);
		}

		float mStep = 0.05;
		if (c == 'w') viewy += mStep;
		if (c == 'a') viewx -= mStep;
		if (c == 's') viewy -= mStep;
		if (c == 'd') viewx += mStep;
		if (c == 'z') viewz += mStep;
		if (c == 'x') viewz -= mStep;

		if (c == 't') manFur = !manFur;
		
		float zStep = 2;
		if (c == '=') {
			fovy-=zStep;
			LENGTH += LENGTH*SCALERATE;
		}
		if (c == '-'){
			fovy+=zStep;
			LENGTH -= LENGTH*SCALERATE;
		}
		
		if (c == 'r') loadTex();
		if (c == 'c') capture = !capture;

		if (c == 'f'){
			// glutReshapeWindow(500,500);
			// glutPositionWindow(900,300);
			fur = !fur;
		}
		view.refresh();
	}
	
	void mouse(int b, int s, int x, int y){
		if(s==GLUT_DOWN) {
			xAng = x;
			yAng = y;
			// trackball.rotate( 0,0,0,0 ) ;
			// view.refresh();
		}
	}
	
	void motion(int x, int y){
		// trackball.rotate((2.0 * xAng - dw) / float(dw),
		// (dh - 2.0 * yAng) / float(dh),
		// (2.0 * x - dw) / float(dw),
		// (dh - 2.0 * y) / float(dh));
		xAng = x ;
		yAng = y ;

		// view.refresh();
	}

	void drawAxes(){
		float depth = 0.3;
		float length = 10;
		glLineWidth(3);
		glColor3f(depth,0,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(length,0,0);
		glEnd();
		glColor3f(0,depth,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,length,0);
		glEnd();
		glColor3f(0,0,depth);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,0,length);
		glEnd();
	}

	void drawMesh(){
		glColor3f(.3,.3,.3);
		glLineWidth(1);
		float spacing=0.5;
		for(float x = -10; x <= 10; x+=spacing){
			glBegin(GL_LINE_LOOP);
			glVertex4f(x,0,10,1);
			glVertex4f(x,0,-10,1);
			glEnd();
		};

		for(float z = 10; z >= -10; z-=spacing){
			glBegin(GL_LINE_LOOP);
			glVertex4f(-10,0,z,1);
			glVertex4f(10,0,z,1);
			glEnd();
		};
	}

	void drawBoundingBox(){
		glLineWidth(1);
		glColor3f(1,1,1);
		glBegin(GL_LINE_LOOP);
			glVertex3f(res.xMax, res.yMax, res.zMax);
			glVertex3f(res.xMax, res.yMin, res.zMax);
			glVertex3f(res.xMin, res.yMin, res.zMax);
			glVertex3f(res.xMin, res.yMax, res.zMax);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(res.xMax, res.yMax, res.zMin);
			glVertex3f(res.xMax, res.yMin, res.zMin);
			glVertex3f(res.xMin, res.yMin, res.zMin);
			glVertex3f(res.xMin, res.yMax, res.zMin);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(res.xMax, res.yMax, res.zMin);
			glVertex3f(res.xMax, res.yMax, res.zMax);

			glVertex3f(res.xMax, res.yMin, res.zMin);
			glVertex3f(res.xMax, res.yMin, res.zMax);

			glVertex3f(res.xMin, res.yMin, res.zMin);
			glVertex3f(res.xMin, res.yMin, res.zMax);

			glVertex3f(res.xMin, res.yMax, res.zMin);
			glVertex3f(res.xMin, res.yMax, res.zMax);
		glEnd();

	}

	void drawLights(){
		glColor3f(1,1,1);
		glPointSize(20);
		glBegin(GL_POINTS);
			glVertex3f(lx,ly,lz);
		glEnd();
	}


	void snapDesire(vector<GLfloat> red, vector<GLfloat> green, vector<GLfloat> blue){

		if(verbose) cout << "writing.." << endl;
		FILE *f;
		unsigned char *img = NULL;
		int filesize = 54 + 3*dw*dh;  //w is your image width, h is image height, both int
		if( img ) free( img );
		img = (unsigned char *)malloc(3*dw*dh);
		memset(img,0,3*dw*dh);

		int x,y,r,g,b;
		for(int i=0; i<dw; i++)
		{
			for(int j=0; j<dh; j++)
			{
				x=i; y=(dh-1)-j;
				r = red[i+dw*j]*255;
				g = green[i+dw*j]*255;
				b = blue[i+dw*j]*255;
				r = 255-r;
				g = 255-g;
				b = 255-b;
				
				float rn = rand()/double(RAND_MAX);
				float vl = (r/255.0)*(r/255.0)*(r/255.0)*DENSITY;
				if(!(rand()%10) && rn<vl){ // is a pick
					// cout << rn << "/" << vl << endl;
					// r=255;
					g=0, b=0;
				}else{ // not a pick
				}
				
				// r = rand();
				img[(x+y*dw)*3+2] = (unsigned char)(r);
				img[(x+y*dw)*3+1] = (unsigned char)(g);
				img[(x+y*dw)*3+0] = (unsigned char)(b);
			}
		}

		unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
		unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
		unsigned char bmppad[3] = {0,0,0};

		bmpfileheader[ 2] = (unsigned char)(filesize    );
		bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
		bmpfileheader[ 4] = (unsigned char)(filesize>>16);
		bmpfileheader[ 5] = (unsigned char)(filesize>>24);

		bmpinfoheader[ 4] = (unsigned char)(       dw    );
		bmpinfoheader[ 5] = (unsigned char)(       dw>> 8);
		bmpinfoheader[ 6] = (unsigned char)(       dw>>16);
		bmpinfoheader[ 7] = (unsigned char)(       dw>>24);
		bmpinfoheader[ 8] = (unsigned char)(       dh    );
		bmpinfoheader[ 9] = (unsigned char)(       dh>> 8);
		bmpinfoheader[10] = (unsigned char)(       dh>>16);
		bmpinfoheader[11] = (unsigned char)(       dh>>24);

		f = fopen("desire.bmp","wb");
		fwrite(bmpfileheader,1,14,f);
		fwrite(bmpinfoheader,1,40,f);
		for(int i=0; i<dh; i++){
			fwrite(img+(dw*(dh-i-1)*3),3,dw,f);
			fwrite(bmppad,1,(4-(dw*3)%4)%4,f); // padding for multiple of 4
		}
		fclose(f);	
	}

	void snap(vector<GLfloat> red, vector<GLfloat> green, vector<GLfloat> blue){

		if(verbose) cout << "writing.." << endl;
		// cout << "r " << rand()%5 << endl;
		FILE *f;
		unsigned char *img = NULL;
		int filesize = 54 + 3*dw*dh;  //w is your image width, h is image height, both int
		if( img ) free( img );
		img = (unsigned char *)malloc(3*dw*dh);
		memset(img,0,3*dw*dh);

		int x,y,r,g,b;
		for(int i=0; i<dw; i++){
			for(int j=0; j<dh; j++){
				x=i; y=(dh-1)-j;
				r = red[i+dw*j]*255;
				g = green[i+dw*j]*255;
				b = blue[i+dw*j]*255;

				img[(x+y*dw)*3+2] = (unsigned char)(r);
				img[(x+y*dw)*3+1] = (unsigned char)(g);
				img[(x+y*dw)*3+0] = (unsigned char)(b);
			}
		}

		unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
		unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
		unsigned char bmppad[3] = {0,0,0};

		bmpfileheader[ 2] = (unsigned char)(filesize    );
		bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
		bmpfileheader[ 4] = (unsigned char)(filesize>>16);
		bmpfileheader[ 5] = (unsigned char)(filesize>>24);

		bmpinfoheader[ 4] = (unsigned char)(       dw    );
		bmpinfoheader[ 5] = (unsigned char)(       dw>> 8);
		bmpinfoheader[ 6] = (unsigned char)(       dw>>16);
		bmpinfoheader[ 7] = (unsigned char)(       dw>>24);
		bmpinfoheader[ 8] = (unsigned char)(       dh    );
		bmpinfoheader[ 9] = (unsigned char)(       dh>> 8);
		bmpinfoheader[10] = (unsigned char)(       dh>>16);
		bmpinfoheader[11] = (unsigned char)(       dh>>24);

		f = fopen("direction.bmp","wb");
		fwrite(bmpfileheader,1,14,f);
		fwrite(bmpinfoheader,1,40,f);
		for(int i=0; i<dh; i++){
			fwrite(img+(dw*(dh-i-1)*3),3,dw,f);
			fwrite(bmppad,1,(4-(dw*3)%4)%4,f); // padding for multiple of 4
		}
		fclose(f);	
	}

	vector<furMeta> readPrecursor(){
		vector<furMeta> meta;
		int i;
		FILE* locations = fopen("desire.bmp", "rb");
		FILE* directions = fopen("direction.bmp", "rb");

		if((locations == NULL) || (directions == NULL))
			throw "Argument Exception";

		unsigned char info[54];
		fread(info, sizeof(unsigned char), 54, locations); // read the 54-byte header
		fread(info, sizeof(unsigned char), 54, directions); // read the 54-byte header

		// extract image height and width from header
		int width = *(int*)&info[18];
		int height = *(int*)&info[22];

		if(verbose) cout << endl;
		if(verbose) cout << " Width: " << width << endl;
		if(verbose) cout << "Height: " << height << endl;

		int row_padded = (width*3 + 3) & (~3);
		unsigned char* data1 = new unsigned char[row_padded];
		unsigned char* data2 = new unsigned char[row_padded];
		unsigned char tmp;
		// ofstream myfile;
		// myfile.open ("image.txt");
		for(int i = 0; i < height; i++)
		{
			fread(data1, sizeof(unsigned char), row_padded,  locations);
			fread(data2, sizeof(unsigned char), row_padded, directions);
			for(int j = 0; j < width*3; j += 3)
			{
				// Convert (B, G, R) to (R, G, B)
				tmp = data1[j];
				data1[j] = data1[j+2];
				data1[j+2] = tmp;
				tmp = data2[j];
				data2[j] = data2[j+2];
				data2[j+2] = tmp;
				if((int)data1[j]>= (255-SPREAD) && (int)data1[j+1]==0 && (int)data1[j+2]==0){ // it's a selected poin0t
					furMeta f;
					f.x  = (((j/3)/((float)dw/2))-1)+0.000;
					f.y  = ((i/((float)dh/2))-1)+0.000;
					f.xn = f.x + ((data2[j]/127.0)-1)*(LENGTH/15.0);
					f.yn = f.y + ((data2[j+1]/127.0)-1)*(LENGTH/15.0);

					double m = -1/(f.yn-f.y)/(f.xn-f.x);

					double dx = f.xn - f.x;
					double dy = f.yn - f.y;

					double pdx = -dy/2;
					double pdy =  dx/2;

					f.ax = pdx + f.xn;
					f.ay = pdy + f.yn;

					f.bx = -pdx + f.xn;
					f.by = -pdy + f.yn;

					f.cx = -pdx + f.x;
					f.cy = -pdy + f.y;

					f.dx = pdx + f.x;
					f.dy = pdy + f.y;

					meta.push_back(f);
					// myfile << j/3 << " > " << f.y << endl;
					// myfile << "[ "<< (int)data2[j] << ", " << (int)data2[j+1]<< ", " << (int)data2[j+2] << " ]" << " at " << i << ", " << j/3 << endl;
				}
			}
		}
		// myfile.close();
		fclose(locations);
		fclose(directions);
		return meta;
	}

}