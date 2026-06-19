#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <algorithm>
#include <sstream>
#include "Container.h"
#include "Object.h"
#include "Decoder.h"
#include "SimulatedAnnealing.h"
#include "GeneticAlgorithm.h"

struct ProblemInstance {
    int id;
    Container container;
    std::vector<Object> boxes;
};

std::string formatCSV(double val) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << val;
    std::string s = oss.str();
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

bool loadAllInstances(const std::string& filename, std::vector<ProblemInstance>& instances) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    int numInstances;
    if (!(file >> numInstances)) return false;

    for (int k = 0; k < numInstances; ++k) {
        if (k >= 10) break;
        int instanceId, seed;
        if (!(file >> instanceId >> seed)) break;

        double cl, cw, ch;
        file >> cl >> cw >> ch;
        Container container(cl, cw, ch);

        int numTypes;
        file >> numTypes;
        std::vector<Object> boxes;
        int uniqueId = 1;

        for (int i = 0; i < numTypes; ++i) {
            int typeId, qty, flagL, flagW, flagH;
            double l, w, h;
            file >> typeId >> l >> flagL >> w >> flagW >> h >> flagH >> qty;
            for (int j = 0; j < qty; ++j) {
                boxes.push_back(Object(uniqueId, l, w, h, flagL == 1, flagW == 1, flagH == 1));
                uniqueId++;
            }
        }
        instances.push_back({ instanceId, container, boxes });
    }
    return true;
}

