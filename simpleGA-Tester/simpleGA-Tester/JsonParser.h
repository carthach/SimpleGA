/**
    JsonParser.h

    Author:  matthias.leimeister
    Date: 04/2015

    (c) Copyright NATIVE INSTRUMENTS, Berlin, Germany
    ALL RIGHTS RESERVED

    Class for reading a JSON file containing drum patterns and parameters
    for pattern variation search from Maschine prototype.
*/

#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/math/special_functions/round.hpp>

#include "DrumEvent.h"

using namespace std;
using boost::property_tree::ptree;

class JsonParser
{
public:

    /// constructor
    JsonParser();

    /// desctructor
    ~JsonParser();

    /** 
     * function that parses a Json string and extracts patterns and sound selection
     * @param *in string containing the Json
     */
    void parseJson(const char *in);

    /** 
     * returns list of patterns in DrumEvents format
     * @return vector<DrumEvents> list of drum patterns
     */
    vector<DrumEvents> getPatternList() { return m_patternList; };
    
    /** 
     * returns vector specifying which sounds are selected
     * @return vector<bool> selection per sound
     */
    vector<bool> getSoundSelection() { return m_soundSelection; };
    
    /** 
     * transform a vector of drum patterns back to Json
     * @param patternVec list of drum patterns as double vector, first index is sound, second index is pattern
     * @return std::string Json string
     */
    std::string noteEventsToJson(const vector<vector<DrumEvents> > &patternVec);

private:

    /** 
     * helper function that reads note events from a Json subtree
     * @param pt subtree in boost::property_tree format
     * @return DrumEvents vector of DrumEvents
     */
    DrumEvents getNoteEventsFromSubtree(const ptree &pt);

    vector<DrumEvents> m_patternList;   ///< list of drum patterns
    vector<bool> m_soundSelection;      ///< selected sounds
};