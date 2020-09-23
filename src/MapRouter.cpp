#include "MapRouter.h"
#include <cmath>
#include "XMLReader.h"
#include <algorithm>
#include <iostream>
#include "CSVReader.h"
#include <sstream>

const CMapRouter::TNodeID CMapRouter::InvalidNodeID = -1;

CMapRouter::CMapRouter(){

}

CMapRouter::~CMapRouter(){

}

double CMapRouter::HaversineDistance(double lat1, double lon1, double lat2, double lon2){
    //calculates distance between two latitude and longitude points in miles
    auto DegreesToRadians = [](double deg){return M_PI * (deg) / 180.0;};
	double LatRad1 = DegreesToRadians(lat1);
	double LatRad2 = DegreesToRadians(lat2);
	double LonRad1 = DegreesToRadians(lon1);
	double LonRad2 = DegreesToRadians(lon2);
	double DeltaLat = LatRad2 - LatRad1;
	double DeltaLon = LonRad2 - LonRad1;
	double DeltaLatSin = sin(DeltaLat/2);
	double DeltaLonSin = sin(DeltaLon/2);	
	double Computation = asin(sqrt(DeltaLatSin * DeltaLatSin + cos(LatRad1) * cos(LatRad2) * DeltaLonSin * DeltaLonSin));
	const double EarthRadiusMiles = 3959.88;
	
	return 2 * EarthRadiusMiles * Computation;
}        

double CMapRouter::CalculateBearing(double lat1, double lon1,double lat2, double lon2){
    //calculates bearing from 0 degrees to 180 degrees in whatever direction headed(North, South, East, NorthEast, etc.)
    auto DegreesToRadians = [](double deg){return M_PI * (deg) / 180.0;};
    auto RadiansToDegrees = [](double rad){return 180.0 * (rad) / M_PI;};
    double LatRad1 = DegreesToRadians(lat1);
	double LatRad2 = DegreesToRadians(lat2);
	double LonRad1 = DegreesToRadians(lon1);
	double LonRad2 = DegreesToRadians(lon2);
    double X = cos(LatRad2)*sin(LonRad2-LonRad1);
    double Y = cos(LatRad1)*sin(LatRad2)-sin(LatRad1)*cos(LatRad2)*cos(LonRad2-LonRad1);
    return RadiansToDegrees(atan2(X,Y));
}

