#pragma once
#ifndef _POINT_H
#define _POINT_H

class Point
{
public:
	Point();
	Point(int xval, int yval);
	void move(int dx, int dy);
	int get_x() const;
	int get_y() const;

private:
	int x, y;
};

#endif // _POINT_H