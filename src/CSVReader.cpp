#include "CSVReader.h"
#include <iostream>

CCSVReader::CCSVReader(std::istream &in) : Input(in){
	csv_init(&Parser, 0);	
}

CCSVReader::~CCSVReader(){
	csv_free(&Parser);
}

void CCSVReader::CallbackField(void *str, size_t len, void *data){//given back a pointer as a data parameter
	CCSVReader *Ptr = static_cast<CCSVReader*>(data);//returns a value as type CCSVReader//can access all member functions inside CSVreader like the parser
	Ptr -> RowBuffer.push_back(std::string((char*)str,len));
}

void CCSVReader::CallbackRow(int ch, void *data){//giving a phone number to get a callback//int is like phone number and data is like my name 
	std::cout<<"@ line "<<__LINE__<<std::endl;
	CCSVReader *Ptr = static_cast<CCSVReader*>(data);
	Ptr -> BufferedRows.push(Ptr ->RowBuffer);
	Ptr -> RowBuffer.clear();
}

bool CCSVReader::End() const{
	if (BufferedRows.empty()){
	return true;
	}
	return false;
}	
bool CCSVReader::ReadRow(std::vector<std::string> &row){

	//std::string *ss = static_cast<std::string*>(in);
	//ss.seekg(0,in.end);
	//size_t streamsize = ss.tellg();
	//csv_parse(&Parser, ss, streamsize, CallbackField, CallbackRow, this);
	//if(End()){
	//	return false;
//}
	//else{
	//	BufferedRows.pop();
	//	return true;

	//while (!RowBuffer.empty() and !Input.eof()){
	//	char Buffer[2048];
	//	Input.read(Buffer, sizeof(Buffer));
	//	csv_parse(&Parser, Buffer, Input.gcount(), CallbackField, CallbackRow, this);
	//}
	//csv_fini(&Parser, CallbackField, CallbackRow, this);
	//row = BufferedRows.front();
	//BufferedRows.pop();
	//return true;

	while (RowBuffer.empty() and !Input.eof()){
		char Buffer[2048];
		Input.read(Buffer, sizeof(Buffer));
		int size = Input.gcount();
		csv_parse(&Parser, Buffer, size, CallbackField, CallbackRow, this);
	}
	csv_fini(&Parser, CallbackField, CallbackRow, this);
	row = BufferedRows.front();
	if(!BufferedRows.empty()){
		BufferedRows.pop();
	}
	return !row.empty();

}