bool CMapRouter::LoadMapAndRoutes(std::istream &osm, std::istream &stops, std::istream &routes){
    // Your code HERE
    CXMLReader Reader(osm);
    //takes in osm file
    SXMLEntity TempEntity;
    Reader.ReadEntity(TempEntity,true);
    //Reader reads the entity//true skips the Cdata
    if((TempEntity.DType != SXMLEntity::EType::StartElement)or(TempEntity.DNameData != "osm")){
        return false;
        //if there is no Start Element(there is one in the second line of the osm) or its a wrong type, then theres a problem so false is returned
    }
    while(!Reader.End()){
        //runs until the end of the file is reached
        Reader.ReadEntity(TempEntity,true);
        if(TempEntity.DType == SXMLEntity::EType::StartElement){
            //if we have a start element(should always pass since we check if we had one)
            if(TempEntity.DNameData == "node"){
                //checks if we have a node
                TNodeID TempID = std::stoul(TempEntity.AttributeValue("id"));
                //takes in id from the attribute//stoul converts string to unsigned long
                double TempLat = std::stod(TempEntity.AttributeValue("lat"));
                //takes in latitude from the attribute//stod converts string to double
                double TempLon = std::stod(TempEntity.AttributeValue("lon"));
                //takes in longitude from the attribute
                Node TempNode;
                //adds in information from temp ID, temp lat, and temp lon into a node
                TempNode.NodeID = TempID;
                TempNode.Location = std::make_pair(TempLat,TempLon);
                NodeTranslation[TempID] = Nodes.size();
                //assigned the id with a node which translates by connecting the node id to the node
                Nodes.push_back(TempNode);
                //shoves the node into a vector
                SortedIDs.push_back(TempID);
                //stores the node id into a vector to sort them after the loop
            }
            else if(TempEntity.DNameData == "way"){
                //ways are series of nodes connected by edges
                //as an analogy, ways are used to traverse from one vertices to another and placing edges along the way
                //if the start element is not a node, it checks if its a way next
                bool OneWay = false;
                //one way travel is set to false by default
                double SpeedLimit = 25.0;
                //the speed travelled is 25 mph by default
                std::vector< TNodeIndex > WayNodes;
                //node index is better because you only need to lookup once and ignores unreferenced nodes on the way
                while(!Reader.End()){
                //runs until the end of the file is reached
                Reader.ReadEntity(TempEntity,true);
                    //reads the entity
                    if(TempEntity.DType == SXMLEntity::EType::StartElement){
                        //checks for start element
                        if(TempEntity.DNameData == "nd"){
                            //checks if there's an nd. nd are labelled to represent ways in the osm
                            auto Lookup = NodeTranslation.find(std::stoul(TempEntity.AttributeValue("ref")));
                            if(NodeTranslation.end() != Lookup){
                                //checks if ref is in the node translation table
                                WayNodes.push_back(Lookup -> second);
                                //the lookup is shoved into waynodes
                            }
                        }
                        else if(TempEntity.DNameData == "tag"){
                            //checks if tag is reached
                            auto Key = TempEntity.AttributeValue("k");
                            auto Value = TempEntity.AttributeValue("v");
                            //processes the key and value
                            if(Key == "maxspeed"){
                                //if maxspeed is found is osm
                                SpeedLimit = std::stod(Value);
                                //changes the speed limit to the new value
                            }
                            else if(Key == "oneway"){
                                //if one way is found on osm
                                OneWay = Value == "yes";
                                //one way will be true if value is yes, otherwise, it will be false
                            }
                        }
                        }
                       if(TempEntity.DType == SXMLEntity::EType::StartElement){
                           if(TempEntity.DNameData == "way"){
                               break;
                               //breaks the while loop after WayNodes became filled up
                           }
                       }
                    }
                 //process the way here  
                 for(auto i = 0; i < WayNodes.size()-1; i++){
                     //traverses the list of way nodes
                     Edge TempEdge;
                     //creates an edge
                     TempEdge.OtherNode = WayNodes[i+1];
                     //TempNodes[WayNodes[i]]
                     TempEdge.Distance = HaversineDistance(Nodes[WayNodes[i]].Location.first,Nodes[WayNodes[i]].Location.second, Nodes[WayNodes[i+1]].Location.first,Nodes[WayNodes[i+1]].Location.second);
                     TempEdge.Time = TempEdge.Distance/SpeedLimit;
                     TempEdge.WalkTime = TempEdge.Distance/3.0;
                     Nodes[WayNodes[i]].Edges.push_back(TempEdge);
                     if(OneWay == false){
                         TempEdge.OtherNode = WayNodes[i];
                         Nodes[WayNodes[i+1]].Edges.push_back(TempEdge);
                     }
                 } 
            }
            }    
    }
    std::sort(SortedIDs.begin(), SortedIDs.end());
    //sorts the vector of node ids created by the while loop
    CCSVReader ReadStops(stops);
    std::vector<std::string> CSVRow;
    ReadStops.ReadRow(CSVRow);

    while(!ReadStops.End()) {
        ReadStops.ReadRow(CSVRow);
        TStopID StopID = std::stoul(CSVRow[0]);
        TNodeID StopNodeID = std::stoul(CSVRow[1]);
        StopTranslation[StopID] = StopNodeID;
        NodeStop[StopNodeID] = StopID;
    }

    CCSVReader ReadRoutes(routes);
    std::vector<std::string> CSVRoute;
    ReadRoutes.ReadRow(CSVRoute);

    while(!ReadRoutes.End()){
        ReadRoutes.ReadRow(CSVRoute);
        if(!SortedRouteList.empty()){
            ToRoute[std::stoul(CSVRoute[1])] = std::string(CSVRoute[0]);
                if(SortedRouteList[SortedRouteList.size()-1].Name == CSVRoute[0]){
                    SortedRouteList[SortedRouteList.size()-1].Stop.push_back(std::stoul(CSVRoute[1]));
        continue;
        }
        }
        BusRoute BS;
        BS.Name = CSVRoute[0];
        BS.Stop.push_back(std::stoul(CSVRoute[1]));
        SortedRouteList.push_back(BS);
        SortedName.push_back(BS.Name);
    }
    for(auto BusRoutes: SortedRouteList){
        for(int BusStop = 0; BusStop < BusRoutes.Stop.size()-1; BusStop++){
            Edge Buss;
            std::vector< TPathStep > path;
            auto FirstStop = StopTranslation.find(BusRoutes.Stop[BusStop]) -> second;
            auto SecondStop = StopTranslation.find(BusRoutes.Stop[BusStop+1]) -> second;
            auto NodeIndex = NodeTranslation.find(FirstStop) -> second;
            auto NodeIndex2 = NodeTranslation.find(SecondStop) -> second;
            Buss.OtherNode = NodeIndex2;
            double Fast = FindFastestBusPath(FirstStop, SecondStop, path, BusRoutes.Name);
            double TrueDistance = 0;
            for(int i = 0; i<path.size()-1; i++){
                auto NodeOne = NodeTranslation.find(path[i].second) -> second;
                auto NodeTwo = NodeTranslation.find(path[i+1].second) -> second;
                double DDistance = HaversineDistance(Nodes[NodeOne].Location.first,Nodes[NodeOne].Location.second, Nodes[NodeTwo].Location.first,Nodes[NodeTwo].Location.second);
                TrueDistance += DDistance;
            }
            Buss.Distance = TrueDistance;
            Buss.WalkTime = TrueDistance/3.0;
            Buss.Time = Fast+(1.0/120.0);
            Buss.Bus = true;
            Buss.NextStop = path;
            Nodes[NodeIndex].Edges.push_back(Buss);
        }
   }
    std::sort(SortedName.begin(),SortedName.end());
    return true;
}

