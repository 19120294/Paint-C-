// BasicShape.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "BasicShape.h"
#include <WindowsX.h>
Point::Point() {
	_x = _y = 0;
}


Point::Point(int x, int y) {
	_x = x;
	_y = y;
}

Point::Point(const Point& other) {
	_x = other._x;
	_y = other._y;
}

Point::~Point() {
}

Point& Point::operator=(const Point& p) {
	_x = p._x;
	_y = p._y;
	return *this;
}

int Point::getX()
{
	return _x;
}

int Point::getY()
{
	return _y;
}

shared_ptr<Point> Point::create(int x, int y)
{
	return make_shared<Point>(x, y);
}

Line::Line() {
}

Line::Line(int a, int b, int c, int d) {
	_a = Point(a, b);
	_b = Point(c, d);
}

Line::Line(Point a, Point b) {
	_a = a;
	_b = b;
}

Line::Line(const Line& l) {
	_a = l._a;
	_b = l._b;
}

Line::~Line()
{
}

Line& Line::operator=(const Line& l) {
	_a = l._a;
	_b = l._b;
	return *this;
}

int Line::type() {
	return 0;
}

shared_ptr<IShape> Line::create() {
	return make_shared<Line>();
}

void Line::setA(int x, int y) {
	_a = Point(x, y);
}

void Line::setB(int x, int y) {
	_b = Point(x, y);
}

Point Line::getA(){
	return _a;
}

Point Line::getB() {
	return _b;
}

BsRectangle::BsRectangle() {

}

BsRectangle::BsRectangle(int a, int b, int c, int d) {
	_a = Point(a, b);
	_b = Point(c, d);
}

BsRectangle::BsRectangle(Point a, Point b) {
	_a = a;
	_b = b;
}

BsRectangle::BsRectangle(const BsRectangle& other) {
	_a = other._a;
	_b = other._b;
}

BsRectangle& BsRectangle::operator=(const BsRectangle& other) {
	_a = other._a;
	_b = other._b;
	return *this;
}

BsRectangle::~BsRectangle()
{
}

int BsRectangle::type() {
	return 1;
}

shared_ptr<IShape> BsRectangle::create() {
	return make_shared<BsRectangle>();
}

void BsRectangle::setA(int x, int y) {
	_a = Point(x, y);
}

void BsRectangle::setB(int x, int y) {
	_b = Point(x, y);
}

Point BsRectangle::getA(){
	return _a;
}
Point BsRectangle::getB() {
	return _b;
}

Circle::Circle() {
}

Circle::Circle(int a, int b, int c, int d) {
	_a = Point(a, b);
	_b = Point(c, d);
}

Circle::Circle(Point a, Point b) {
	_a = a;
	_b = b;
}

Circle::Circle(const Circle& other) {
	_a = other._a;
	_b = other._b;
}

Circle& Circle::operator=(const Circle& other) {
	_a = other._a;
	_b = other._b;
	return *this;
}

Circle::~Circle()
{
}

int Circle::type(){
	return 2;
}

shared_ptr<IShape> Circle::create() {
	return make_shared<Circle>();
}

void Circle::setA(int x, int y) {
	_a = Point(x, y);
}

void Circle::setB(int x, int y) {
	_b = Point(x, y);
}

Point Circle::getA(){
	return _a;
}
Point Circle::getB() {
	return _b;
}

ShapeFactory::~ShapeFactory() {
	_prototypes.clear();
}
ShapeFactory* ShapeFactory::_instance = NULL;
ShapeFactory* ShapeFactory::getInstance() {
	if (_instance) {
		return _instance;
	}
	_instance = new ShapeFactory();
	return _instance;
}

shared_ptr<IShape> ShapeFactory::create(int type) {
	return _prototypes[type]->create();
}

ShapeFactory::ShapeFactory() {
	_prototypes.push_back(make_shared<Line>());
	_prototypes.push_back(make_shared<BsRectangle>());
	_prototypes.push_back(make_shared<Circle>());
}

IShape::IShape()
{
}

IShape::~IShape()
{
}

void IShape::setA(int, int)
{
}

void IShape::setB(int, int)
{
}

Point IShape::getA()
{
	return Point();
}

Point IShape::getB()
{
	return Point();
}

