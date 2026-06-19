#pragma once
#include <vector>
#include "Container.h"
#include "Object.h"
#include "Point3D.h"

class Decoder
{
public:
    static void decode(Container& container, std::vector<Object>& sequence);
};