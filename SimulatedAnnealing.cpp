#include "SimulatedAnnealing.h"
#include "Decoder.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <string>

SimulatedAnnealing::SimulatedAnnealing(double initTemp, double coolRate,
    double minTemp, int iterPerTemp)
    : initialTemperature(initTemp), coolingRate(coolRate),
    minTemperature(minTemp), iterationsPerTemp(iterPerTemp)
{}

void SimulatedAnnealing::generateNeighbor(std::vector<Object>& sequence) const {
    if (sequence.size() < 2) return;

    int action = rand() % 2;

    if (action == 0) {
        int idx1 = rand() % sequence.size();
        int idx2 = rand() % sequence.size();
        std::swap(sequence[idx1], sequence[idx2]);
    }
    else {
        int idx = rand() % sequence.size();
        sequence[idx].setRandomAllowedOrientation(rand());
    }
}

std::vector<Object> SimulatedAnnealing::solve(int instanceId, Container container, std::vector<Object> initialSequence, int runIndex) {
    std::vector<Object> currentSequence = initialSequence;

    for (int i = static_cast<int>(currentSequence.size()) - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(currentSequence[i], currentSequence[j]);
    }
    for (Object& obj : currentSequence) obj.setRandomAllowedOrientation(rand());

    std::vector<Object> bestSequence = currentSequence;

    container.clearContainer();
    Decoder::decode(container, currentSequence);
    double currentFitness = container.getUtilizedVolume();
    double bestFitness = currentFitness;

    double temperature = initialTemperature;

    std::string csvName = "evolutie_SA_Inst" + std::to_string(instanceId) + "_Run" + std::to_string(runIndex) + ".csv";
    std::ofstream csvFile(csvName);
    if (csvFile.is_open()) {
        csvFile << "Pas_Racire;Temperatura;Volum_Curent;Cel_Mai_Bun_Volum\n";
    }

    int step = 0;

    while (temperature > minTemperature) {
        for (int i = 0; i < iterationsPerTemp; ++i) {
            std::vector<Object> neighborSequence = currentSequence;
            generateNeighbor(neighborSequence);

            container.clearContainer();
            Decoder::decode(container, neighborSequence);
            double neighborFitness = container.getUtilizedVolume();

            double deltaFitness = neighborFitness - currentFitness;

            if (deltaFitness > 0) {
                currentSequence = neighborSequence;
                currentFitness = neighborFitness;

                if (currentFitness > bestFitness) {
                    bestFitness = currentFitness;
                    bestSequence = currentSequence;
                }
            }
            else {
                double probability = std::exp(deltaFitness / temperature);
                double randomValue = static_cast<double>(rand()) / RAND_MAX;

                if (randomValue < probability) {
                    currentSequence = neighborSequence;
                    currentFitness = neighborFitness;
                }
            }
        }

        if (csvFile.is_open()) {
            csvFile << step << ";" << temperature << ";" << currentFitness << ";" << bestFitness << "\n";
        }

        step++;
        temperature *= coolingRate;
    }

    if (csvFile.is_open()) csvFile.close();

    return bestSequence;
}