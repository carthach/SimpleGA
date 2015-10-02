//
//  main.cpp
//  simpleGA-Tester
//
//  Created by Cárthach Ó Nuanáin on 01/10/2015.
//  Copyright © 2015 Cárthach Ó Nuanáin. All rights reserved.
//

#include <iostream>
#include <vector>
#include "GeneticAlgorithm.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    
    std::vector<int> kickPattern = {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
    std::vector<int> snarePattern = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
    std::vector<int> closeHatPattern = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    std::vector<int> openHatPattern = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    std::vector<int> fullPattern;
    
    fullPattern.insert(fullPattern.begin(), kickPattern.begin(), kickPattern.end());
    fullPattern.insert(fullPattern.begin(), snarePattern.begin(), snarePattern.end());
    fullPattern.insert(fullPattern.begin(), closeHatPattern.begin(), closeHatPattern.end());
    fullPattern.insert(fullPattern.begin(), openHatPattern.begin(), openHatPattern.end());
    
    for(int i=0; i<64; i++)
        fullPattern[i] = 1;
    
    GeneticAlgorithm ga(fullPattern, GeneticAlgorithm::GATYPE::NUMERICAL, GeneticAlgorithm::MEASURE::SWAP, 40, 0.1, 0, 1);
    
    int i = 0;
    std::vector<int> best;
    while(ga.bestFitness < 0.95) {
        best = ga.evolve();
        i++;
        std::cout << "bestFitness: " << ga.bestFitness << "\n";
    }
    std::cout << "Generations: " << i << "\n";
    
    return 0;
}
