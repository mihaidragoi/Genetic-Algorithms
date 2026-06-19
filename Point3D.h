#pragma once

class Point3D
{
private:
    double x; 
    double y; 
    double z; 

public:
    Point3D();
    Point3D(double x, double y, double z);
    double getX() const;
    double getY() const;
    double getZ() const;
};