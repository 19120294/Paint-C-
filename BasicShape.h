#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <sstream>
#include <fstream>
#include <WindowsX.h>
#include <Windows.h>
using namespace std;

class Point {
public:
	Point();
	Point(int, int);
	Point(const Point&);
	~Point();
	Point& operator=(const Point&);
	int getX();
	int getY();
	shared_ptr<Point> create(int, int);
private:
	int _x;
	int _y;
};


class IShape {
public:
	IShape();
	~IShape();
	virtual int type() = 0;
	virtual shared_ptr<IShape> create() = 0;
	virtual void setA(int, int);
	virtual void setB(int, int);
	virtual Point getA();
	virtual Point getB();
};


class Line : public IShape {
public:
	Line();
	Line(int, int, int, int);
	Line(Point, Point);
	Line(const Line&);
	~Line();
	Line& operator= (const Line&);
	int type();
	shared_ptr<IShape> create();
	void setA(int, int);
	void setB(int, int);
	Point getA();
	Point getB();
private:
	Point _a;
	Point _b;
};

class BsRectangle : public IShape
{
public:
	BsRectangle();
	BsRectangle(int, int, int, int);
	BsRectangle(Point, Point);
	BsRectangle(const BsRectangle&);
	BsRectangle& operator=(const BsRectangle&);
	~BsRectangle();
	int type();
	shared_ptr<IShape> create();
	void setA(int, int);
	void setB(int, int);
	Point getA();
	Point getB();
private:
	Point _a;
	Point _b;
};

class Circle : public IShape {
public:
	Circle();
	Circle(int, int, int, int);
	Circle(Point, Point);
	Circle(const Circle&);
	Circle& operator=(const Circle&);
	~Circle();
	int type();
	shared_ptr<IShape> create();
	void setA(int, int);
	void setB(int, int);
	Point getA();
	Point getB();
private:
	Point _a;
	Point _b;
};

class ShapeFactory
{
public:
	~ShapeFactory();

	static ShapeFactory* getInstance();
	shared_ptr<IShape> create(int);
private:
	ShapeFactory();
	static ShapeFactory* _instance;
	vector<shared_ptr<IShape>> _prototypes;
};
