#include "XMLWriter.h"
#include "StringUtils.h"
#include <iostream>
        CXMLWriter::CXMLWriter(std::ostream &os) : Output(os){
        
        }
        CXMLWriter::~CXMLWriter(){

        }
        
        bool CXMLWriter::Flush(){
        SXMLEntity AnotherEntity;
        AnotherEntity.DType = SXMLEntity::EType::CompleteElement;
        }
        bool CXMLWriter::WriteEntity(const SXMLEntity &entity){
        
        }