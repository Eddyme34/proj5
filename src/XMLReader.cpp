#include "XMLReader.h"

CXMLReader::CXMLReader(std::istream &is) : Input(is){
    Parser = XML_ParserCreate(NULL);
    XML_SetUserData(Parser, this);
    XML_SetElementHandler(Parser, CallbackBeg, CallbackEnd);
    XML_SetCharacterDataHandler(Parser, CallbackHandler);
}

CXMLReader::~CXMLReader(){
    XML_ParserFree(Parser);
    }

void CXMLReader::CallbackBeg(void* userData, const XML_Char *name, const XML_Char **atts){
    CXMLReader *Ptr = static_cast<CXMLReader*>(userData);
    SXMLEntity AnotherEntity;
    AnotherEntity.DNameData = std::string(name);
    AnotherEntity.DType = SXMLEntity::EType::StartElement;
    for(int ch = 0; atts[ch]; ch+=2){
        AnotherEntity.SetAttribute(std::string(atts[ch]), std::string(atts[ch+1]));
        //AnotherEntity.DAttributes.push_back(std::make_pair< std::string name, std::string atts>);
    }
    Ptr -> BufferedArts.push(AnotherEntity);
}

void CXMLReader::CallbackEnd(void* userData, const XML_Char *name){
    CXMLReader *Ptr = static_cast<CXMLReader*>(userData);
    SXMLEntity AnotherEntity;
    AnotherEntity.DType = SXMLEntity::EType::EndElement;
    AnotherEntity.DNameData = std::string(name);
    Ptr -> BufferedArts.push(AnotherEntity);
}

void CXMLReader::CallbackHandler(void* userData, const XML_Char *s, int len){
    CXMLReader *Ptr = static_cast<CXMLReader*>(userData);
    SXMLEntity AnotherEntity;
    AnotherEntity.DNameData = std::string(s, len);
    AnotherEntity.DType = SXMLEntity::EType::CharData;
    Ptr -> BufferedArts.push(AnotherEntity);
}

bool CXMLReader::End(){
    if(BufferedArts.empty() and Input.eof()){
        return true;
    }
    return false;
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    while(BufferedArts.empty() and !Input.eof()){
        //if(skipcdata == false){
            char buffer[1024];
            Input.read(buffer,sizeof(buffer));
            XML_Parse(Parser, buffer, Input.gcount(), Input.eof());
        //}
    }
    if(skipcdata != false){
        while(BufferedArts.empty() == false && BufferedArts.front().DType == SXMLEntity::EType::CharData ){
            BufferedArts.pop();
        }
    }

    if(!BufferedArts.empty()){
            entity = BufferedArts.front();
            BufferedArts.pop();
         return true;
    }

    return false;
}