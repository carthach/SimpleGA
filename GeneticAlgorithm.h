//
//  geneticAlgorithm.cpp
//  drumGA
//
//  Created by Cárthach Ó Nuanáin on 10/10/2014.
//
//
#include <vector>
#include <algorithm>
#include "SwapDistance.h"
#if FLEXT_SYS != FLEXT_SYS_MAX //Need this hack for Max/MSP
//#include <random>
#endif
#include <ctime>

using namespace std;

struct Member {
    vector<int> gene;
    vector<float> features;
    
    float fitness = 0.0f;
    float density = 0.0f;
    int distance = 0;
    
    Member()
    {
        for(int i=0; i<3; i++)
            features.push_back(0.0f);
    }
    
    Member(const vector<int>& gene)
    {
        this->gene = gene;
        for(int i=0; i<3; i++)
            features.push_back(0.0f);
    }

};

class GeneticAlgorithm {
public:    
    vector<Member> population;
    vector<Member> matingPool;
    
    int min, range;

    enum GATYPE {
        NUMERICAL,
        ALPHANUMERICAL
    };
    
    enum MEASURE {
        HAMMING,
        SWAP
    };
    
    int populationSize, matingPoolSize;
    int geneLength;
    float mutationRate = 0.1;

    float dampSimilarity = 1.0f;
    float dampSyncopation = 1.0f;
    float dampDensity = 1.0f;

    vector<float> featureWeights;
    
    SwapDistance swapDistance;
    
    MEASURE measure;
    
    float bestFitness;

    Member targetPattern;
    vector<vector<int> > targetPatternSplit;
    
    GATYPE gaType;
    
    vector<vector<int> > splitPattern(const vector<int>& pattern);
    
    //Our init function for constructors
    void init(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate);
    
    //Our default constructor
    GeneticAlgorithm();
    
    //For alphanumerical
    GeneticAlgorithm(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate);
    
    //For numerical
    GeneticAlgorithm(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate, int min, int max);
    
    void seedPopulation();
    
    Member crossover();
    
    void mutation(Member* child);
    
    void rouletteSelectionUnbounded();
    
    //Here we do roulette wheel selection to determine the pool (
    void rouletteSelection();
    
    //Here we select the top percentile and repeatedly add them to the pool
    void truncateSelection(float percentage);
    
    void elitism(float factor);
    
    
    //Selection, crossover and mutation
    void reproduction();
    
    //Compute the fitness of the entire population
    void getFitness();

    //Sorting function
    static bool sortByFitness(const Member &lhs, const Member &rhs) { return lhs.fitness < rhs.fitness; }
    static bool sortByFitnessDescending(const Member &lhs, const Member &rhs) { return lhs.fitness > rhs.fitness; }

    //Sorting function
    static bool sortByDensity(const Member &lhs, const Member &rhs) { return lhs.density < rhs.density; }
    
    //Here we do the evolution stage
    Member evolve();
    
    void getDensityForPopulation();
    
    float getDensity(const vector<int> & pattern);
    
    //Return the floating point distance
    float getFitness(const vector<int> &memberGene, const vector<int> &targetGene, int* distance);
    
    float getSyncopation(const vector<int> &pattern);
    
    float getFitness(vector<int> newPattern);
    
    vector<float> computeFeatures(const vector<int> &pattern, const vector<int> &targetPattern);
    
    float getNumberOfOnsets(const vector<int>& pattern);
    
    //Hamming fitness as 1/d
    int getHammingDistance(const vector<int> &stringA, const vector<int> &stringB);
    float getHammingFitness(const vector<int> &stringA, const vector<int> &stringB, int* distance_out);
    
    //Directed-swap fitness as 1/d
    float getSwapFitness(const vector<int> &stringA, const vector<int> &stringB);
    
    void getComplexFitness();
    
    vector<int> getRandomMember();

    template<class Iter_T, class Iter2_T>
    double vectorDistance(Iter_T first, Iter_T last, Iter2_T first2);

};

