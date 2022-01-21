#include <iostream>

// My Includes
#include "Controller.h"

using namespace std;

int main(int argc, char* argv[]){

	// Controller::setParams(100,0.1,1.5,0,1);
	// Controller::setFur("furs/leaf.png");
	// Controller::loadPly("models/fracttree.ply");
	
	// Controller::loadPly("models/galleon.ply");
	
	// Controller::loadPly("models/icosahedron.ply");
	
	// Controller::setParams(10,0.5,0.15,0,0);
	// Controller::setFur("furs/test.png");
	// Controller::loadPly("models/bunny.ply");
	
	// Controller::loadPly("models/beethoven.ply");
	
	// Controller::loadPly("models/apple.ply");
	
	// Controller::setParams(200,2.0,0.5,0,1);
	// Controller::setFur("furs/test.png");
	// Controller::loadPly("models/sphere.ply");
	
	// Controller::setParams(200,10.5,0.20,1,0);
	// Controller::loadPly("models/big_spider.ply");
	
	Controller::setParams(200,2.0,0.6,0,1);
	Controller::setFur("furs/test.png");
	Controller::loadPly("models/teddy.ply");

	// void setParams(int spread, float density, float length, int rotate, int useTexture);
	Controller::init();
	return 0;
}