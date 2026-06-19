#include "Object.h"
#include <stdexcept>

Object::Object(int id, double length, double width, double height,
    bool rotL, bool rotW, bool rotH)
    : id(id), length(length), width(width), height(height),
    orientation(0), rotL(rotL), rotW(rotW), rotH(rotH)
{
}

std::array<bool, 6> Object::computeAllowedOrientations() const {

    std::array<bool, 6> allowed;

    allowed[0] = true;

    allowed[1] = rotW && rotH;

    allowed[2] = rotL && rotW;

    allowed[3] = rotL && rotW && rotH;

    allowed[4] = rotL && rotW && rotH;

    allowed[5] = rotL && rotH;

    return allowed;
}

void Object::setOrientation(int ori) {
    auto allowed = computeAllowedOrientations();
    int o = ori % 6;
    if (allowed[o]) {
        orientation = o;
    }
    else {
        orientation = 0;
    }
}

void Object::setRandomAllowedOrientation(int randValue) {
    auto allowed = computeAllowedOrientations();

    int permise[6];
    int nrPermise = 0;
    for (int i = 0; i < 6; ++i) {
        if (allowed[i]) permise[nrPermise++] = i;
    }

    orientation = permise[randValue % nrPermise];
}

double Object::getLength() const { return length; }
double Object::getWidth()  const { return width; }
double Object::getHeight() const { return height; }
double Object::getVolume() const { return length * width * height; }
int    Object::getId()     const { return id; }

void Object::setPosition(const Point3D& pos) { position = pos; }
Point3D Object::getPosition() const { return position; }

double Object::getEffectiveLength() const {
    switch (orientation) {
    case 0: return length;
    case 1: return length;
    case 2: return width;
    case 3: return width;
    case 4: return height;
    case 5: return height;
    default: return length;
    }
}

double Object::getEffectiveWidth() const {
    switch (orientation) {
    case 0: return width;
    case 1: return height;
    case 2: return length;
    case 3: return height;
    case 4: return length;
    case 5: return width;
    default: return width;
    }
}

double Object::getEffectiveHeight() const {
    switch (orientation) {
    case 0: return height;
    case 1: return width;
    case 2: return height;
    case 3: return length;
    case 4: return width;
    case 5: return length;
    default: return height;
    }
}

int Object::getOrientation() const {
    return orientation;
}


std::vector<int> Object::getAllowedOrientationsList() const {
    auto allowed = computeAllowedOrientations();
    std::vector<int> result;
    result.reserve(6);
    for (int i = 0; i < 6; ++i) {
        if (allowed[i]) result.push_back(i);
    }
    return result;
}