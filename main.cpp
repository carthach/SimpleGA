/*
 flext tutorial - advanced 2
 
 Copyright (c) 2002-2006 Thomas Grill (gr@grrrr.org)
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "license.txt," in this distribution.
 
 -------------------------------------------------------------------------
 
 This is an optimized version of the example "simple 3"
 
 It has the exact same functionality but methods are registered at class setup opposed to
 object setup (in the constructor) in "simple 3"
 
 The advantage of this lies in the fact that the message database needs only be constructed
 once for all objects, namely on creation of the first object of this class.
 All objects [simpleGA] will share the same database, saving memory.
 
 */
// include flext header
#include <flext.h>
#include <iostream>
#include <vector>
#include <string>
#include "geneticAlgorithm.cpp"


using namespace std;


// check for appropriate flext version
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 500)
#error You need at least flext version 0.5.0
#endif

class simpleGA:
public flext_base
{
    // flext header with a setup function called "setup"
    FLEXT_HEADER_S(simpleGA,flext_base,setup)
    
public:
    simpleGA(int argc, const t_atom* argv);
    
protected:
    void m_list(int argc, const t_atom* argv);
    void m_bang();
    void m_help();
    void m_string(int argc, const t_atom* argv);
    void m_targetFitness(float targetFitness);

    int populationSize;
    float mutationRate;
    float targetFitness;
    
private:
    // define the _static_ class setup function
    static void setup(t_classid c);
    
    FLEXT_CALLBACK_V(m_list)
    FLEXT_CALLBACK_V(m_string)
    FLEXT_CALLBACK_F(m_targetFitness)
    FLEXT_CALLBACK(m_bang)
    FLEXT_CALLBACK(m_help)
    
    GeneticAlgorithm geneticAlgorithm;
};

// instantiate the class (constructor takes no arguments)
FLEXT_NEW_V("simpleGA",simpleGA)

simpleGA::simpleGA(int argc, const t_atom* argv)
{
    // define inlets
    AddInAnything();  // add inlet of type anything (index 0)
    
    AddOutList();
    AddOutFloat();
    AddOutBang();
    
    populationSize = 100;
    mutationRate = 0.1;
    
    targetFitness = 1.0;

    if(argc == 2) {
        populationSize = GetAInt(argv[0]);
        mutationRate = GetAFloat(argv[1]);
    }
}

void simpleGA::setup(t_classid c)
{
    // register methods:
    // notice the naming FLEXT_CADD_METHOD* instead of FLEXT_ADD_METHOD*
    // there is also an additional parameter c pointing to the class definition
    
    FLEXT_CADDMETHOD(c, 0, m_list);
    FLEXT_CADDBANG(c, 0, m_bang);
    
    FLEXT_CADDMETHOD_(c,0,"targetFitness",m_targetFitness);
    FLEXT_CADDMETHOD_(c,0,"help",m_help);
    FLEXT_CADDMETHOD_(c,0,"string",m_string);
}

void simpleGA::m_list(int argc, const t_atom *argv)
{
    vector<int> newString;
    int min = 0, max =0;
    for(int i=0; i<argc; i++) {
        newString.push_back(GetAInt(argv[i]));
        if(newString.back()>max)
            max = newString.back();
        if(newString.back()<min)
            min = newString.back();
    }
    geneticAlgorithm = GeneticAlgorithm(100, newString, NUMERICAL, min, max);
    post("Number received");
}

void simpleGA::m_string(int argc, const t_atom *argv)
{
    std::vector<int> newString;
    for(int i=0; i<argc; i++) {
        const char* word = GetAString(argv[i]);
        char c;
        do {
            c = *word++;
            if(c != 0)
                newString.push_back(c);
        } while (c != '\0');
        if(i < argc-1)
            newString.push_back(' ');
    }
    geneticAlgorithm = GeneticAlgorithm(100, newString, ALPHANUMERICAL);
    
    post("Alphanumeric string received");
}

void simpleGA::m_bang()
{
    if(geneticAlgorithm.targetString.empty()) {
        post("No target string");
        return;
    }
    std::vector<int> bestMember = geneticAlgorithm.evolve();
    
    AtomList myList(bestMember.size());
    
    if(geneticAlgorithm.gaType == NUMERICAL) {
        for(int i = 0; i< bestMember.size(); ++i)
            SetInt(myList[i],bestMember[i]);
        ToOutList(0,myList);
    } else if (geneticAlgorithm.gaType == ALPHANUMERICAL)
    {
        string s;
        for(int i = 0; i< bestMember.size(); ++i)
            s.push_back(bestMember[i]);
        ToOutString(0, s.c_str());
    }
    
    if(geneticAlgorithm.bestFitness >= targetFitness)
        ToOutBang(2);
    
    ToOutFloat(1, geneticAlgorithm.bestFitness);
}

void simpleGA::m_targetFitness(float targetFitness)
{
    this->targetFitness = targetFitness;
    post("Target Fitness changed");
}

void simpleGA::m_help()
{
    post("%s - example for tagged messages",thisName());
}


