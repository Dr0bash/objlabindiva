#include "Affind3D.h"


Point::Point(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Point2D::Point2D(float x, float y)
{
	this->x = x;
	this->y = y;
}

Polyhedron::Polyhedron(void)
{
	this->normalList = vector<Point>();
	this->pointList = vector<Point>();
	this->texturePoint = vector<Point2D>();
	this->polygons = vector<vector<tuple<int,int,int>>>();
}

Polyhedron::Polyhedron(vector<Point> points)
{
	this->pointList = vector<Point>();
	
	for (int i = 0; i < points.size(); i++)
	{
		this->pointList.push_back(points[i]);
	}



}