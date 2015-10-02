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
#include <random>
#endif
#include <ctime>

using namespace std;

struct Member {
    vector<int> gene;
    float fitness;
    int distance;
};

class GeneticAlgorithm {
    vector<Member> population;
    vector<Member> matingPool;
    
    int min, range;
public:
    enum GATYPE {
        NUMERICAL,
        ALPHANUMERICAL
    };
    
    enum MEASURE {
        HAMMING,
        SWAP
    };
    
    int populationSize;
    int geneLength;
    float mutationRate = 0.1;
    
    SwapDistance swap;
    
    MEASURE measure;
    
    float bestFitness;
    vector<int> targetString;
    GATYPE gaType;
    
    //Our init function for constructors
    void init(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate)
    {
        srand(time(NULL));
        
        this->targetString = targetString;
        this->gaType = gaType;
        this->measure = measure;
        this->populationSize = populationSize;
        this->mutationRate = mutationRate;
        
        geneLength = targetString.size();
        
        if(gaType == ALPHANUMERICAL) {
            min = 32;
            range = 94;
        }
        
        bestFitness = 0.0f;
        seedPopulation();
    }
    
    //Our default constructor
    GeneticAlgorithm()
    {
        init(vector<int>(0), NUMERICAL, HAMMING, 100, 0.1);
    }
    
    //For alphanumerical
    GeneticAlgorithm(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate)
    {
        init(targetString, gaType, measure, populationSize, mutationRate);
    }
    
    //For numerical
    GeneticAlgorithm(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate, int min, int max)
    {
        this->populationSize = populationSize;
        this->min = min;
        range = (max - min) + 1;
        init(targetString, gaType, measure, populationSize, mutationRate);
    }
    
    void seedPopulation()
    {
        for(int i=0; i<populationSize; i++) {
            vector<int> gene;
            
            if(gaType == ALPHANUMERICAL) {
                for(int j=0;j<geneLength; j++) {
                    gene.push_back( ' ' + rand()%94);
                }
            } else {
                for(int j=0;j<geneLength; j++) {
                    gene.push_back(rand()%range);
                }
            }
            
            Member member = {gene, 0.0};
            population.push_back(member);
        }
        getFitness();
    }
    
    Member crossover()
    {
        int i1 = rand() % matingPool.size();
        int i2 = rand() % matingPool.size();
        int spos = rand() % geneLength;
        
        Member* parent1 = &matingPool[i1];
        Member* parent2 = &matingPool[i2];
        
        Member child;
        
        for(int i=0; i<spos;i++)
            child.gene.push_back(parent1->gene[i]);
        for(int i=spos; i<geneLength;i++)
            child.gene.push_back(parent2->gene[i]);
        child.fitness = 0.0;
        
        return child;
    }
    
    void mutation(Member* child)
    {
        if(rand()%100 < mutationRate*100.0) {
            int mutatePoint = rand()%geneLength;
            child->gene[mutatePoint] =  min + rand()%range;
        }
    }
    
    //Here we do roulette wheel selection to determine the pool (
    void rouletteSelection()
    {
        //Nature of Code - Inefficient
//        for (int i=0; i<population.size(); i++) {
//            int n = population[i].fitness * 100.0;
//            
//            for(int j=0; j<n; j++)
//                matingPool.push_back(population[i]);
//        }
        
        
        //Standard roulette wheel
        float totalFitness = 0;
        for(int i=0; i<population.size(); i++)
            totalFitness += population[i].fitness;
        
        for(int i=0; i<population.size(); i++)
        {

//            double rndNumber = rand() / (double) totalFitness;
            
            float rndNumber = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/totalFitness));
            
            double offset = 0.0;
            int pick = 0;
            
            for (int j = 0; j < population.size(); j++) {
                offset += population[j].fitness;
                if (rndNumber < offset) {
                    pick = j;
                    break;
                }
            }
            
            matingPool.push_back(population[pick]);
            
        }
    }
    
    //Here we select the top percentile and repeatedly add them to the pool
    void truncateSelection(float percentage)
    {
        int proportion = population.size() - (percentage * population.size());
        
        int j = proportion;
        
        for(int i=0; i<population.size(); i++) {
            matingPool.push_back(population[j]);
            
            j = (j == population.size()-1 ? proportion : j+1);
        }
        
    }
    
    void reproduction()
    {
        matingPool.clear();
        
        //Here we create the matingPool using a selection procedure
        truncateSelection(0.2);
//        rouletteSelection();
        
        for(int i=0; i<population.size();i++) {
            Member child = crossover();
            mutation(&child);
            population[i] = child;
        }
    }
    
    void getFitness()
    {
        for(int i=0; i<populationSize;i++) {
            
            population[i].fitness = getFitness(population[i].gene, targetString, &population[i].distance);
        }
    }
    
    static bool sortByFitness(const Member &lhs, const Member &rhs) { return lhs.fitness < rhs.fitness; }
    
    //Here we do the evolution stage
    vector<int> evolve()
    {
        reproduction();
        
        getFitness();
        sort(population.begin(), population.end(),sortByFitness); //Sort by fitness
        
        Member bestIndividual = population.back();
        bestFitness = bestIndividual.fitness;
        return bestIndividual.gene;
    }
    
    float getFitness(const vector<int> &memberGene, const vector<int> &targetGene, int* distance)
    {
        switch(measure) {
            case HAMMING:
                return getHammingFitness(memberGene, targetGene,distance);
                break;
            case SWAP:
                return getSwapFitness(memberGene, targetGene);
                break;
            default:
                break;
        }
    }
        
    float getHammingFitness(const vector<int> &stringA, const vector<int> &stringB, int* distance_out)
    {
        int distance = 0;
        for(int i=0;i<geneLength;i++)
            if(stringA[i] != stringB[i])
                distance += 1;
                
        int correct = geneLength - distance;
        *distance_out = distance;
        return (float)correct / (float)geneLength;
    }
    
    float getSwapFitness(const vector<int> &stringA, const vector<int> &stringB)
    {
        
        int distance = swap.getDistance(stringA, stringB);
        return 1.0f / float(distance + 1);
    }
    
    vector<int> getRandomMember()
    {
        int randomIndex = rand() % populationSize;
        return population[randomIndex].gene;
    }
};

