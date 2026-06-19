#include "Container.h"

Container::Container(double length, double width, double height)
    : length(length), width(width), height(height) {
}

double Container::getLengthContainer() const { return length; }
double Container::getWidthContainer()  const { return width; }
double Container::getHeightContainer() const { return height; }

double Container::getUtilizedVolume() const {
    double totalVolume = 0;
    for (const Object& obj : objectsPlaced) {
        totalVolume += obj.getVolume();
    }
    return totalVolume;
}

void Container::placeObject(const Object& obj) {
    objectsPlaced.push_back(obj);
}

void Container::clearContainer() {
    objectsPlaced.clear();
}

const std::vector<Object>& Container::getPlacedObjects() const {
    return objectsPlaced;
}

int Container::getPlacedCount() const {
    return static_cast<int>(objectsPlaced.size());
}

bool Container::canPlaceObject(const Object& obj) const {
    double x = obj.getPosition().getX();
    double y = obj.getPosition().getY();
    double z = obj.getPosition().getZ();
    double el = obj.getEffectiveLength();
    double ew = obj.getEffectiveWidth();
    double eh = obj.getEffectiveHeight();

    if (x < 0 || y < 0 || z < 0 ||
        x + el > length ||
        y + ew > width ||
        z + eh > height) {
        return false;
    }

    for (const Object& placed : objectsPlaced) {
        double px = placed.getPosition().getX();
        double py = placed.getPosition().getY();
        double pz = placed.getPosition().getZ();
        double pel = placed.getEffectiveLength();
        double pew = placed.getEffectiveWidth();
        double peh = placed.getEffectiveHeight();

        bool overlapX = (x < px + pel) && (x + el > px);
        bool overlapY = (y < py + pew) && (y + ew > py);
        bool overlapZ = (z < pz + peh) && (z + eh > pz);

        if (overlapX && overlapY && overlapZ) {
            return false;
        }
    }

    return true;
}