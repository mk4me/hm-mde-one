/********************************************************************
    created:  2012/02/29
    created:  29:2:2012   8:54
    filename: ShallowCopyParser.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___SHALLOWCOPYPARSER_H__
#define HEADER_GUARD___SHALLOWCOPYPARSER_H__

#include <webserviceslib/Entity.h>

namespace webservices
{

//! Parser p³ytkiej kopi danych ruchu
class MotionShallowCopyParser
{
public:
    //! \param path Œcie¿ka do pliku do przeparsowania
    //! \param shallowCopy [out] Struktura p³ytkiej kopii danych ruchu do wype³nienia
    static void parseFile(const std::string & path, MotionShallowCopy::ShallowCopy & shallowCopy);
};

//! Parser p³ytkiej kopi danych medycznych
class MedicalShallowCopyParser
{
public:
    //! \param path Œcie¿ka do pliku do przeparsowania
    //! \param shallowCopy [out] Struktura p³ytkiej kopii danych medycznych do wype³nienia
    static void parseFile(const std::string & path, MedicalShallowCopy::ShallowCopy & shallowCopy);
};

}

#endif	//	HEADER_GUARD___SHALLOWCOPYPARSER_H__
