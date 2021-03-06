#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "quadtree.h"

struct Point3D;
struct Node;

struct Point2D{
    float x;
    float y;
};

Point2D sub2DVect(Point2D p1, Point2D p2);

struct Triangle{
    Point2D p1;
    Point2D p2;
    Point2D p3;
};

Point3D sub3DVect(Point3D p1, Point3D p2);

float determinant(Point2D Vect1, Point2D Vect2);

void afficherTriangle(Triangle* triangle);

Point2D add2DVect(Point2D p1, Point2D p2);
Point2D scale2DVect(Point2D p1, float scalaire);
Point2D projection2D(Point3D p1);

Point3D add3DVect(Point3D p1, Point3D p2);

float norme2D(Point2D p1);
float norme3D(float x, float y, float z);

//Reste à calculer les intersections entre les quad et la camera
#endif