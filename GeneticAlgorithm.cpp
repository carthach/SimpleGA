//
//  geneticAlgorithm.cpp
//  drumGA
//
//  Created by Cárthach Ó Nuanáin on 10/10/2014.
//
//
#include <vector>
#include <algorithm>

#ifndef FLEXT_THREADS
#define FLEXT_THREADS
#endif

using namespace std;

struct Member {
    vector<int> gene;
    float fitness;
};

enum GATYPE {
    NUMERICAL,
    ALPHANUMERICAL
};

class GeneticAlgorithm {
    vector<Member> population;
    vector<Member> matingPool;
    
    int populationSize;
    int geneLength;
    float mutationRate = 0.1;
    
    int min, range;
    
public:
    float bestFitness;
    vector<int> targetString;
    GATYPE gaType;
    
    GeneticAlgorithm()
    {
        
    }
    
    void init(int populationSize, vector<int> targetString, GATYPE gaType)
    {
        this->populationSize = populationSize;
        this->targetString = targetString;
        this->gaType = gaType;
        geneLength = targetString.size();
        
        if(gaType == ALPHANUMERICAL) {
            min = 32;
            range = 94;
        }
        
        seedPopulation();
    }
    
    GeneticAlgorithm(int populationSize, vector<int> targetString, GATYPE gaType)
    {
        init(populationSize,targetString, gaType);
    }
    
    GeneticAlgorithm(int populationSize, vector<int> targetString, GATYPE gaType, int min, int max)
    {
        this->min = min;
        range = (max - min) + 1;
        init(populationSize,targetString, gaType);
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
                    gene.push_back(random()%range);
                }
            }
            
            Member member = {gene, 0.0};
            population.push_back(member);
        }
    }
    
    Member crossover()
    {
        int i1 = rand() % matingPool.size();
        int i2 = rand() % matingPool.size();
        int spos = rand() % geneLength;
        
        Member* parent1 = &matingPool[i1];
        Member* parent2 = &matingPool[i2];
        
        Member child;
        
        std::cout << "Here\n";
        
        for(int i=0; i<spos;i++)
            child.gene.push_back(parent1->gene[i]);
        for(int i=spos; i<geneLength;i++)
            child.gene.push_back(parent2->gene[i]);
        child.fitness = 0.0;
        
        std::cout << "There\n";        
    
        return child;
    }
    
    void mutation(Member* child)
    {
        if(rand()%100 < mutationRate*100.0) {
            int mutatePoint = rand()%geneLength;
            child->gene[mutatePoint] =  min + rand()%range;
        }
    }
    
    void reproduction()
    {
        matingPool.clear();
        selection();
        for(int i=0; i<population.size();i++) {
            Member child = crossover();
            mutation(&child);
            population[i] = child;
        }
    }
    
    //Here we do roulette wheel selection to determine the pool (
    void selection()
    {
        for (int i=0; i<population.size(); i++) {
            int n = population[i].fitness * 100.0;
            
            for(int j=0; j<n; j++)
                matingPool.push_back(population[i]);
        }
    }
    
    void getFitness()
    {
        for(int i=0; i<populationSize;i++)
            population[i].fitness = getFitness(population[i].gene, targetString);
    }
    
    static bool sortByFitness(const Member &lhs, const Member &rhs) { return lhs.fitness < rhs.fitness; }
    
    vector<int> evolve()
    {
        getFitness();
        sort(population.begin(), population.end(),sortByFitness); //Sort by fitness
        Member bestIndividual = population.back();
        if(bestIndividual.fitness != 1.0) {
            reproduction();
        }
        bestFitness = bestIndividual.fitness;
        return bestIndividual.gene;
    }
    
    float getFitness(vector<int> memberGene, vector<int> targetGene)
    {
        float fitness = 0.0;
        for(int i=0;i<geneLength;i++)
            if(memberGene[i] == targetGene[i])
                fitness += 1.0;
        fitness /= (float)geneLength;
        return fitness;
    }
    
    vector<int> getRandomMember()
    {
        int randomIndex = rand() % populationSize;
        return population[randomIndex].gene;
    }
};

