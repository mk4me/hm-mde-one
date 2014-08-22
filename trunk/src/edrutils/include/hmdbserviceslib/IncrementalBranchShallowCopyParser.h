/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   15:15
	filename: 	IncrementalBranchShallowCopyParser.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_HMDBSERVICES__INCREMENTALBRANCHSHALLOWCOPYPARSER_H__
#define HEADER_GUARD_HMDBSERVICES__INCREMENTALBRANCHSHALLOWCOPYPARSER_H__

#include <hmdbserviceslib/Export.h>
#include <string>
#include <hmdbserviceslib/IncrementalBranchShallowCopy.h>

namespace hmdbServices {
	//! Parser przyrostowej p�ytkiej kopii bazy danych
	class HMDBSERVICES_EXPORT IncrementalBranchShallowCopyParser
	{
	public:
		//! \param document Strumie� z danymi do parsowania
		//! \param shallowCopy [out] Struktura przyrostowej p�ytkiej kopii danych ruchu do wype�nienia
		//! \return Czy uda�o si� sparsowa� strumie�
		static const bool parseFile(std::istream * document, IncrementalBranchShallowCopy& shallowCopy);
	};
}
#endif
