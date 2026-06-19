#pragma once
#include "Container.h"
#include "Object.h"
#include <vector>

class SimulatedAnnealing
{
private:
    double initialTemperature; 
    double coolingRate;      
    double minTemperature;     
    int    iterationsPerTemp; 

    void generateNeighbor(std::vector<Object>& sequence) const;

public:
    SimulatedAnnealing(double initTemp, double coolRate,
        double minTemp, int iterPerTemp);

    std::vector<Object> solve(int instanceId, Container container, std::vector<Object> initialSequence, int runIndex = 1);
};