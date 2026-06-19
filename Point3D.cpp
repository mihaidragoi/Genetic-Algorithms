#include "Point3D.h"

Point3D::Point3D() : x(-1), y(-1), z(-1)
{
}

Point3D::Point3D(double x, double y, double z) : x(x), y(y), z(z)
{
}

double Point3D::getX() const
{
	return x;
}

double Point3D::getY() const
{
	return y;
}

double Point3D::getZ() const
{
	return z;
}