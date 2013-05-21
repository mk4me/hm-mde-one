/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:26
    filename: MetadataParser.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___METADATAPARSER_H__
#define HEADER_GUARD___METADATAPARSER_H__

#include <boost/shared_ptr.hpp>
#include <webserviceslib/Entity.h>

namespace webservices
{

//! Parser metadanych ruchu
class MotionMetadataParser
{
public:    
    //! \param path Ścieżka do pliku do przeparsowania
    //! \param metadata [out] Struktura metadanych do wypełnienia
    static void parseFile(const std::string & path, MotionMetaData::MetaData & metadata);
};

//! Parser metadanych medycznych
class MedicalMetadataParser
{
public:
    //! \param path Ścieżka do pliku do przeparsowania
    //! \param metadata [out] Struktura metadanych do wypełnienia
    static void parseFile(const std::string & path, MedicalMetaData::MetaData & metadata);
};

}

#endif	//	HEADER_GUARD___METADATAPARSER_H__