void writeSolutionDetails(std::ofstream& file, int instanceId, const std::string& alg, int run,
    Container container, std::vector<Object> sequence) {
    container.clearContainer();
    Decoder::decode(container, sequence);

    int order = 1;
    for (const Object& obj : container.getPlacedObjects()) {
        file << instanceId << ";" << alg << ";" << run << ";" << order++ << ";"
            << obj.getId() << ";" << obj.getLength() << ";" << obj.getWidth() << ";"
            << obj.getHeight() << ";" << obj.getOrientation() << ";"
            << formatCSV(obj.getPosition().getX()) << ";"
            << formatCSV(obj.getPosition().getY()) << ";"
            << formatCSV(obj.getPosition().getZ()) << ";"
            << obj.getEffectiveLength() << ";" << obj.getEffectiveWidth() << ";"
            << obj.getEffectiveHeight() << ";" << formatCSV(obj.getVolume()) << "\n";
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    std::vector<ProblemInstance> instances;
    std::string testFile = "thpack2.txt";
    std::string baseName = testFile.substr(0, testFile.find_last_of('.'));

    std::string runReportName = "raport_rulari_" + baseName + ".csv";
    std::string solutionName = "solutii_toate_rularile_" + baseName + ".csv"; 
    std::string summaryName = "sumar_instante_" + baseName + ".csv";

    std::ofstream runReport(runReportName);
    std::ofstream solutionFile(solutionName);
    std::ofstream summaryFile(summaryName);

    if (runReport.is_open())
        runReport << "Instanta;Algoritm;Run;Cutii_Plasate;Total_Cutii;Procentaj_Umplere;Timp_s\n";

    if (solutionFile.is_open())
        solutionFile << "Instanta;Algoritm;Run;Nr_Ordine;ID_Cutie;L_Original;W_Original;H_Original;"
        << "Orientare;Pos_X;Pos_Y;Pos_Z;Ef_L;Ef_W;Ef_H;Volum_Cutie\n";

    if (summaryFile.is_open())
        summaryFile << "Instanta;Algoritm;Nr_Rulari;Total_Cutii;Vol_Container;"
        << "Umplere_Min_pct;Umplere_Max_pct;Umplere_Medie_pct;Cutii_Plasate_Medie;Timp_Mediu_s\n";

    if (!loadAllInstances(testFile, instances)) return 1;

    const int NUM_RUNS = 5;

    SimulatedAnnealing sa(1000.0, 0.90, 5.0, 30);
    GeneticAlgorithm   ga(30, 50, 0.1);

    double totalPctSA = 0, totalTimeSA = 0;
    double totalPctGA = 0, totalTimeGA = 0;

    for (size_t i = 0; i < instances.size(); ++i) {
        Container container = instances[i].container;
        std::vector<Object> boxes = instances[i].boxes;
        int instId = instances[i].id;
        int total = static_cast<int>(boxes.size());
        double volContainer = container.getLengthContainer() * container.getWidthContainer() * container.getHeightContainer();

        std::vector<double> sa_pct(NUM_RUNS), sa_time(NUM_RUNS), sa_cnt(NUM_RUNS);
        std::vector<double> ga_pct(NUM_RUNS), ga_time(NUM_RUNS), ga_cnt(NUM_RUNS);

        std::cout << "\n>>> Instanta ID " << instId << " (" << total << " cutii)\n";

        for (int run = 0; run < NUM_RUNS; ++run) {
            
            auto t0 = std::chrono::high_resolution_clock::now();
            std::vector<Object> seqSA = sa.solve(instId, container, boxes, run + 1);
            double tSA = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();

            Container evalSA = container;
            evalSA.clearContainer();
            Decoder::decode(evalSA, seqSA);
            double pctSA = (evalSA.getUtilizedVolume() / volContainer) * 100.0;
            int cntSA = evalSA.getPlacedCount();
            sa_pct[run] = pctSA; sa_time[run] = tSA; sa_cnt[run] = cntSA;

            if (runReport.is_open())
                runReport << instId << ";SA;" << run + 1 << ";" << cntSA << ";" << total << ";" << formatCSV(pctSA) << ";" << formatCSV(tSA) << "\n";
            if (solutionFile.is_open())
                writeSolutionDetails(solutionFile, instId, "SA", run + 1, container, seqSA);

            std::cout << "  [Rularea " << run + 1 << "] SA: " << cntSA << " cutii | Umplere: " << std::fixed << std::setprecision(2) << pctSA << "% | " << tSA << "s\n";

            
            t0 = std::chrono::high_resolution_clock::now();
            std::vector<Object> seqGA = ga.solve(instId, container, boxes, run + 1);
            double tGA = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();

            Container evalGA = container;
            evalGA.clearContainer();
            Decoder::decode(evalGA, seqGA);
            double pctGA = (evalGA.getUtilizedVolume() / volContainer) * 100.0;
            int cntGA = evalGA.getPlacedCount();
            ga_pct[run] = pctGA; ga_time[run] = tGA; ga_cnt[run] = cntGA;

            if (runReport.is_open())
                runReport << instId << ";GA;" << run + 1 << ";" << cntGA << ";" << total << ";" << formatCSV(pctGA) << ";" << formatCSV(tGA) << "\n";
            if (solutionFile.is_open())
                writeSolutionDetails(solutionFile, instId, "GA", run + 1, container, seqGA);

            std::cout << "  [Rularea " << run + 1 << "] GA: " << cntGA << " cutii | Umplere: " << std::fixed << std::setprecision(2) << pctGA << "% | " << tGA << "s\n";
        }

        auto sacc = [&](const std::vector<double>& v) { return std::accumulate(v.begin(), v.end(), 0.0) / NUM_RUNS; };
        double sa_avg = sacc(sa_pct), ga_avg = sacc(ga_pct);
        totalPctSA += sa_avg; totalTimeSA += sacc(sa_time);
        totalPctGA += ga_avg; totalTimeGA += sacc(ga_time);

        if (summaryFile.is_open()) {
            summaryFile << instId << ";SA;" << NUM_RUNS << ";" << total << ";" << formatCSV(volContainer) << ";"
                << formatCSV(*std::min_element(sa_pct.begin(), sa_pct.end())) << ";"
                << formatCSV(*std::max_element(sa_pct.begin(), sa_pct.end())) << ";"
                << formatCSV(sa_avg) << ";" << formatCSV(sacc(sa_cnt)) << ";" << formatCSV(sacc(sa_time)) << "\n";
            summaryFile << instId << ";GA;" << NUM_RUNS << ";" << total << ";" << formatCSV(volContainer) << ";"
                << formatCSV(*std::min_element(ga_pct.begin(), ga_pct.end())) << ";"
                << formatCSV(*std::max_element(ga_pct.begin(), ga_pct.end())) << ";"
                << formatCSV(ga_avg) << ";" << formatCSV(sacc(ga_cnt)) << ";" << formatCSV(sacc(ga_time)) << "\n";
        }
    }

    std::cout << "\n=== RAPORT FINAL ===\nSA Umplere Medie: " << (totalPctSA / instances.size()) << "%\n";
    std::cout << "GA Umplere Medie: " << (totalPctGA / instances.size()) << "%\n";
    return 0;
}