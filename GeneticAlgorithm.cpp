//
//  GeneticAlgorithm.cpp
//  simpleGA
//
//  Created by Cárthach Ó Nuanáin on 22/10/2015.
//
//

#include "GeneticAlgorithm.h"

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
    
    targetPattern.gene = targetString;
    
    this->gaType = gaType;
    this->measure = measure;
    this->populationSize = populationSize;
    this->matingPoolSize = this->populationSize;
//    this->matingPoolSize = this->populationSize*5;    
    
    this->mutationRate = mutationRate;
    
    geneLength = targetString.size();
    
    if(gaType == ALPHANUMERICAL) {
        min = 32;
        range = 94;
    }
    
    if(geneLength > 0) {
        targetPattern.features = computeFeatures(targetPattern.gene, targetPattern.gene);
        targetPatternSplit = splitPattern(targetPattern.gene);
        
        bestFitness = 0.0f;
        seedPopulation();
    }
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
        
        Member member(gene);
        population.push_back(member);
        
//        population.push_back(targetPattern.gene);
    }
//    getFitness();
    getComplexFitness();
}

Member GeneticAlgorithm::crossover()
{
    int i1 = rand() % matingPool.size();
    int i2 = rand() % matingPool.size();
    int spos = rand() % geneLength;
    
    Member* parent1 = &matingPool[i1];
    Member* parent2 = &matingPool[i2];
    
    vector<int> gene;;
    
    for(int i=0; i<spos;i++)
        gene.push_back(parent1->gene[i]);
    for(int i=spos; i<geneLength;i++)
        gene.push_back(parent2->gene[i]);
    
    Member child(gene);
    
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
//    truncateSelection(0.2);
//    rouletteSelectionUnbounded();
    
    //New Way
    rouletteSelection();
    elitism(0.09);
    
    for(int i=0; i<population.size();i++) {
        Member child = crossover();
        mutation(&child);
        population[i] = child;
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

float GeneticAlgorithm::getNumberOfOnsets(const vector<int>& pattern)
{
    float numberOfOnsets = 0.0f;

    numberOfOnsets = std::count(pattern.begin(),pattern.end(), 1);
    
    return numberOfOnsets;
}


float GeneticAlgorithm::getDensity(const vector<int>& pattern)
{    
    return getNumberOfOnsets(pattern) / pattern.size();
}

//Here we do the evolution stage
Member GeneticAlgorithm::evolve()
{
    //The core
    reproduction();
    
    //Fitness and sorting
//    getFitness();
    
    getComplexFitness();
    
//    getDensityForPopulation();
    sort(population.begin(), population.end(),sortByFitness); //Sort by fitness
    
    //Returning
    Member bestIndividual = population.back();
    bestFitness = bestIndividual.fitness;
    return bestIndividual;
}

void GeneticAlgorithm::getFitness()
{
    for(int i=0; i<population.size(); i++)
        population[i].fitness = getFitness(population[i].gene, targetPattern.gene, &population[i].distance);
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
    
    for(int i=0; i<pattern.size(); i++)
    {
        //If it's a note
        if(pattern[i]) {
            int startTime = i;
            int beatIndex = 0;
            
            while(startTime < beatsTicks[beatIndex] || startTime >= beatsTicks[beatIndex+1])
                beatIndex++;
            
            //Get duration
            int duration = 1;
            int nextTime = startTime+1;
            while(pattern[nextTime] != 1 && nextTime < pattern.size()) {
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
    
    if(totalNumberOfOnsets > 0)
        return totalSyncopation/totalNumberOfOnsets;
    else
        return 0.0f;
}


vector<float> GeneticAlgorithm::computeFeatures(const vector<int> &pattern, const vector<int> &targetPattern)
{
    vector<vector<int> > splitPatterns = splitPattern(pattern);
    
    vector<float> features(3, 0.0f);
    
    for(int i=0; i<splitPatterns.size(); i++)
    {
        //If we're computing the target features then put in the same for both params
        if(&pattern != &targetPattern)
        {
            int distance;
            features[0] += getHammingDistance(splitPatterns[i], targetPatternSplit[i]);
        }
        if(getNumberOfOnsets(splitPatterns[i]))
            features[1] += (getSyncopation(splitPatterns[i])/getNumberOfOnsets(splitPatterns[i]));
    }
    
    features[0] /= 4.0f;
    features[1] /= 4.0f;
    
    features[2] = (getDensity(pattern));

    return features;
}

int GeneticAlgorithm::getHammingDistance(const vector<int> &stringA, const vector<int> &stringB)
{
    int distance = 0;
    for(int i=0;i<stringA.size();i++)
        if(stringA[i] != stringB[i])
            distance += 1;
    return distance;
}


//Hamming fitness as 1/d
float GeneticAlgorithm::getHammingFitness(const vector<int> &stringA, const vector<int> &stringB, int* distance_out)
{

    int distance = getHammingDistance(stringA, stringB);
    
    int correct = stringA.size() - distance;
    *distance_out = distance;
    
    float result = (float)correct / (float)stringA.size();
    
    return  result;
}

//Directed-swap fitness as 1/d
float GeneticAlgorithm::getSwapFitness(const vector<int> &stringA, const vector<int> &stringB)
{
    int distance = swapDistance.getDistance(stringA, stringB);
    return 1.0f / (float)(distance + 1);
}

//Compute the fitness of the entire population
void GeneticAlgorithm::getComplexFitness()
{
    for(int i=0; i<population.size();i++)
    {
        population[i].features = computeFeatures(population[i].gene, targetPattern.gene);
        
        float distance = vectorDistance(population[i].features.begin(), population[i].features.end(), targetPattern.features.begin());
        
        float fitness = 1.0 / (distance+1);
        
        population[i].fitness = fitness;
        
//        std::cout << "fitness: " << fitness << "\n";
    }
}

//Just get a random member from the population
vector<int> GeneticAlgorithm::getRandomMember()
{
    int randomIndex = rand() % populationSize;
    return population[randomIndex].gene;
}


template<class Iter_T, class Iter2_T>
double GeneticAlgorithm::vectorDistance(Iter_T first, Iter_T last, Iter2_T first2) {
    double ret = 0.0;
    while (first != last) {
        double dist = (*first++) - (*first2++);
        ret += dist * dist;
    }
    return ret > 0.0 ? sqrt(ret) : 0.0;
}
