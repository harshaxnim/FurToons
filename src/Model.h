#ifndef MODEL_H
#define MODEL_H

#include <vector>

using namespace std;

struct{
	float x=0;
	float y=0;
	float z=0;

	float xn=0;
	float yn=0;
	float zn=0;
	float hn=0;
} typedef Vertex;

struct{
	int a;
	int b;
	int c;
} typedef Face;

struct{
	float xMin;
	float yMin;
	float zMin;

	float xMax;
	float yMax;
	float zMax;

	float xCnt;
	float yCnt;
	float zCnt;
} typedef Analysis;


class Model{
public:
	int vertexCount, faceCount;
	vector<Face> faces;
	vector<Vertex> vertexSet;
	Analysis res;
	void loadPly(string s);
	void calculateNormals(Face f);
	void updateNormal(int index, float xn, float yn, float zn);
	void superNormalizeNormals(); // account all normals
	void resize();
	void analyse();
	Analysis getAnalysis();
};

#endif
