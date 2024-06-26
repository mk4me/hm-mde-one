/********************************************************************
    created:  2013/01/14
    created:  14:1:2013   16:13
    filename: IParserManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPARSERMANAGERREADER_H__
#define HEADER_GUARD___IPARSERMANAGERREADER_H__

#include <list>
#include <map>
#include <corelib/IParser.h>

namespace core {

	//! Interfejs daj�cy dost�p do zarejestrowanych w aplikacji parser�w
	class IParserManagerReader
	{
	public:
		//! Typ agreguj�cy prototypy parser�w
		typedef std::list<plugin::IParserConstPtr> ParserPrototypes;
		//! Typ opisuj�cy indexy i typ parsera dla danego �r�d�a
		typedef std::map<int, utils::TypeInfo> TypesMap;

	public:
		//! Destrutkor wirtualny
		virtual ~IParserManagerReader() {}

		//! \param parserPrototypes [out] Kolekcja wype�niania aktualnie znanymi prototypami parser�w
		virtual void parsers(ParserPrototypes & parserPrototypes) const = 0;
		//! \param source �cie�ka �r�d�a danych
		//! \param parserPrototypes [out] Kolekcja wype�niania aktualnie znanymi prototypami parser�w akceptuj�cymi zadane �r�d�o
		virtual void parsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! Tylko parsery z w��snym I/O
		//! \param source �cie�ka �r�d�a danych
		//! \param parserPrototypes [out] Kolekcja wype�niania aktualnie znanymi prototypami parser�w akceptuj�cymi zadane �r�d�o
		virtual void sourceParsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! Tylko parsery strumieniowe
		//! \param source �cie�ka �r�d�a danych
		//! \param parserPrototypes [out] Kolekcja wype�niania aktualnie znanymi prototypami parser�w akceptuj�cymi zadane �r�d�o
		virtual void streamParsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! \param source �cie�ka �r�d�a danych dla kt�rej spradzamy czy s� jakie� parsery
		//! \return Prawda je�li dana �cie�ka �r�d�a jest obs�ugiwana przez jaki� parser
		virtual const bool sourceIsAccepted(const std::string & source) const = 0;
		//! \param source �cie�ka �r�d�a danych dla kt�rej spradzamy mo�liwe do uzyskania typy danych
		//! \param types [out] Zbi�r mo�liwych do uzyskania z danego �r�d�a typ�w danych
		virtual void sourcePossibleTypes(const std::string & source, utils::TypeInfoSet & types) const = 0;
		//! \param parserID Identyfikator parsera
		//! \param source �r�d�o
		//! \return Wspierane typy zarejestrowane w HierarchyManager
		virtual const TypesMap parserTypes(const core::UniqueID parserID, const std::string & source) const = 0;
	};
}

#endif	//	HEADER_GUARD___IPARSERMANAGERREADER_H__