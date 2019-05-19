#include "CSVReader.h"

CCSVReader::CCSVReader(std::istream &in) : Input(in){
	csv_init(&Parser, 0);
}

CCSVReader::~CCSVReader(){
	csv_free(&Parser);
}

void CCSVReader::CallbackField(void *str, size_t len, void *data){
	CCSVReader *Ptr = static_cast<CCSVReader*>(data);
	RowBuffer.push_back(std::string(str,len));
}

void CCSVReader::CallbackRow(int ch, void *data){
	CCSVReader *Ptr = static_cast<CCSVReader*>(data);
}

bool CCSVReader::End() const{

}	
bool CCSVReader::ReadRow(std::vector<std::string> &row){

}


