/********************************************************************
    created:  2013/01/11
    created:  11:1:2013   8:18
    filename: ParserManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PARSERMANAGER_H__
#define HEADER_GUARD___PARSERMANAGER_H__

#include <core/IParserManagerReader.h>
#include <boost/regex.hpp>

namespace core {

class ParserManager : public IParserManagerReader
{
private:

	//! Struktura rozszerzająca opis wyrażenia o wyrażenie regularne uzywane do weryfikacji ścieżki źródła
	struct ExpressionDescription : public plugin::IParser::ExpressionDescription
	{
		boost::regex regularExpression;
	};

	typedef std::map<std::string, ExpressionDescription> Expressions;

	//! Dane wyciągnięte z parsera  używamy ich do określenia czy dane zasoby mogą być przez parser obsłużone
	//! Opis parsera strumieni
	struct ParserData
	{
		Expressions expressions;
		plugin::IStreamParserCapabilities * streamParser;
		plugin::ISourceParserCapabilities * sourceParser;
	};

	//! Zbiór parserów z ich opisami
	typedef std::map<plugin::IParserPtr, ParserData> Parsers;

private:

	Parsers parsers_;

public:
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów
	virtual void parsers(ParserPrototypes & parserPrototypes) const;
	//! \param source Ścieżka źródła danych
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów akceptującymi zadane źródło
	virtual void parsers(const std::string & source, ParserPrototypes & parserPrototypes) const;
	//! Tylko parsery z włąsnym I/O
	//! \param source Ścieżka źródła danych
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów akceptującymi zadane źródło
	virtual void sourceParsers(const std::string & source, ParserPrototypes & parserPrototypes) const;
	//! Tylko parsery strumieniowe
	//! \param source Ścieżka źródła danych
	//! \param parserPrototypes [out] Kolekcja wypełniania aktualnie znanymi prototypami parserów akceptującymi zadane źródło
	virtual void streamParsers(const std::string & source, ParserPrototypes & parserPrototypes) const;
	//! \param source Ścieżka źródła danych dla której spradzamy czy są jakieś parsery
	//! \return Prawda jeśli dana ścieżka źródła jest obsługiwana przez jakiś parser
	virtual const bool sourceIsAccepted(const std::string & source) const;
	//! \param source Ścieżka źródła danych dla której spradzamy możliwe do uzyskania typy danych
	//! \param types [out] Zbiór możliwych do uzyskania z danego źródła typów danych
	virtual void sourcePossibleTypes(const std::string & source, TypeInfoSet & types) const;

	//! \param parser Rejestrowany parser
	void registerParser(const plugin::IParserPtr & parser);
};

}

#endif	//	HEADER_GUARD___PARSERMANAGER_H__
