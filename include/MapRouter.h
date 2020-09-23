#ifndef MAPROUTER_H
#define MAPROUTER_H

#include <vector>
#include <istream>
#include <map>

class CMapRouter{
    //goal: code information in to create the ability to maintain Dijkstra algorithm
    public:
        using TNodeID = unsigned long;
        //Type TNodeID declared as type unsigned long type
        using TStopID = unsigned long;
        using TLocation = std::pair<double, double>;
        //the pair is latitude and longitude given by OSM
        using TPathStep = std::pair<std::string, TNodeID>;
        
        static const TNodeID InvalidNodeID;
    private:
        using TNodeIndex = int;
        //Type TNodeID declared as type integer
        struct Edge{
            //struct Edge tells you what the adjacent side is, how far away it is, and how long it takes to get there
            TNodeIndex OtherNode;
            //the adjacent side of the edge
            double Distance;
            //distance to the other node
            double Time;
            //how long it takes to get to other node
            double WalkTime;
            
            bool Bus = false;

            std::vector< TPathStep > NextStop;
        };
        struct Node{
            //information in this struct is from OSM
            TNodeID NodeID;
            //interface to map router that finds shortest path(ID number in OSM) from one Node ID to another
            TLocation Location;
            //a std::pair type containing latitude and longitude
            std::vector < Edge > Edges;
            //a list of edges that act as an adjacency list
            bool BusEdge = false;
        };
        struct BusRoute{
            std::vector < TStopID > Stop;
            
            std::string Name;

        };
        std::map < TStopID, std::string > ToRoute;
        std::vector < std::string > SortedName;
        std::vector < BusRoute > SortedRouteList;
        std::map < TNodeID, TStopID > NodeStop;
        std::map < TStopID, TNodeID > StopTranslation;
        std::vector < Node > Nodes;
        //takes in all nodes created into a vector.(node is basically a vertices)
        std::map < TNodeID, TNodeIndex > NodeTranslation;
        //translates from node id to node index
        std::vector < TNodeID > SortedIDs;
        //In GetSortedNodeIDByIndex so it can sort the NodeIDs and NodeCount gets the same node
        double FindFastestBusPath(TNodeID src, TNodeID dest, std::vector< TPathStep > &path, std::string name);
        static std::string LatLongToDMS(double lat, double lon);
    public:
        CMapRouter();
        ~CMapRouter();
        
        static double HaversineDistance(double lat1, double lon1, double lat2, double lon2);
        static double CalculateBearing(double lat1, double lon1,double lat2, double lon2);
        
        bool LoadMapAndRoutes(std::istream &osm, std::istream &stops, std::istream &routes);
        //takes in input stream, parse data, and loading data in
        size_t NodeCount() const;
        TNodeID GetSortedNodeIDByIndex(size_t index) const;
        TLocation GetSortedNodeLocationByIndex(size_t index) const;
        TLocation GetNodeLocationByID(TNodeID nodeid) const;
        TNodeID GetNodeIDByStopID(TStopID stopid) const;
        size_t RouteCount() const;
        std::string GetSortedRouteNameByIndex(size_t index) const;
        bool GetRouteStopsByRouteName(const std::string &route, std::vector< TStopID > &stops);
        
        double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path);
        double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TPathStep > &path);
        bool GetPathDescription(const std::vector< TPathStep > &path, std::vector< std::string > &desc) const;
};

#endif
