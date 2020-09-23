#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <exception>
#include <vector>
#include <cmath>
#include <iomanip>
#include "MapRouter.h"
#include "StringUtils.h"
#include "CSVWriter.h"

int main(int argc, char *argv[]){
    std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    int NumRoutes = 1000;
    std::string DataPath = "./data";
    std::string ResultsPath = "./results";
    bool Verbose = false;
    
    for(int ArgIndex = 1; ArgIndex < argc; ArgIndex++){
        auto ArgParts = StringUtils::Split(argv[ArgIndex],"=");
        bool ShowSyntax = false;
        if(ArgParts[0] == "--data"){
            if(2 <= ArgParts.size()){
                DataPath = ArgParts[1];
            }
            else{
                ShowSyntax = true;
            }
        }
        else if(ArgParts[0] == "--results"){
            if(2 <= ArgParts.size()){
                ResultsPath = ArgParts[1];
            }
            else{
                ShowSyntax = true;
            }
        }
        else if((ArgParts[0] == "--verbose") and (ArgParts.size() == 1)){
            Verbose = true;   
        }
        else{
            try{
                NumRoutes = std::stoul(argv[ArgIndex]);
            }
            catch(std::exception &Except){
                ShowSyntax = true;
            }
        }
        if(ShowSyntax){
            std::cerr<<"Syntax Error: findroute [--data=path | --resutls=path | --verbose] [numpoints]"<<std::endl;
            return EXIT_FAILURE;
        }
    }
    if(DataPath.back() != '/'){
        DataPath += "/";
    }
    if(ResultsPath.back() != '/'){
        ResultsPath += "/";
    }
    std::ofstream OutputBrief(ResultsPath + "out.txt");
    std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    std::cerr<<"Loading"<<std::endl;
    std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    auto LoadStart = std::chrono::steady_clock::now();
    CMapRouter MapRouter;

    std::ifstream OSMStream(DataPath + "davis.osm");
    std::ifstream BusStopStream(DataPath + "stops.csv");
    std::ifstream BusRoutesStream(DataPath + "routes.csv");
    std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;

    if(not MapRouter.LoadMapAndRoutes(OSMStream, BusStopStream, BusRoutesStream)){
        std::cerr<<"Failed to load map and routes."<<std::endl;
        std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
        return EXIT_FAILURE;
    }
    
    auto LoadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-LoadStart);

    std::vector< std::tuple< CMapRouter::TNodeID, CMapRouter::TNodeID > > RandomSourceDest;
    std::vector< std::vector< CMapRouter::TNodeID > > ShortestPaths;
    std::vector< CMapRouter::TNodeID > TempShortestPath;
    std::vector< double > ShortestDistance;
    std::vector< std::vector< CMapRouter::TPathStep > > FastestPaths;
    std::vector< CMapRouter::TPathStep > TempFastestPath;
    std::vector< double > FastestTime;

    std::cerr<<"Loaded"<<std::endl;
    std::cerr<<"Generating End Points"<<std::endl;
    std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    srand(1234);
    for(int Index = 0; Index < NumRoutes; Index++){
        std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
        auto SourceIndex = rand() % MapRouter.RouteCount();
        auto DestIndex = rand() % MapRouter.RouteCount();
        while(SourceIndex == DestIndex){
            DestIndex = rand() % MapRouter.RouteCount();
        }
        RandomSourceDest.push_back(std::make_tuple(SourceIndex, DestIndex));
    }
    ShortestPaths.resize(NumRoutes);
    ShortestDistance.resize(NumRoutes);
    FastestPaths.resize(NumRoutes);
    FastestTime.resize(NumRoutes);
    
    std::cerr<<"Finding Paths"<<std::endl;    
    auto ProcessingStart = std::chrono::steady_clock::now();
    std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    for(int Index = 0; Index < NumRoutes; Index++){
        std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
        auto CurSourceDest = RandomSourceDest[Index];
        auto SourceID = MapRouter.GetSortedNodeIDByIndex(std::get<0>(CurSourceDest));
        auto DestID = MapRouter.GetSortedNodeIDByIndex(std::get<1>(CurSourceDest));
        std::vector< CMapRouter::TNodeID > &ShortestPath = Verbose ? ShortestPaths[Index] : TempShortestPath;
        std::vector< CMapRouter::TPathStep > &FastestPath = Verbose ? FastestPaths[Index] : TempFastestPath;
        ShortestDistance[Index] = MapRouter.FindShortestPath(SourceID, DestID, ShortestPath);
        FastestTime[Index] = MapRouter.FindFastestPath(SourceID, DestID, FastestPath);
    }
    auto ProcessingDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-ProcessingStart);
    std::cerr<<"Outputting Results"<<std::endl;    
    std::vector< CMapRouter::TPathStep > OutPath;
    std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;
    for(int Index = 0; Index < NumRoutes; Index++){
        std::cout<<"In "<<__FILE__<<" @ "<<__LINE__<<std::endl;

        OutputBrief<<Index<<" SP:";
        if(std::numeric_limits<double>::max() == ShortestDistance[Index]){
            OutputBrief<<" (N/A)";   
        }
        else{
            OutputBrief<<" ("<<std::setprecision(2)<<ShortestDistance[Index]<<" mi)";    
        }
        if(Verbose){
            for(auto Node : ShortestPaths[Index]){
                OutputBrief<<" "<<Node;
            }
            
            if(std::numeric_limits<double>::max() != ShortestDistance[Index]){
                OutPath.clear();
                for(auto &NodeID : ShortestPaths[Index]){
                    OutPath.push_back(std::make_pair("Walk", NodeID));
                }
    
                auto Filename = std::to_string(Index) + "_" + std::to_string(ShortestDistance[Index]) + "mi.csv";
                std::ofstream OutFile(ResultsPath + Filename);
                if(OutFile.is_open()){
                    CCSVWriter PathOutput(OutFile);
                    std::vector< std::string > Row{"mode","node_id"};
                    PathOutput.WriteRow(Row);
                    for(auto &Step : OutPath){
                        Row[0] = std::get<0>(Step);
                        Row[1] = std::to_string(std::get<1>(Step));
                        PathOutput.WriteRow(Row);
                    }
                }
            }
        }
        
        OutputBrief<<std::endl;
        OutputBrief<<Index<<" FP:";
        if(std::numeric_limits<double>::max() == FastestTime[Index]){
            OutputBrief<<" (N/A)";   
        }
        else{
            int IntegralTime;
            double Remainder = FastestTime[Index];
            bool PrecededOutput = false;
            OutputBrief<<" (";
            IntegralTime = Remainder;
            if(IntegralTime){
                OutputBrief<<IntegralTime<<":";
                PrecededOutput = true;
            }
            Remainder -= IntegralTime;
            Remainder *= 60;
            IntegralTime = Remainder;
            if(IntegralTime or PrecededOutput){
                if(PrecededOutput){
                    OutputBrief<<std::setw(2)<<std::setfill('0');
                }
                OutputBrief<<IntegralTime<<":";
                PrecededOutput = true;
            }
            Remainder -= IntegralTime;
            Remainder *= 60;
            IntegralTime = Remainder;
            if(PrecededOutput){
                OutputBrief<<std::setw(2)<<std::setfill('0');
            }
            OutputBrief<<IntegralTime;
            if(PrecededOutput){
                OutputBrief<<")";
            }
            else{
                Remainder -= IntegralTime;
                Remainder *= 100;
                IntegralTime = Remainder;
                OutputBrief<<"."<<std::setw(2)<<std::setfill('0')<<IntegralTime<<" s)";    
            }
        }
        if(Verbose){
            for(auto Node : FastestPaths[Index]){
                OutputBrief<<" "<<std::get<1>(Node);
            }
            OutputBrief<<std::endl;
            if(std::numeric_limits<double>::max() != FastestTime[Index]){
                auto Filename = std::to_string(Index) + "_" + std::to_string(FastestTime[Index]) + "hr.csv";
                std::ofstream OutFile(ResultsPath + Filename);
                if(OutFile.is_open()){
                    CCSVWriter PathOutput(OutFile);
                    std::vector< std::string > Row{"mode","node_id"};
                    PathOutput.WriteRow(Row);
                    for(auto &Step : FastestPaths[Index]){
                        Row[0] = std::get<0>(Step);
                        Row[1] = std::to_string(std::get<1>(Step));
                        PathOutput.WriteRow(Row);
                    }
                }
            }
        }
        else{
            OutputBrief<<std::endl;    
        }
    }
    
    OutputBrief<<"Duration (load): "<<LoadDuration.count()<<std::endl;
    std::cerr<<"Duration (load): "<<LoadDuration.count()<<std::endl;
    OutputBrief<<"Duration (proc): "<<ProcessingDuration.count()<<std::endl;
    std::cerr<<"Duration (proc): "<<ProcessingDuration.count()<<std::endl;
    auto SamplesPerDay = long((86400000 - LoadDuration.count()) / (double(ProcessingDuration.count()) / NumRoutes));
    auto MarginOfError = long(double(SamplesPerDay) / sqrt(NumRoutes));
    OutputBrief<<"Queries per day: "<<SamplesPerDay<<" (+-"<<MarginOfError<<"), "<<(SamplesPerDay - MarginOfError)<<" min"<<std::endl;
    std::cerr<<"Queries per day: "<<SamplesPerDay<<" (+-"<<MarginOfError<<"), "<<(SamplesPerDay - MarginOfError)<<" min"<<std::endl;
    
    std::cout<<"@line: "<<__LINE__<<std::endl;

    //std::stringstream InOSM(OSMFileData);
    //std::stringstream InStops(StopsCSVFileData);
    //std::stringstream InRoutes(RoutesCSVFileData);
    //CMapRouter MapRouter1;

    bool Done = false;
    std::string Cur = "";
    std::vector<std::string> cmd;
    cmd.push_back(Cur);
    int counter = 0;
    while(!Done){
        std::string InputLine;
        std::cout<<"> ";
        std::getline(std::cin, InputLine);
        //std::string Temp = "";

        if(InputLine == "exit"){
            Done = true;
        }
        //up arrow 
        else if(InputLine == "^[[A"){
            counter++;
            std::getline(std::cin, cmd[counter]);
            
        }
        //down arrow
        else if(InputLine == "^[[B"){
            if(counter){
                counter--;
                std::getline(std::cin, cmd[counter]);

            }
        }
        //left
        else if(InputLine == "^[[D"){
            
        }
        //right
        else if(InputLine == "^[[C"){
            
        }
        //manu
        else if(InputLine == "help"){
            std::cout<<"help     Display this help menu "<< std::endl;
            std::cout<<"exit     Exit the program "<< std::endl;
            std::cout<<"node     Syntax \"node [0, count)\" "<< std::endl;
            std::cout<<"         Will output node ID and Lat/Lon for node"<< std::endl;
            std::cout<<"fastest  Syntax \"fastest start end\" "<< std::endl;
            std::cout<<"         Calculates the time for fastest path from start to end"<< std::endl;
            std::cout<<"shortest Syntax \"shortest start end\" "<< std::endl;
            std::cout<<"         Calculates the distance for the shortest path from start to end"<< std::endl;
            std::cout<<"save     Saves the last calculated path to file"<< std::endl;
            std::cout<<"print    Prints the steps for the last calculated"<< std::endl;
            Cur = InputLine;
            cmd.push_back(Cur);
        }

        else if(InputLine == "node"){

            Cur = InputLine;
            cmd.push_back(Cur);
        }

        else if(InputLine == "fastest"){
            std::string Fsat = "";
            //for(auto &Step : FastestPaths[Index]){
            //    Fsat += std::get<0>(Step) + " " + std::to_string(std::get<1>(Step));;
            //}
            //std::cout<< Fsat << std::endl;
            Cur = InputLine;
            cmd.push_back(Cur);
        }

        else if(InputLine == "shortest"){
            std::string Short = "";
            for(auto &Step : OutPath){
                Short += std::get<0>(Step) + " " + std::to_string(std::get<1>(Step));;
            }

            std::cout<< Short << std::endl;
            Cur = InputLine;
            cmd.push_back(Cur);
        }

        else if(InputLine == "save"){
            //auto Filename = std::to_string(Index) + "_" + std::to_string(  ) + "hr.csv";
            //std::ofstream OutFile(ResultsPath + Filename);
            //Output.is_open()
            //std::ofstream Output(ResultsPath + Filename);
            //CCSVWriter Writer(Output);
            //std::vector< std::string > Row{"path"};
            //Writer.WriteRow(Row);
            //Cur = InputLine;
            //cmd.push_back(Cur);
        }

        else if(InputLine == "print"){

            Cur = InputLine;
            cmd.push_back(Cur);
        }
    
        else{
            std::cout<<"Unknown command: "<< InputLine << std::endl;
            Cur = InputLine;
            cmd.push_back(Cur);
        }
    }
    return EXIT_SUCCESS;
}
