#include "GeneticAlgorithm.h"
#include "Decoder.h"
#include <algorithm>
#include <fstream>
#include <string>

GeneticAlgorithm::GeneticAlgorithm(int popSize, int gens, double mutRate)
    : populationSize(popSize), generations(gens), mutationRate(mutRate), rng(std::random_device{}()) 
{
}


void GeneticAlgorithm::calculateFitness(Individual& ind, Container container) {
    container.clearContainer();
    Decoder::decode(container, ind.sequence);
    ind.fitness = container.getUtilizedVolume();
}

Individual GeneticAlgorithm::crossover(const Individual& parent1,
    const Individual& parent2) const {
    int size = static_cast<int>(parent1.sequence.size());
    Individual child;

    child.sequence = parent1.sequence;

    int start = rng() % size;
    int end = rng() % size;
    if (start > end) std::swap(start, end);

    std::vector<int> inSegment(size, 0);
    for (int i = start; i <= end; ++i) {
        inSegment[i] = 1;
    }

    int childIndex = (end + 1) % size;
    int parentIndex = (end + 1) % size;

    for (int i = 0; i < size; ++i) {
        const Object& candidate = parent2.sequence[parentIndex];

        bool exists = false;
        for (int j = start; j <= end; ++j) {
            if (child.sequence[j].getId() == candidate.getId()) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            child.sequence[childIndex] = candidate;
            childIndex = (childIndex + 1) % size;
        }
        parentIndex = (parentIndex + 1) % size;
    }

    return child;
}


void GeneticAlgorithm::mutate(Individual& ind) const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    if (dist(rng) < mutationRate) {
        if (rng() % 2 == 0) {
            int idx1 = rng() % ind.sequence.size();
            int idx2 = rng() % ind.sequence.size();
            std::swap(ind.sequence[idx1], ind.sequence[idx2]);
        }
        else {
            int idx = rng() % ind.sequence.size();
            ind.sequence[idx].setRandomAllowedOrientation(rng()); 
        }
    }
}

std::vector<Object> GeneticAlgorithm::solve(int instanceId, Container container, std::vector<Object> initialBoxes, int runIndex) {
    std::vector<Individual> population(populationSize);

    for (int i = 0; i < populationSize; ++i) {
        population[i].sequence = initialBoxes;
        std::shuffle(population[i].sequence.begin(), population[i].sequence.end(), rng);
        for (Object& obj : population[i].sequence) {
            obj.setRandomAllowedOrientation(rng()); 
        }
        calculateFitness(population[i], container);
    }

    std::string csvName = "evolutie_GA_Inst" + std::to_string(instanceId) + "_Run" + std::to_string(runIndex) + ".csv";
    std::ofstream csvFile(csvName);
    if (csvFile.is_open()) csvFile << "Generatie;Cel_Mai_Bun_Volum\n";

    Individual globalBest = population[0];

    for (int gen = 0; gen < generations; ++gen) {
        std::sort(population.begin(), population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness > b.fitness;
            });

        if (population[0].fitness > globalBest.fitness) {
            globalBest = population[0];
        }

        if (csvFile.is_open()) {
            csvFile << gen << ";" << globalBest.fitness << "\n";
        }

        std::vector<Individual> nextGeneration;
        nextGeneration.reserve(populationSize);

        int eliteCount = std::max(1, populationSize / 10);
        for (int i = 0; i < eliteCount; ++i) {
            nextGeneration.push_back(population[i]);
        }

        while (static_cast<int>(nextGeneration.size()) < populationSize) {

            int p1_idx1 = rng() % populationSize;
            int p1_idx2;
            do { p1_idx2 = rng() % populationSize; } while (p1_idx2 == p1_idx1);
            Individual& parent1 = (population[p1_idx1].fitness > population[p1_idx2].fitness) ? population[p1_idx1] : population[p1_idx2];

            int p2_idx1 = rng() % populationSize;
            int p2_idx2;
            do { p2_idx2 = rng() % populationSize; } while (p2_idx2 == p2_idx1);
            Individual& parent2 = (population[p2_idx1].fitness > population[p2_idx2].fitness) ? population[p2_idx1] : population[p2_idx2];

            Individual child = crossover(parent1, parent2);
            mutate(child);
            calculateFitness(child, container);
            nextGeneration.push_back(child);
        }

        population = std::move(nextGeneration);
    }

    if (csvFile.is_open()) csvFile.close();

    return globalBest.sequence;
}