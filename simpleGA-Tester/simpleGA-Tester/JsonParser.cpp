/*
    JsonParser.cpp

    Author:  matthias.leimeister
    Date: 04/2015

    (c) Copyright NATIVE INSTRUMENTS, Berlin, Germany
    ALL RIGHTS RESERVED

    Class for reading a JSON file containing drum patterns and parameters
    for pattern variation search from Maschine prototype.
*/

//#include "stdafx.h"
#include "JsonParser.h"
#include "boost/property_tree/xml_parser.hpp"


JsonParser::JsonParser()
{}


JsonParser::~JsonParser()
{}


void JsonParser::parseJson(const char *in)
{
    stringstream ss;
    ss << in;

	std::ofstream outFile("C:\\Users\\rich\\Desktop\\in.js");
    outFile << ss.str() << std::endl;
    outFile.close();

    ptree pt;
    read_json(ss, pt);

	//std::ofstream outFile2("C:\\Users\\rich\\Desktop\\in.xml");
	//write_xml(outFile2, pt);
	//outFile2.close();
    ptree selectIdx = pt.get_child("Select");
    m_soundSelection.clear();

    for (auto it = selectIdx.begin(); it != selectIdx.end(); ++it)
    {
        bool idx = it->second.get_value<bool>("");
        m_soundSelection.push_back(idx);
    }

    ptree patterns = pt.get_child("NoteClips");
    m_patternList.clear();

    // process each pattern individually
    for (auto it = patterns.begin(); it != patterns.end(); ++it)
    {
        DrumEvents nextPattern = getNoteEventsFromSubtree(it->second);
        m_patternList.push_back(nextPattern);
    }
}


DrumEvents JsonParser::getNoteEventsFromSubtree(const ptree &pt)
{
    DrumEvents eventVec;

    auto clipIdNode = pt.get_child("ClipId");
    int clipId = clipIdNode.get_value<int>();

    // only compute variations for kick(0), snare(1), and hihat(2-3), otherwise return empty pattern
    if (clipId > 3)
        return eventVec;

    auto eventsNode = pt.get_child("Events");
    for (const auto &event : eventsNode.get_child("")) // events are stored in an array, i.e. a tree with empty keys
    {
        DrumEvent newEvent;

        newEvent.N16th = static_cast<int>(round(event.second.get<float>("On") * 4.f));
        newEvent.length = static_cast<int>(event.second.get<float>("Length") * 4.f);
        newEvent.delta = event.second.get<float>("On") * 4.f - newEvent.N16th;
        newEvent.velocity = event.second.get<float>("Velocity") / 127.f;
        newEvent.pitch = event.second.get<int>("Pitch");
        newEvent.eventId = event.second.get<int>("EventId");
        newEvent.groupId = event.second.get<int>("GroupId");
        newEvent.soundId = event.second.get<int>("SoundId");
        newEvent.clipId = clipId;

        eventVec.push_back(newEvent);
    }

    return eventVec;
}


std::string JsonParser::noteEventsToJson(const vector<vector<DrumEvents> > &patternVec)
{
    ptree root;

    for (auto clipId = 0; clipId < patternVec.size(); ++clipId)
    {
        // each element of patternVec is a list of similar patterns
        auto patternList = patternVec[clipId];

        if (patternList.empty())
            continue;

        ptree simList;

        // each element of patternList is a pattern (vector<DrumEvents>)
        for (const auto &pattern : patternList)
        {
            ptree nextPattern;

            nextPattern.put<int>("ClipId", clipId);  // same ClipId as query pattern
            nextPattern.put<int>("ActiveEventId", pattern.size() - 1);

            ptree events;
            for (auto i = 0; i < pattern.size(); ++i)
            {
                ptree nextEvent;

                nextEvent.put<float>("On", pattern[i].N16th / 4.f);
                nextEvent.put<float>("Off", (pattern[i].N16th + 1.f) / 4.f);
                nextEvent.put<float>("Length", 0.25f);
                nextEvent.put<int>("Velocity", (int)(pattern[i].velocity*127.f));
                nextEvent.put<int>("Pitch", 60);
                nextEvent.put<int>("EventId", i);
                nextEvent.put<int>("GroupId", 0);
                nextEvent.put<int>("SoundId", clipId);

                events.push_back(make_pair("", nextEvent));
            }

            nextPattern.put_child("Events", events);

            simList.push_back(make_pair("", nextPattern));
        }

        string key;
        if (clipId == 0)
            key = string("kick");
        else if (clipId == 1)
            key = string("snare");
        else if (clipId == 2)
            key = string("chihat");
        else if (clipId == 3)
            key = string("ohihat");

        root.push_back(make_pair(key, simList));
    }

    ostringstream s;
    write_json(s, root);

    return s.str();
}
