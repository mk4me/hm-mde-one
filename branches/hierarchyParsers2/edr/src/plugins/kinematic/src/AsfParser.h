/********************************************************************
	created:	2012/05/24
	created:	24:5:2012   20:53
	filename: 	AsfParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__ASFPARSER_H__
#define HEADER_GUARD_KINEMATIC__ASFPARSER_H__

#include <corelib/IParser.h>
#include <corelib/IDataManagerReader.h>

//! klasa wykorzystuje bibliotekę kinematiclib aby wczytywać pliki asf
class AsfParser : public plugin::IParser, public plugin::ISourceParserCapabilities
{    
	UNIQUE_ID("{0E3B8309-AA5B-4ECD-B941-8FA64F8C9625}")
	CLASS_DESCRIPTION("Asf parser", "Asf parser")
private:
    //! object wrapper z wczytanym plikiem
    core::ObjectWrapperPtr skeletalModel;

public:
    AsfParser();
    virtual ~AsfParser();

public:
    //! Parsowanie pliku
    //! \param path scieżka do parsowanego pliku
	virtual void parse(const std::string & source);
	//! tworzy pusty obiekt parsera
	virtual plugin::IParser* create() const;
	//! zwraca wspierane rozszerzenia plików (asf)
	//! \param extensions kolecja, do której trafią wspierane rozszerzenia
	virtual void acceptedExpressions(Expressions & expressions) const;
    //! Zwraca obiekty dostarczone przez parser
    //! \param objects kolekcja z obiektami (set)
    virtual void getObjects(core::IHierarchyItemPtr& hierarchy, core::Objects& additionalObjects );
};


#endif
