//
//  main.cpp
//  simpleGA-Tester
//
//  Created by Cárthach Ó Nuanáin on 01/10/2015.
//  Copyright © 2015 Cárthach Ó Nuanáin. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "GeneticAlgorithm.h"

typedef std::map<string, vector<int> >  PatternMap;
typedef std::map<string, vector<float> >  FitnessValues;

FitnessValues parallelGA(PatternMap patternMap, GeneticAlgorithm::MEASURE measureType, int noOfGenerations)
{
    std::vector<GeneticAlgorithm> parallelGAs;
    
    for(auto const &pattern : patternMap)
    {
        GeneticAlgorithm ga(pattern.second, GeneticAlgorithm::GATYPE::NUMERICAL, measureType, 10, 0.1, 0, 1);
        parallelGAs.push_back(ga);
    }
    
    FitnessValues fitnessValues;
    
    for(int i=0; i<noOfGenerations; i++)
    {
        float bestFitness = 0.0;
        float averageFitness = 0.0;
        
        for(int i=0; i<parallelGAs.size(); i++)
        {
            parallelGAs[i].evolve();
            
            bestFitness += parallelGAs[i].bestFitness;
            averageFitness += parallelGAs[i].getAverageFitness();
        }
        
        bestFitness /= 4.0;
        averageFitness /= 4.0;
        
        fitnessValues["best"].push_back(bestFitness);
        fitnessValues["average"].push_back(averageFitness);
    }
    
    
    return fitnessValues;
}

FitnessValues linearGA(PatternMap patternMap, GeneticAlgorithm::MEASURE measureType, int noOfGenerations)
{
    std::vector<int> fullPattern;
    
    fullPattern.insert(fullPattern.end(), patternMap["kick"].begin(), patternMap["kick"].end());
    fullPattern.insert(fullPattern.end(), patternMap["snare"].begin(), patternMap["snare"].end());
    fullPattern.insert(fullPattern.end(), patternMap["ch"].begin(), patternMap["ch"].end());
    fullPattern.insert(fullPattern.end(), patternMap["oh"].begin(), patternMap["oh"].end());
    
    GeneticAlgorithm ga(fullPattern, GeneticAlgorithm::GATYPE::NUMERICAL, measureType, 200, 0.3, 0, 1);
    
    FitnessValues fitnessValues;
    
    for(int i=0; i<noOfGenerations; i++)
    {
        ga.evolve();
        
        fitnessValues["best"].push_back(ga.bestFitness);
        fitnessValues["average"].push_back(ga.getAverageFitness());
    }
    
    return fitnessValues;
}

void writeFitnessValues(FitnessValues fitnessValues, string path, string measure, string rep)
{
    ofstream bestFitnessFile;
    ofstream avgFitnessFile;
    
    bestFitnessFile.open(path + "/" + measure + "_" + rep + "_" + "best.txt");
    avgFitnessFile.open(path + "/" + measure + "_" + rep + "_" + "avg.txt");
    
    for(int i=0; i<fitnessValues["best"].size(); i++)
    {
        bestFitnessFile << fitnessValues["best"][i] << "\n";
        avgFitnessFile << fitnessValues["average"][i] << "\n";
    }
    
    bestFitnessFile.close();
    avgFitnessFile.close();
}

int main(int argc, const char * argv[])
{
    PatternMap patternMap;
    
    patternMap["kick"] = {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
    patternMap["snare"] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
    patternMap["ch"] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    patternMap["oh"] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    int noOfGenerations = 100;
    int noOfGenerationsSwap = 500;

    FitnessValues hammingLinearFitness = linearGA(patternMap, GeneticAlgorithm::MEASURE::HAMMING ,noOfGenerations);
    FitnessValues hammingParallelFitness = parallelGA(patternMap,GeneticAlgorithm::MEASURE::HAMMING, noOfGenerations);
    FitnessValues swapLinearFitness = linearGA(patternMap,GeneticAlgorithm::MEASURE::SWAP, noOfGenerationsSwap);
    FitnessValues swapParallelFitness = parallelGA(patternMap,GeneticAlgorithm::MEASURE::SWAP, noOfGenerationsSwap);
    
    string path = "/Users/carthach/Google Drive/Code/thesis_plots/fitness_best-avg";
    
    writeFitnessValues(hammingLinearFitness, path, "hamming", "linear");
    writeFitnessValues(hammingParallelFitness, path, "hamming", "parallel");
    writeFitnessValues(swapLinearFitness, path, "swap", "linear");
    writeFitnessValues(swapParallelFitness, path, "swap", "parallel");
    
    return 0;
}
