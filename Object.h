#pragma once
#include "Point3D.h"
#include <array>
#include <vector>

class Object
{
private:
    int id;
    double length;
    double width;
    double height;
    Point3D position;
    int orientation;

    bool rotL;
    bool rotW;
    bool rotH;

    std::array<bool, 6> computeAllowedOrientations() const;

public:
    Object(int id, double length, double width, double height,
        bool rotL = true, bool rotW = true, bool rotH = true);

    double getLength() const;
    double getWidth() const;
    double getHeight() const;
    double getVolume() const;
    int getId() const;
    int getOrientation() const;

    void setPosition(const Point3D& position);
    void setOrientation(int orientation);
    void setRandomAllowedOrientation(int randValue);

    std::vector<int> getAllowedOrientationsList() const;

    double getEffectiveLength() const;
    double getEffectiveWidth() const;
    double getEffectiveHeight() const;

    Point3D getPosition() const;
};  