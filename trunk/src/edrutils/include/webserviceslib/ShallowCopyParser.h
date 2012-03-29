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

//! Parser p�ytkiej kopi danych ruchu
class MotionShallowCopyParser
{
public:
    //! \param path �cie�ka do pliku do przeparsowania
    //! \param shallowCopy [out] Struktura p�ytkiej kopii danych ruchu do wype�nienia
    static void parseFile(const std::string & path, MotionShallowCopy::ShallowCopy & shallowCopy);
};

//! Parser p�ytkiej kopi danych medycznych
class MedicalShallowCopyParser
{
public:
    //! \param path �cie�ka do pliku do przeparsowania
    //! \param shallowCopy [out] Struktura p�ytkiej kopii danych medycznych do wype�nienia
    static void parseFile(const std::string & path, MedicalShallowCopy::ShallowCopy & shallowCopy);
};

}

#endif	//	HEADER_GUARD___SHALLOWCOPYPARSER_H__
