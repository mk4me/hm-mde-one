/********************************************************************
	created:	2013/12/20
	created:	20:12:2013   12:01
	filename: 	InternalXmlParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__INTERNALXMLPARSER_H__
#define HEADER_GUARD_DICOM__INTERNALXMLPARSER_H__

#include <utils/SmartPtr.h>
#include <corelib/Filesystem.h>
#include <corelib/IParser.h>

namespace dicom {

//! Parser plików png, dostarcza obrazow z warstwami
class InternalXmlParser : public plugin::IParser, public plugin::ISourceParserCapabilities
{
    UNIQUE_ID("{10F620FF-98EC-46A3-BE64-6269EFB16784}")
	CLASS_DESCRIPTION("PNG parser", "PNG parser")
	
public:
    InternalXmlParser();
    virtual ~InternalXmlParser();

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
    utils::ObjectWrapperPtr sessionInternal;
};

}
#endif