size_t CMapRouter::NodeCount() const{
    // Your code HERE
    return SortedIDs.size();
    //tells us how many nodes we have
}

CMapRouter::TNodeID CMapRouter::GetSortedNodeIDByIndex(size_t index) const{
    // Your code HERE
    return SortedIDs[index];
    //searches for a node id you asked for from the sorted list of nodes
}

CMapRouter::TLocation CMapRouter::GetSortedNodeLocationByIndex(size_t index) const{
    //it finds the location of the sorted node through the index
    // Your code HERE
    if(index >= SortedIDs.size()){
        return std::make_pair(180.0,360.0);
        //if the index is invalid(out of bounds) then we return an invalid location
    }
    auto NodeID = SortedIDs[index];
    auto NodeIndex = NodeTranslation.find(NodeID) -> second;
    return Nodes[NodeIndex].Location;
    //returns the node location by searching an id through the sorted list, translating it to a node, and finally finding the location of the node
}

CMapRouter::TLocation CMapRouter::GetNodeLocationByID(TNodeID nodeid) const{
    //finds the location of the node through the id
    // Your code HERE
    auto NodeIndex = NodeTranslation.find(nodeid) -> second;
    return Nodes[NodeIndex].Location;
    //returns the location by translating the node id into a node and then returning a location of the node
}

CMapRouter::TNodeID CMapRouter::GetNodeIDByStopID(TStopID stopid) const{
    // Your code HERE
    return StopTranslation.find(stopid) -> second;
}

size_t CMapRouter::RouteCount() const{
    // Your code HERE
    return SortedName.size();
}

std::string CMapRouter::GetSortedRouteNameByIndex(size_t index) const{
    // Your code HERE
    return SortedName[index];
}

bool CMapRouter::GetRouteStopsByRouteName(const std::string &route, std::vector< TStopID > &stops){
    // Your code HERE
    for(auto i:SortedRouteList){
        if(i.Name == route){
            stops = i.Stop;
        }
    }
    return !stops.empty();
}

