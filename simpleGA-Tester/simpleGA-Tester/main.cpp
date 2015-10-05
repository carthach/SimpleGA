//
//  main.cpp
//  simpleGA-Tester
//
//  Created by Cárthach Ó Nuanáin on 01/10/2015.
//  Copyright © 2015 Cárthach Ó Nuanáin. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "GeneticAlgorithm.h"
#include "JsonParser.h"

using namespace std;

DrumEvents binaryToPattern(const vector<int>& binaryPattern)
{
    DrumEvents drumEvents;
    
    for(int i=0; i<binaryPattern.size(); i++) {
        if(binaryPattern[i]) {
            DrumEvent drumEvent;
            drumEvent.N16th = i;
            drumEvent.velocity = 0.788f;
            drumEvent.eventId = i;
            drumEvent.groupId = 0;
            
            drumEvents.push_back(drumEvent);
        }
    }
    return drumEvents;
}

vector<int> patternToBinary(const DrumEvents& drumEvents)
{
    vector<int> binaryPattern(16, 0);
    
    for(int i=0; i<drumEvents.size(); i++) {
        binaryPattern[drumEvents[i].N16th] = 1;
    }
    
    return binaryPattern;
}

vector<int> joinPattern(const vector<vector<int> >& patterns)
{
    vector<int> result;
    
    for(int i=0; i<patterns.size(); i++)
        result.insert(result.begin()+(i*patterns[i].size()), patterns[i].begin(), patterns[i].end());
    
    return result;
}

vector<vector<int> > splitPattern(vector<int> pattern)
{
    vector<vector<int> > patterns;
    
    for(int i=0; i<4; i++) {
        vector<int> p(pattern.begin()+(i*16),pattern.begin()+((i+1)*16));
        patterns.push_back(p);
    }
    
    return patterns;
}

int call(const char *in, char *out, int lenOut)
{
    // insert code here...
    vector<vector<int> > inputPatterns;
    
    //=============================
    //Basic pattern
//    vector<int> kickPattern = {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
//    vector<int> snarePattern = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
//    vector<int> closeHatPattern = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
//    vector<int> openHatPattern = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//    
//    inputPatterns.push_back(kickPattern);
//    inputPatterns.push_back(snarePattern);
//    inputPatterns.push_back(closeHatPattern);
//    inputPatterns.push_back(openHatPattern);
    
    ifstream ifs("/Users/carthach/Desktop/in.json");
    string content( (istreambuf_iterator<char>(ifs) ),
                        (istreambuf_iterator<char>()    ) );
    
    JsonParser jp;
    jp.parseJson(content.c_str());
    
    DrumPattern drumPatternIn = jp.getPatternList();
    
    for(int i=0; i<4; i++)
        inputPatterns.push_back(patternToBinary(drumPatternIn[i]));
    
    vector<int> inputPattern = joinPattern(inputPatterns);
    
    //=============================
    //Do the GA
    
    GeneticAlgorithm ga(inputPattern, GeneticAlgorithm::GATYPE::NUMERICAL, GeneticAlgorithm::MEASURE::HAMMING, 100, 0.1, 0, 1);
    
    vector<vector<int> > gaPatterns;
    vector<float> fitnessRec;
    
    int i = 0;
    while(ga.bestFitness < 0.95) {
        //    while(i < 33) {
        vector<int> best = ga.evolve();
        gaPatterns.push_back(best);
        
        i++;
        fitnessRec.push_back(ga.bestFitness);
        cout << "bestFitness: " << ga.bestFitness << "\n";
    }
    
    int incr = gaPatterns.size() / 32.0;
    
    vector<vector<int> > prunedPatterns;
    
    cout << "===========================\n";
    
    for(int i=0; i<32; i++) {
        int index = i * incr;
        prunedPatterns.push_back(gaPatterns[index]);
        cout << index << "\n";
        
        std::cout << "Fitness: " << fitnessRec[index] << "\n";
    }
    
    prunedPatterns.push_back(inputPattern);
    
    cout << "Generations: " << i << "\n";
    
    //=============================
    //Serialise output patterns
    vector<DrumPattern> outputPatterns;
    
    vector<DrumEvents> kickPatterns, snarePatterns, closedHatPatterns, openHatPatterns;
    
    for(int i=0 ; i < prunedPatterns.size(); i++) {
        vector<vector<int> > patterns = splitPattern(prunedPatterns[i]);
        
        DrumEvents kickEvents = binaryToPattern(patterns[0]);
        DrumEvents snareEvents = binaryToPattern(patterns[1]);
        DrumEvents closedHatEvents = binaryToPattern(patterns[2]);
        DrumEvents openHatEvents = binaryToPattern(patterns[3]);
        
        kickPatterns.push_back(kickEvents);
        snarePatterns.push_back(snareEvents);
        
        closedHatPatterns.push_back(closedHatEvents);
        openHatPatterns.push_back(openHatEvents);
    }
    
    //Kicks
    outputPatterns.push_back(kickPatterns);
    outputPatterns.push_back(snarePatterns);
    outputPatterns.push_back(closedHatPatterns);
    outputPatterns.push_back(openHatPatterns);
    
    //Serialise
    string jsonPatternOut;
    jsonPatternOut = jp.noteEventsToJson(outputPatterns);
    
    //Write out file
//    ofstream outFile;
//    outFile.open("/Users/carthach/Desktop/newPattern.json");
//    outFile << jsonPatternOut;
//    outFile.close();
    return 0;
}


int main(int argc, const char * argv[]) {

    call("", "", 0);
    
    return 0;
}
