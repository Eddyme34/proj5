#ifndef CSVREADER_H
#define CSVREADER_H

#include <istream>
#include <string>
#include <queue>
#include <vector>
#include <csv.h>

class CCSVReader{
    protected:
	std::istream &Input;
	struct csv_parser Parser;
	std::queue<std::vector<std::string>>BufferedRows;
	std::vector<std::string> RowBuffer;	
        static void CallbackField(void *str, size_t len, void *data);
	static void CallbackRow(int ch, void *data);
    public:
        CCSVReader(std::istream &in);
        ~CCSVReader();
        
        bool End() const;
        bool ReadRow(std::vector< std::string > &row);
};

#endif
