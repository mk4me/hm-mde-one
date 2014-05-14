/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   15:15
	filename: 	IncrementalBranchShallowCopyParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_WEBSERVICES__INCREMENTALBRANCHSHALLOWCOPYPARSER_H__
#define HEADER_GUARD_WEBSERVICES__INCREMENTALBRANCHSHALLOWCOPYPARSER_H__

#include <string>
#include <webserviceslib/IncrementalBranchShallowCopy.h>
namespace webservices {

//! Parser przyrostowej p³ytkiej kopii bazy danych
class IncrementalBranchShallowCopyParser
{
public:
    //! \param path Œcie¿ka do pliku do przeparsowania
    //! \param shallowCopy [out] Struktura przyrostowej p³ytkiej kopii danych ruchu do wype³nienia
    static void parseFile(const std::string & path, IncrementalBranchShallowCopy& shallowCopy);
};


}
#endif
