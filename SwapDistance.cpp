//
//  SwapDistance.cpp
//  simpleGA
//
//  Created by Cárthach Ó Nuanáin on 15/12/2014.
//
//

#include "SwapDistance.h"

void printPair(const std::pair<int, int> &thePair)
{
    std::cout << "i: " << thePair.first << " j: " << thePair.second << "\n";
}

float SwapDistance::getDistance(const std::vector<int> &patternOne, const std::vector<int> &patternTwo)
{
    int noOfVertices = 0;
    int distance = 0;
    
    vector<int> patternOneOnsetTimes;
    vector<int> patternTwoOnsetTimes;
    
    for(int i=0; i<patternOne.size();i++)
    {
        if(patternOne[i] == 1)
            patternOneOnsetTimes.push_back(i);
        if(patternTwo[i] == 1)
            patternTwoOnsetTimes.push_back(i);
    }
	
	//------
	//Temporary hack to handle case of string of all zeroes -> crashes dijkstras
    if(patternOneOnsetTimes.size() == 0 && patternTwoOnsetTimes.size() == 0)
        return 0.0f;
    
    if(patternOneOnsetTimes.size() == 0)
        return patternTwoOnsetTimes.size();

    
    if(patternTwoOnsetTimes.size() == 0)
        return patternOneOnsetTimes.size();
		
	//-----
    
    if(patternOneOnsetTimes.size() == patternTwoOnsetTimes.size())
    {
        for(int i=0; i<patternOneOnsetTimes.size(); i++)
            distance += abs(patternOneOnsetTimes[i] - patternTwoOnsetTimes[i]);
    }
    else
    {
        //S should be longer than T
        vector<int> * s = &patternOneOnsetTimes;
        vector<int> * t = &patternTwoOnsetTimes;
        
        int cs = patternOneOnsetTimes.size();
        int ct = patternTwoOnsetTimes.size();
        
        //s is shorter than t swap
        if(cs<ct) {
            s = &patternTwoOnsetTimes;
            t = &patternOneOnsetTimes;
            
            cs = patternTwoOnsetTimes.size();
            ct = patternOneOnsetTimes.size();
        }
        
        //Reset boost/dijkstra
        vertexMap.clear();
        g.clear();
        
        //Add vertices and create
        int i = 0; int j = 0;
        for(i=0; i<ct; i++) {
            for(j=i;j<=(i+(cs-ct)); j++) {
                std::pair<int, int> id(i, j);
                Vertex v = boost::add_vertex(g);
                
                vertexMap[id] = v;
            }
        }
        
        Vertex vStart = vertexMap[pair<int, int>(0, 0)];
        Vertex vEnd = vertexMap[pair<int, int>(i, j)];
        
        //Add edges
        for(int i=0; i<ct; i++)
        {
            for(int j=i;j<=(i+(cs-ct)); j++)
            {
                if(j+1 <= (i+(cs-ct))) {
                    Weight weight = (Weight)abs(t->at(i) - s->at(j+1));
                    
                    std::pair<int, int> thisVertexID(i, j);
                    std::pair<int, int> otherVertexID(i, j+1);
                    
                    Vertex thisVertex = vertexMap[thisVertexID];
                    Vertex otherVertex = vertexMap[otherVertexID];
                    
                    //                    printPair(thisVertexID);
                    //                    printPair(otherVertexID);
                    
                    add_edge(thisVertex, otherVertex, weight,  g);
                }
                if(j+1 <= (i+1+cs-ct) && (i+1 < ct)) {
                    Weight weight = (Weight)abs(t->at(i+1) - s->at(j+1));
                    
                    std::pair<int, int> thisVertexID(i, j);
                    std::pair<int, int> otherVertexID(i+1, j+1);
                    
                    Vertex thisVertex = vertexMap[thisVertexID];
                    Vertex otherVertex = vertexMap[otherVertexID];
                    
                    add_edge(thisVertex, otherVertex, weight,  g);
                }
                
            }
        }
        

        
        //        std::cout << "vertices(g) = ";
        //        typedef graph_traits<Graph>::vertex_iterator vertex_iter;
        //        std::pair<vertex_iter, vertex_iter> vp;
        //        for (vp = vertices(g); vp.first != vp.second; ++vp.first) {
        //            Vertex v = *vp.first;
        //            std::cout << index[v] <<  " ";
        //        }
        //        std::cout << std::endl;
        //
        //        // ...
        //        std::cout << "edges(g) = ";
        //        graph_traits<Graph>::edge_iterator ei, ei_end;
        //        for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        //            std::cout << "(" << index[source(*ei, g)]
        //            << "," << index[target(*ei, g)] << ") ";
        //        std::cout << std::endl;
        //        // ...
        
        // Create things for Dijkstra
        std::vector<Vertex> predecessors(boost::num_vertices(g)); // To store parents
        std::vector<Weight> distances(boost::num_vertices(g)); // To store distances
        
        IndexMap indexMap = boost::get(boost::vertex_index, g);
        PredecessorMap predecessorMap(&predecessors[0], indexMap);
        DistanceMap distanceMap(&distances[0], indexMap);
        
        // Compute shortest paths from v0 to all vertices, and store the output in predecessors and distances
        // boost::dijkstra_shortest_paths(g, v0, boost::predecessor_map(predecessorMap).distance_map(distanceMap));
        // This is exactly the same as the above line - it is the idea of "named parameters" - you can pass the
        // prdecessor map and the distance map in any order.
        boost::dijkstra_shortest_paths(g, vStart, boost::distance_map(distanceMap).predecessor_map(predecessorMap));
        
        distance =  distances[distances.size()-1] + abs(s->at(0)-t->at(0));
    }
    return (float)distance;
}

//// Driver program to test above functions
//int main()
//{
//    // create the graph given in above fugure
//    vector<int> s = {1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
//    vector<int> t = {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0};
//
//    vector<int> fandango = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
//    vector<int> solea = {0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1};
//    vector<int> buleria = {0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1};
//    vector<int> seguiriya = {1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0};
//    vector<int> guajira = {1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
//    vector<int> ancestral = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0};
//
//
//    std::cout << "Solea-Solea: " << getDirectedSwap(solea, solea) << "\n";
//    std::cout << "Solea-Buleria: " << getDirectedSwap(solea, buleria) << "\n";
//    std::cout << "Solea-Guajira: " << getDirectedSwap(solea, guajira) << "\n";
//    std::cout << "Solea-Seguiriya: " << getDirectedSwap(solea, seguiriya) << "\n";
//    std::cout << "Solua-Fandango: " << getDirectedSwap(solea, fandango) << "\n";
//    std::cout << "Solua-Fandango: " << getDirectedSwap(solea, ancestral) << "\n";
//
//    std::cout << "Solea-Solea: " << getDirectedSwap(buleria, buleria) << "\n";
//    std::cout << "Solea-Buleria: " << getDirectedSwap(buleria, guajira) << "\n";
//    std::cout << "Solea-Guajira: " << getDirectedSwap(buleria, seguiriya) << "\n";
//    std::cout << "Solea-Seguiriya: " << getDirectedSwap(buleria, fandango) << "\n";
//    std::cout << "Solua-Fandango: " << getDirectedSwap(buleria, ancestral) << "\n";
//}