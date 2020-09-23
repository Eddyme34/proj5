#ifndef XMLREADER_H
#define XMLREADER_H

#include "XMLEntity.h"
#include <istream>
#include <expat.h>
#include <queue>

class CXMLReader{
    private:
    std::istream &Input;
    SXMLEntity Entity;
    XML_Parser Parser;
    std::queue< SXMLEntity > BufferedArts;
    static void CallbackBeg(void* userData, const XML_Char *name, const XML_Char **atts);
    static void CallbackEnd(void* userData, const XML_Char *name);
    static void CallbackHandler(void* userData, const XML_Char *s, int len);
    public:
        CXMLReader(std::istream &is);
        ~CXMLReader();
        bool End();
        bool ReadEntity(SXMLEntity &entity, bool skipcdata = false);
};

#endif
