#ifndef Affin3D_H
#define Affin3D_H

#include <vector>
#include <tuple>

using namespace std;

class Point
{
public:
	Point(float x, float y, float z);
	float x;
	float y;
	float z;
};

class Point2D
{
public:
	Point2D(float x, float y);
	float x;
	float y;
};

class Polyhedron
{
public:
	Polyhedron();
	Polyhedron(vector<Point> points);
	vector<Point> pointList;
	vector<Point> normalList;
	vector<Point2D> texturePoint;
	vector<vector<tuple<int,int,int>>> polygons;
};


#endif