double CMapRouter::FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path){
    // Your code HERE
    std::vector <double> Dist;
    Dist.resize(Nodes.size(), std::numeric_limits<double>::max());
    auto NewSrc = NodeTranslation.find(src) -> second;
    std::vector <TNodeIndex> Prev;
    Prev.resize(Nodes.size(), -1);
    std::vector <TNodeIndex> heap;
    auto NewDest = NodeTranslation.find(dest) -> second;
    //auto NodeIndex = NodeTranslation.find(src) -> second;
    Dist[NewSrc] = 0;
    Prev[NewSrc] = NewSrc;
    auto CompareLambda = [&Dist](TNodeIndex idx1, TNodeIndex idx2){return Dist[idx1] < Dist[idx2];};
    heap.push_back(NewSrc);
    while(!heap.empty()){
        std::make_heap(heap.begin(), heap.end(), CompareLambda);
        auto Cur = heap.front();
        std::pop_heap (heap.begin(),heap.end(),CompareLambda); 
        heap.pop_back();

        for(auto edge : Nodes[Cur].Edges) {
            double AltDist = Dist[Cur] + edge.Distance;
            if(AltDist < Dist[edge.OtherNode]) {
                if(Dist[edge.OtherNode] == std::numeric_limits<double>::max()){
                    heap.push_back(edge.OtherNode);
                }
                Dist[edge.OtherNode] = AltDist;
                Prev[edge.OtherNode] = Cur;



            }
        }
    }
    if(Dist[NewDest]==std::numeric_limits<double>::max()){
        return std::numeric_limits<double>::max();
    }
    path.push_back(dest);
    TNodeIndex pathmake = NewDest;
    while(pathmake != NewSrc){
        auto NodeID = SortedIDs[Prev[pathmake]];
        auto NodeIndex = NodeTranslation.find(NodeID) -> second;
        path.push_back(Nodes[NodeIndex].NodeID);
        pathmake = Prev[pathmake];
    }
    std::reverse(path.begin(), path.end());
    //auto DestLoc = NodeTranslation.find(dest) -> second;

    
    return Dist[NewDest];
}
double CMapRouter::FindFastestBusPath(TNodeID src, TNodeID dest, std::vector< TPathStep > &path, std::string name){
        // Your code HERE
    std::vector <double> Time;
    Time.resize(Nodes.size(), std::numeric_limits<double>::max());
    auto NewSrc = NodeTranslation.find(src) -> second;
    std::vector <TNodeIndex> Prev;
    Prev.resize(Nodes.size(), -1);
    std::vector <TNodeIndex> heap;
    auto NewDest = NodeTranslation.find(dest) -> second;
    //auto NodeIndex = NodeTranslation.find(src) -> second;
    Time[NewSrc] = 0;
    Prev[NewSrc] = NewSrc;
    auto CompareLambda = [&Time](TNodeIndex idx1, TNodeIndex idx2){return Time[idx1] < Time[idx2];};
    heap.push_back(NewSrc);
    while(!heap.empty()){
        std::make_heap(heap.begin(), heap.end(), CompareLambda);
        auto Cur = heap.front();
        std::pop_heap (heap.begin(),heap.end(),CompareLambda); 
        heap.pop_back();

        for(auto edge : Nodes[Cur].Edges) {
            double AltTime = Time[Cur] + edge.Time;
            if(AltTime < Time[edge.OtherNode]) {
                if(Time[edge.OtherNode] == std::numeric_limits<double>::max()){
                    heap.push_back(edge.OtherNode);
                }
                Time[edge.OtherNode] = AltTime;
                Prev[edge.OtherNode] = Cur;



            }
        }
    }
    if(Time[NewDest]==std::numeric_limits<double>::max()){
        return std::numeric_limits<double>::max();
    }
    path.push_back(std::make_pair("Bus "+name,dest));
    TNodeIndex pathmake = NewDest;
    while(pathmake != NewSrc){
        auto NodeID = SortedIDs[Prev[pathmake]];
        auto NodeIndex = NodeTranslation.find(NodeID) -> second;
        path.push_back(std::make_pair("Bus "+name,Nodes[NodeIndex].NodeID));
        pathmake = Prev[pathmake];
    }
    return Time[NewDest];
}

