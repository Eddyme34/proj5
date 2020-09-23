#include "CSVWriter.h"
#include "StringUtils.h"
#include <iostream>

CCSVWriter::CCSVWriter(std::ostream &out) : Output(out){
//std::string *ss = static_cast<std::string*>(out);
//int curRow = 0;
}

CCSVWriter::~CCSVWriter(){
}

bool CCSVWriter::WriteRow(const std::vector<std::string> &row){
	std::vector< std::string > OutputRow;
	for(auto &Field : row){
		OutputRow.push_back(std::string("\"") + StringUtils::Replace(Field,"\"","\"\"") +"\"");
	}
	Output<<StringUtils::Join(",",OutputRow)<<std::endl;
	return Output.good();
//	size_t streamsize = ss.size();
//	if(csv_write(row, streamsize*2+2, ss, streamsize)){
//	Output.write(row[curRow++]);
//	return true;
//	}
//	return false;
}
