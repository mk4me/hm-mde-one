/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:26
	filename: MetadataParser.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_METADATAPARSER_H__
#define HEADER_GUARD__HMDBSERVICES_METADATAPARSER_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/ShallowCopy.h>

namespace hmdbServices
{
	//! Parser metadanych ruchu
	class HMDBSERVICES_EXPORT MotionMetadataParser
	{
	public:
		//! \param path Ścieżka do pliku do przeparsowania
		//! \param metadata [out] Struktura metadanych do wypełnienia
		static void parseFile(const std::string & path, MotionMetaData::MetaData & metadata);
	};

	//! Parser metadanych medycznych
	class HMDBSERVICES_EXPORT MedicalMetadataParser
	{
	public:
		//! \param path Ścieżka do pliku do przeparsowania
		//! \param metadata [out] Struktura metadanych do wypełnienia
		static void parseFile(const std::string & path, MedicalMetaData::MetaData & metadata);
	};
}

#endif	//	HEADER_GUARD__HMDBSERVICES_METADATAPARSER_H__
