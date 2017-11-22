#ifndef POINT2D_H
#define POINT2D_H

class Point2d {
public:
	Point2d(double x = -1.0, double y = -1.0) : x(x), y(y) {};
	double x, y;
};

#endif