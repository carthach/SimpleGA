//
//  GeneticAlgorithm.cpp
//  simpleGA
//
//  Created by Cárthach Ó Nuanáin on 22/10/2015.
//
//

#include <GeneticAlgorithm.h>

vector<vector<int> > GeneticAlgorithm::splitPattern(const vector<int>& pattern)
{
    vector<vector<int> > patterns;
    
    for(int i=0; i<4; i++) {
        vector<int> p(pattern.begin()+(i*16),pattern.begin()+((i+1)*16));
        patterns.push_back(p);
    }
    
    return patterns;
}

//Our init function for constructors
void GeneticAlgorithm::init(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate)
{
    srand(time(NULL));
    
    this->targetString = targetString;
        
    if(targetString.size() == 64)
        targetStringSplit = splitPattern(targetString);
    
    for(int i=0; i<targetStringSplit.size(); i++) {
        targetSyncopation += getSyncopation(targetStringSplit[i]);
        targetDensity += getDensity(targetStringSplit[i]);
    }
    
    this->gaType = gaType;
    this->measure = measure;
    this->populationSize = populationSize;
    //        this->matingPoolSize = this->populationSize;
    this->matingPoolSize = this->populationSize*5;
    
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
GeneticAlgorithm::GeneticAlgorithm()
{
    init(vector<int>(0), NUMERICAL, HAMMING, 100, 0.1);
}

//For alphanumerical
GeneticAlgorithm::GeneticAlgorithm(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate)
{
    init(targetString, gaType, measure, populationSize, mutationRate);
}

//For numerical
GeneticAlgorithm::GeneticAlgorithm(vector<int> targetString, GATYPE gaType, MEASURE measure, int populationSize, float mutationRate, int min, int max)
{
    this->populationSize = populationSize;
    this->min = min;
    range = (max - min) + 1;
    init(targetString, gaType, measure, populationSize, mutationRate);
}

void GeneticAlgorithm::seedPopulation()
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

Member GeneticAlgorithm::crossover()
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

void GeneticAlgorithm::mutation(Member* child)
{
    if(rand()%100 < mutationRate*100.0) {
        int mutatePoint = rand()%geneLength;
        child->gene[mutatePoint] =  min + rand()%range;
    }
}

void GeneticAlgorithm::rouletteSelectionUnbounded()
{
    //Nature of Code - Inefficient
    for (int i=0; i<population.size(); i++) {
        int n = population[i].fitness * 100.0;
        
        for(int j=0; j<n; j++)
            matingPool.push_back(population[i]);
    }
}

//Here we do roulette wheel selection to determine the pool (
void GeneticAlgorithm::rouletteSelection()
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
    
    for(int i=0; i<matingPoolSize; i++)
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
void GeneticAlgorithm::truncateSelection(float percentage)
{
    int proportion = population.size() - (percentage * population.size());
    
    int j = proportion;
    
    for(int i=0; i<population.size(); i++) {
        matingPool.push_back(population[j]);
        
        j = (j == population.size()-1 ? proportion : j+1);
    }
    
}

void GeneticAlgorithm::elitism(float factor)
{
    int noOfEliteMembers = factor * population.size();
    
    for(int i=population.size()-1; i>(population.size()-noOfEliteMembers); --i)
        matingPool.push_back(population[i]);
}


//Selection, crossover and mutation
void GeneticAlgorithm::reproduction()
{
    //Clear the pool
    matingPool.clear();
    
    //Here we create the matingPool using a selection procedure
    //        truncateSelection(0.2);
    //        rouletteSelection();
    rouletteSelectionUnbounded();
    
    //Elitism
    //        elitism(0.09);
    
    for(int i=0; i<population.size();i++) {
        Member child = crossover();
        mutation(&child);
        population[i] = child;
    }
}

//Compute the fitness of the entire population
void GeneticAlgorithm::getFitness()
{
    for(int i=0; i<populationSize;i++) {
        population[i].fitness = getFitness(population[i].gene, targetString, &population[i].distance);
    }
}

//Sorting function
static bool sortByFitness(const Member &lhs, const Member &rhs) { return lhs.fitness < rhs.fitness; }
static bool sortByFitnessDescending(const Member &lhs, const Member &rhs) { return lhs.fitness > rhs.fitness; }

void GeneticAlgorithm::getDensityForPopulation()
{
    for(int i=0; i<population.size(); i++)
        population[i].density = getDensity(population[i].gene);
}

float GeneticAlgorithm::getDensity(const vector<int> & pattern)
{
    float density = std::count(pattern.begin(),pattern.end(), 1);
    return density /= pattern.size();
}

//Sorting function
static bool sortByDensity(const Member &lhs, const Member &rhs) { return lhs.density < rhs.density; }

//Here we do the evolution stage
Member GeneticAlgorithm::evolve()
{
    //The core
    reproduction();
    
    //Fitness and sorting
    getFitness();
    getDensityForPopulation();
    sort(population.begin(), population.end(),sortByFitness); //Sort by fitness
    
    //Returning
    Member bestIndividual = population.back();
    bestFitness = bestIndividual.fitness;
    return bestIndividual;
}

//Return the floating point distance
float GeneticAlgorithm::getFitness(const vector<int> &memberGene, const vector<int> &targetGene, int* distance)
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

float GeneticAlgorithm::getSyncopation(const vector<int> &pattern)
{
    float totalSyncopation = 0;
    
    float beatIntervalTicks = 4.0f;
    
    vector<float> beatsTicks;
    for(int i=0; i<6; i++)
        beatsTicks.push_back(i*beatIntervalTicks);
    
    int totalNumberOfOnsets = 0;
    
    for(int i=0; i<pattern.size(); i++) {
        //If it's a note
        if(pattern[i]) {
            int startTime = i;
            int beatIndex = 0;
            
            while(startTime < beatsTicks[beatIndex] || startTime >= beatsTicks[beatIndex+1])
                beatIndex++;
            
            //Get duration
            int duration = 0;
            
            int nextTime = i;
            while(pattern[nextTime] != 1 || nextTime < pattern.size()) {
                duration++;
                nextTime++;
            }
            
            float distanceToBeatOnLeft = abs(startTime - beatsTicks[beatIndex])/float(beatIntervalTicks);
            float distanceToBeatOnRight = abs(startTime - beatsTicks[beatIndex+1])/float(beatIntervalTicks);
            
            float distanceToNearestBeat = std::min(distanceToBeatOnLeft,distanceToBeatOnRight);
            
            //calculate the WNBD measure for this note, and add to total syncopation value for this bar
            if(distanceToNearestBeat == 0)
                totalSyncopation += 0;
            //or if this note is held on past the following beat, but ends on or before the later beat
            else if((startTime+duration > beatsTicks[beatIndex+1]) && (startTime+duration <= beatsTicks[beatIndex+2]))
                totalSyncopation += (float)2/distanceToNearestBeat;
            else
                totalSyncopation += (float)1/distanceToNearestBeat;
            
            totalNumberOfOnsets++;
        }
    }
    
    return totalSyncopation/totalNumberOfOnsets;
}

float GeneticAlgorithm::getCombinedFitness(vector<int> newPattern)
{
    vector<vector<int> > splitPatterns = splitPattern(newPattern);
    
    float similarity, syncopation, density;
    
    for(int i=0; i<splitPatterns.size(); i++) {
        similarity += getHammingFitness(splitPatterns[i], targetStringSplit[i], NULL);
        syncopation += (getSyncopation(splitPatterns[i]));
        density += (getDensity(splitPatterns[i]));
    }
    
    float fitness = 0.0f;
    
    return fitness;
}

//Hamming fitness as 1/d
float GeneticAlgorithm::getHammingFitness(const vector<int> &stringA, const vector<int> &stringB, int* distance_out)
{
    int distance = 0;
    for(int i=0;i<geneLength;i++)
        if(stringA[i] != stringB[i])
            distance += 1;
    
    int correct = geneLength - distance;
    *distance_out = distance;
    return (float)correct / (float)geneLength;
}

//Directed-swap fitness as 1/d
float GeneticAlgorithm::getSwapFitness(const vector<int> &stringA, const vector<int> &stringB)
{
    
    int distance = swapDistance.getDistance(stringA, stringB);
    return 1.0f / (float)(distance + 1);
}

//Just get a random member from the population
vector<int> GeneticAlgorithm::getRandomMember()
{
    int randomIndex = rand() % populationSize;
    return population[randomIndex].gene;
}