# 3D Bin Packing / Single Container Loading 📦

## 📖 Overview
This repository contains a C++ implementation aimed at solving the **3D Bin Packing / Single Container Loading** NP-hard problem. The goal is to optimally pack a set of different-sized boxes into a single container of fixed dimensions ($L \times W \times H$) while maximizing the utilized volume and respecting rotation constraints.

To achieve this, the project implements and compares two stochastic meta-heuristics: a **Genetic Algorithm (GA)** and **Simulated Annealing (SA)**.

## 🧠 Implemented Algorithms & Methodologies

### 1. Decoding Heuristic
Both algorithms use a common evaluation method based on the **Extreme Points** heuristic.
*   Boxes are placed using a **Bottom-Left-Back** strategy.
*   If a box cannot fit within the remaining space or violates constraints (e.g., overlaps or goes outside the container boundaries), it is simply skipped.

### 2. Genetic Algorithm (GA)
A population-based evolutionary approach:
*   **Representation:** An individual (chromosome) is defined as a permutation of all available boxes, paired with an orientation index representing up to 6 valid 3D rotations.
*   **Selection:** Binary Tournament is used to select parents based on their fitness scores.
*   **Crossover:** Order Crossover (OX) is implemented to maintain valid permutations and prevent duplicate or missing boxes in the offspring.
*   **Mutation:** Applied with a specific probability to either swap the positions of two boxes or change a single box's orientation.
*   **Elitism:** The best solutions are directly copied to the next generation without modification to ensure stable convergence.

### 3. Simulated Annealing (SA)
A single-solution local search inspired by the physical cooling of metals:
*   **Neighborhood Generation:** Generates a neighboring solution via a random Swap (changing the order of two boxes) or Reorientation (changing the rotation of a box).
*   **Acceptance Criterion:** Better solutions ($\Delta > 0$) are always accepted. Worse solutions ($\Delta \le 0$) are accepted with a probability of $P = e^{\frac{\Delta}{T}}$ to escape local optima.
*   **Cooling Schedule:** Employs a multiplicative cooling schedule using the formula $T = T \cdot \alpha$.

## 📊 Performance & Conclusions
Based on benchmarking tests using standard datasets (`thpack1` and `thpack2`), the algorithms exhibit the following behaviors:
*   **Genetic Algorithm:** Generally achieves higher container fill rates and better stability due to elitism, but requires more execution time due to population processing.
*   **Simulated Annealing:** Executes significantly faster and can occasionally find excellent solutions, though its convergence is "noisier" and highly dependent on the chosen cooling parameters.

## 📂 Repository Structure
*   `main.cpp`: Entry point of the application.
*   **Core Entities:** `Object.cpp` / `.h`, `Container.cpp` / `.h`, `Point3D.cpp` / `.h`.
*   **Algorithms:** `GeneticAlgorithm.cpp` / `.h`, `SimulatedAnnealing.cpp` / `.h`.
*   **Evaluation:** `Decoder.cpp` / `.h` (Handles the Extreme Points and Bottom-Left-Back logic).
*   **Outputs:** CSV reports (e.g., `raport_rulari_thpack1.csv`) detailing the execution metrics and optimal 3D placement coordinates.
