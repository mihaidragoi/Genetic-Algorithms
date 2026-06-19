#pragma once
#include "Container.h"
#include "Object.h"
#include <vector>
#include <random>

struct Individual {
    std::vector<Object> sequence;
    double fitness = 0.0;         
};

class GeneticAlgorithm
{
private:
    int    populationSize; 
    int    generations;    
    double mutationRate;   

    mutable std::mt19937 rng;

    void calculateFitness(Individual& ind, Container container);
    Individual crossover(const Individual& parent1,
        const Individual& parent2) const;
    void mutate(Individual& ind) const;

public:
    GeneticAlgorithm(int popSize, int gens, double mutRate);
    std::vector<Object> solve(int instanceId, Container container, std::vector<Object> initialBoxes, int runIndex = 1);
};