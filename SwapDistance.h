#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <boost/config.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;
using namespace std;

class SwapDistance {
public:
    float getDistance(const std::vector<int> &patternOne, const std::vector<int> &patternTwo);
    
private:
    typedef int Weight;
    typedef boost::property<boost::edge_weight_t, Weight> WeightProperty;
    
    typedef boost::adjacency_list < boost::listS, boost::vecS, boost::directedS, boost::no_property, WeightProperty> Graph;
    typedef boost::graph_traits <Graph >::vertex_descriptor Vertex;
    
    std::map<std::pair<int, int>, Vertex> vertexMap;
    
    // get the property map for vertex indices
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    
    typedef boost::iterator_property_map < Vertex*, IndexMap, Vertex, Vertex& > PredecessorMap;
    typedef boost::iterator_property_map < Weight*, IndexMap, Weight, Weight& > DistanceMap;
    
    Graph g;
};


