#include "Model.h"
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void Model::loadPly(string s){
	string line;
	ifstream source(s);
	if (source.is_open()){
		float a=0,b=0,c=0,d=0;
		while(getline (source,line)){
			if(line.compare(0,14,"element vertex") == 0){
				stringstream ss(line.substr(14));
				ss >> vertexCount;
			}
			else if(line.compare(0,12,"element face") == 0){
				stringstream ss(line.substr(12));
				ss >> faceCount;
			}
			else if(line.compare(0,10,"end_header") == 0){
				break;
			}
		}

		for(int i=0; i<vertexCount; i++){
			getline (source,line);
			stringstream ss(line);
			ss >> a >> b >> c;

			Vertex v;
			v.x=a;
			v.y=b;
			v.z=c;
			vertexSet.push_back(v);
		}
		for(int i=0; i<faceCount;i++){
			getline (source,line);
			stringstream ss(line);
			ss >> d >> a >> b >> c;

			Face f;
			f.a=a;
			f.b=b;
			f.c=c;
			faces.push_back(f);
			calculateNormals(f);
		}
		superNormalizeNormals(); // account all normals
	}
	else{
		cout << "Can't open the file" << endl;
		exit(-1);
	}
	resize();
}

void Model::calculateNormals(Face f){
	float xn, yn, zn, va[3], vb[3], val;
	
	// get vectors
	va[0] = vertexSet[f.a].x - vertexSet[f.b].x;
	va[1] = vertexSet[f.a].y - vertexSet[f.b].y;
	va[2] = vertexSet[f.a].z - vertexSet[f.b].z;
 
	vb[0] = vertexSet[f.a].x - vertexSet[f.c].x;
	vb[1] = vertexSet[f.a].y - vertexSet[f.c].y;
	vb[2] = vertexSet[f.a].z - vertexSet[f.c].z;
 
	// cross product
	xn = va[1] * vb[2] - vb[1] * va[2];
	yn = vb[0] * va[2] - va[0] * vb[2];
	zn = va[0] * vb[1] - vb[0] * va[1];
 
	// normalize
	val = sqrt( xn*xn + yn*yn + zn*zn );
	xn /= val;
	yn /= val;
	zn /= val;
	
	// update
	updateNormal(f.a, xn, yn, zn);
	updateNormal(f.b, xn, yn, zn);
	updateNormal(f.c, xn, yn, zn);
}

void Model::updateNormal(int index, float xn, float yn, float zn){
	vertexSet[index].xn += xn;
	vertexSet[index].yn += yn;
	vertexSet[index].zn += zn;
	vertexSet[index].hn++;
}

void Model::superNormalizeNormals(){ // account all normals
	for(int i=0; i<vertexCount; i++){
		vertexSet[i].xn /= vertexSet[i].hn;
		vertexSet[i].yn /= vertexSet[i].hn;
		vertexSet[i].zn /= vertexSet[i].hn;
		vertexSet[i].hn = 1;
	}
}

void Model::analyse(){
	res.xMin = res.xMax = vertexSet[0].x;
	res.yMin = res.yMax = vertexSet[0].y;
	res.zMin = res.zMax = vertexSet[0].z;
	for(int i=0; i<vertexCount; i++){
		if(res.xMin > vertexSet[i].x) res.xMin = vertexSet[i].x;
		if(res.xMax < vertexSet[i].x) res.xMax = vertexSet[i].x;
		
		if(res.yMin > vertexSet[i].y) res.yMin = vertexSet[i].y;
		if(res.yMax < vertexSet[i].y) res.yMax = vertexSet[i].y;

		if(res.zMin > vertexSet[i].z) res.zMin = vertexSet[i].z;
		if(res.zMax < vertexSet[i].z) res.zMax = vertexSet[i].z;

		res.xCnt += vertexSet[i].x;
		res.yCnt += vertexSet[i].y;
		res.zCnt += vertexSet[i].z;
	}
	res.xCnt /= vertexCount;
	res.yCnt /= vertexCount;
	res.zCnt /= vertexCount;

}

void Model::resize(){
	analyse();
	float fullMax = max(max(res.xMax,res.yMax),res.zMax);

	for(int i=0; i<vertexCount; i++){
		vertexSet[i].x /= fullMax;
		vertexSet[i].y /= fullMax;
		vertexSet[i].z /= fullMax;
	}
	analyse();

}

Analysis Model::getAnalysis(){
	return res;
}