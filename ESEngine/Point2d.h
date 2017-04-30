#ifndef POINT2D_H
#define POINT2D_H

class Point2d {
public:
	Point2d(double x, double y) : x(x), y(y) {};
	double x, y;

private:
	Point2d() {};
};

#endif