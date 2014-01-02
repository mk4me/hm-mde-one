/********************************************************************
	created:	2013/12/20
	created:	20:12:2013   11:54
	filename: 	LayersXmlParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYERSXMLPARSER_H__
#define HEADER_GUARD_DICOM__LAYERSXMLPARSER_H__

#include <utils/SmartPtr.h>
#include <corelib/Filesystem.h>
#include <corelib/IParser.h>

namespace dicom {

//! Parser plików xml z warstwami
class LayersXmlParser : public plugin::IParser, public plugin::ISourceParserCapabilities
{
    UNIQUE_ID("{D572DC2F-934B-43CE-9460-12ABAFFB20E0}")
	CLASS_DESCRIPTION("Layers parser", "Layers parser")
	
public:
    LayersXmlParser();
    virtual ~LayersXmlParser();

public:
    //! Parsowanie pliku c3d
    //! \param path poprawna ścieżka do pliku
    virtual void parse(const std::string & source);
    //! \return pusty obiekt nowego parsera
    virtual plugin::IParser* create() const;
    //! Zwraca obiekty dostarczone przez parser
    //! \param objects kolekcja z obiektami (set)
    virtual void getObjects(core::Objects& objects);
    //! Zwraca rozszerzenia, które są obsługiwane przez parser (tylko c3d)
    //! \param extensions kolecja z roszerzeniami
    virtual void acceptedExpressions(Expressions & expressions) const;

private:
    utils::ObjectWrapperPtr layers;
};

}
#endif