double CMapRouter::FindFastestPath(TNodeID src, TNodeID dest, std::vector< TPathStep > &path){
    // Your code HERE
    std::vector <double> Time;
    Time.resize(Nodes.size(), std::numeric_limits<double>::max());
    auto NewSrc = NodeTranslation.find(src) -> second;
    std::vector <TNodeIndex> Prev;
    Prev.resize(Nodes.size(), -1);
    std::vector <TNodeIndex> heap;
    auto NewDest = NodeTranslation.find(dest) -> second;
    //auto NodeIndex = NodeTranslation.find(src) -> second;
    Time[NewSrc] = 0;
    Prev[NewSrc] = NewSrc;
    auto CompareLambda = [&Time](TNodeIndex idx1, TNodeIndex idx2){return Time[idx1] < Time[idx2];};
    heap.push_back(NewSrc);
    double AltTime;
    while(!heap.empty()){
        std::make_heap(heap.begin(), heap.end(), CompareLambda);
        auto Cur = heap.front();
        std::pop_heap (heap.begin(),heap.end(),CompareLambda); 
        heap.pop_back();

        for(auto edge : Nodes[Cur].Edges) {
            if(edge.Bus == true){
                AltTime = Time[Cur] + edge.Time;
                auto NodeID = SortedIDs[edge.OtherNode];
                auto NodeIndex = NodeTranslation.find(NodeID) -> second;
                Nodes[NodeIndex].BusEdge = true;
            }
            else{
                AltTime = Time[Cur] + edge.WalkTime;
            }
            if(AltTime < Time[edge.OtherNode]) {
                if(Time[edge.OtherNode] == std::numeric_limits<double>::max()){
                    heap.push_back(edge.OtherNode);
                }
                Time[edge.OtherNode] = AltTime;
                Prev[edge.OtherNode] = Cur;



            }
        }
    }
    if(Time[NewDest]==std::numeric_limits<double>::max()){
        return std::numeric_limits<double>::max();
    }
    auto Index = NodeTranslation.find(dest) -> second;
    if(Nodes[Index].BusEdge == true){
    auto ToStopID = NodeStop[dest];
    std::string AName = ToRoute.find(ToStopID) -> second;
    path.push_back(std::make_pair("Bus "+AName, dest));
    }
    else{
        path.push_back(std::make_pair("Walk", dest));
    }
    TNodeIndex pathmake = NewDest;
    while(pathmake != NewSrc){
        auto NodeID = SortedIDs[Prev[pathmake]];
        auto NodeIndex = NodeTranslation.find(NodeID) -> second;
    for(auto edge:Nodes[NodeIndex].Edges){
        if(!edge.NextStop.empty()){
            for(int i = 1; i< edge.NextStop.size()-1; i++){
                path.push_back(edge.NextStop[i]);
            }
        }
        }
        if(Nodes[NodeIndex].BusEdge == true){
        auto AStopID = NodeStop[Nodes[NodeIndex].NodeID];
        std::string TheName = ToRoute[AStopID];
        path.push_back(std::make_pair("Bus "+TheName, Nodes[NodeIndex].NodeID));
        }
        else{
        path.push_back(std::make_pair("Walk",Nodes[NodeIndex].NodeID));
        }
        pathmake = Prev[pathmake];
    }
    std::reverse(path.begin(), path.end());

    return Time[NewDest];
}




