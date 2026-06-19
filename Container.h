#pragma once
#include <vector>
#include "Object.h"

class Container
{
private:
    double length; 
    double width;  
    double height; 
    std::vector<Object> objectsPlaced;

public:
    Container(double length, double width, double height);

    double getLengthContainer() const;
    double getWidthContainer()  const;
    double getHeightContainer() const;

    double getUtilizedVolume() const;

    bool canPlaceObject(const Object& obj) const;

    void placeObject(const Object& obj);

    void clearContainer();

    const std::vector<Object>& getPlacedObjects() const;

    int getPlacedCount() const;
};