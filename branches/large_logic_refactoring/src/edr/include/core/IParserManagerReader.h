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
#include <core/IParser.h>

namespace plugin {

	//! Interfejs daj¹cy dostêp do zarejestrowanych w aplikacji parserów
	class IParserManagerReader
	{
	public:
		//! Typ agreguj¹cy prototypy parserów
		typedef std::list<IParserConstPtr> ParserPrototypes;

	public:
		//! \param parserPrototypes [out] Kolekcja wype³niania aktualnie znanymi prototypami parserów
		virtual void parsers(ParserPrototypes & parserPrototypes) const = 0;
		//! \param source Œcie¿ka Ÿród³a danych
		//! \param parserPrototypes [out] Kolekcja wype³niania aktualnie znanymi prototypami parserów akceptuj¹cymi zadane Ÿród³o
		virtual void parsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! Tylko parsery z w³¹snym I/O
		//! \param source Œcie¿ka Ÿród³a danych
		//! \param parserPrototypes [out] Kolekcja wype³niania aktualnie znanymi prototypami parserów akceptuj¹cymi zadane Ÿród³o
		virtual void sourceParsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! Tylko parsery strumieniowe
		//! \param source Œcie¿ka Ÿród³a danych
		//! \param parserPrototypes [out] Kolekcja wype³niania aktualnie znanymi prototypami parserów akceptuj¹cymi zadane Ÿród³o
		virtual void streamParsers(const std::string & source, ParserPrototypes & parserPrototypes) const = 0;
		//! \param source Œcie¿ka Ÿród³a danych dla której spradzamy czy s¹ jakieœ parsery
		//! \return Prawda jeœli dana œcie¿ka Ÿród³a jest obs³ugiwana przez jakiœ parser
		virtual const bool sourceIsAccepted(const std::string & source) const = 0;
		//! \param source Œcie¿ka Ÿród³a danych dla której spradzamy mo¿liwe do uzyskania typy danych
		//! \param types [out] Zbiór mo¿liwych do uzyskania z danego Ÿród³a typów danych
		virtual void sourcePossibleTypes(const std::string & source, core::TypeInfoSet & types) const = 0;
	};
}

#endif	//	HEADER_GUARD___IPARSERMANAGERREADER_H__