bool CMapRouter::GetPathDescription(const std::vector< TPathStep > &path, std::vector< std::string > &desc) const{
    // Your code HERE
    TLocation StartLocation;
    TLocation EndLocation;
    double Direction;
    std::string NSEW;
    std::string Description;
    std::string FullDescription;
    TStopID StopID;
    std::string RouteName;
    std::string BusStop;
    size_t FindRoute;
    if(path.empty()){
        return false;
    }
    for(int i = 0; i<path.size(); i++){
        StartLocation = GetNodeLocationByID(path[i].second);
        if(i == 0){
            Description = LatLongToDMS(StartLocation.first, StartLocation.second);
            FullDescription = "Start at "+ Description;
            desc.push_back(FullDescription);
        }
        if(i == path.size()-1){
            EndLocation = GetNodeLocationByID(path[i].second);
            Description = LatLongToDMS(EndLocation.first, EndLocation.second);
            FullDescription = "End at "+ Description;
            desc.push_back(FullDescription);
        }
        else if(path[i+1].first == "Walk"){
            EndLocation = GetNodeLocationByID(path[i+1].second);
            Direction = CalculateBearing(StartLocation.first, StartLocation.second, EndLocation.first, EndLocation.second);
            Description = LatLongToDMS(EndLocation.first, EndLocation.second);
            Direction += 0.5;
            if((int)Direction == 0){
                NSEW = "N ";
            }
            else if((int)Direction == 90){
                NSEW = "E ";
            }
            else if((int)Direction == 180){
                NSEW = "S ";
            }
            else if((int)Direction == -89){
                NSEW = "W ";
            }
            FullDescription = "Walk " + NSEW + "to " + Description;
            desc.push_back(FullDescription);
        }
        else if((path[i].first != "Walk") and (path[i+1].first != "Walk") and (i+1 == path.size() - 1)){
            std::string GetOff = std::to_string(NodeStop.find(path[i+1].second) -> second);
            FullDescription = "Take " + path[i].first + " and get off at stop " + GetOff;
            desc.push_back(FullDescription);
        }
        else if((path[i]).first != "Walk"){
            if(path[i+2].first == "Walk"){
            std::string GetOff = std::to_string(NodeStop.find(path[i+1].second) -> second);
            FullDescription = "Take " + path[i].first + " and get off at stop " + GetOff;
            desc.push_back(FullDescription);
            }
        }

    }
    return true;
}

std::string CMapRouter::LatLongToDMS(double lat, double lon){
    std::string Degrees;
    std::string GiveMS = "0.";
    std::string RightofStr;
    std::string Min;
    double DMinute;
    double DSecond;
    std::string SMinute;
    std::string LatDMS;
    std::string LonDMS;
    std::string LatLoc;
    std::string LonLoc;
    std::ostringstream streamLat;//https://thispointer.com/c-convert-double-to-string-and-manage-precision-scientific-notation/
    streamLat << lat;
    std::string Slat = streamLat.str();
    std::ostringstream streamLon;//https://thispointer.com/c-convert-double-to-string-and-manage-precision-scientific-notation/
    streamLon << lon;
    std::string Slon = streamLon.str();

    size_t Period = Slat.find('.');
    if(Period != Slat.npos){
        RightofStr = GiveMS + Slat.substr(Period+1);
        Degrees = Slat.substr(0, Period);
        DMinute = std::stod(RightofStr) * 60.0;
        SMinute = std::to_string(DMinute);
        RightofStr = GiveMS + SMinute.substr(SMinute.find('.') + 1);
        Min = SMinute.substr(0, SMinute.find('.'));
        DSecond = std::stod(RightofStr) * 60.0;
        SMinute = std::to_string(DSecond);
        LatDMS = Degrees+"d "+SMinute + "'" + SMinute;
        }
    else{
        LatDMS = Slat + "d 0' 0";
    }
    Period = Slon.find('.');
    if(Period != Slon.npos){
        RightofStr = GiveMS + Slon.substr(Period+1);
        Degrees = Slon.substr(0, Period);
        DMinute = std::stod(RightofStr) * 60.0;
        SMinute = std::to_string(DMinute);
        RightofStr = GiveMS + SMinute.substr(SMinute.find('.') + 1);
        Min = SMinute.substr(0, SMinute.find('.'));
        DSecond = std::stod(RightofStr) * 60.0;
        SMinute = std::to_string(DSecond);
        LonDMS = Degrees+"d "+SMinute + "'" + SMinute;
        }
    else{
        LonDMS = Slon + "d 0' 0";
    }
    if(lat >= 0){
        LatLoc = "N, ";
    }
    else{
        LatLoc = "S, ";
    }
    if(lon >= 0){
        LonLoc = "E";
    }
    else{
        LonLoc = "W";
    }
    std::string DMS = LatDMS+"\" "+ LatLoc + LonDMS + "\" "+ LonLoc;
    return DMS;
}
