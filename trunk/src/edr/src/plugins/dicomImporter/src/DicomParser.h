/********************************************************************
	created:	2013/08/12
	created:	12:8:2013   22:54
	filename: 	DicomParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__DICOMPARSER_H__
#define HEADER_GUARD_DICOMIMPORTER__DICOMPARSER_H__

#include <utils/SmartPtr.h>
#include <corelib/Filesystem.h>
#include <corelib/IParser.h>

namespace dicomImporter {

//! Klasa wykorzystuje dcmtk do zasilenia systemu w obiekty domenowe DICOM
class DicomParser : public plugin::IParser, public plugin::ISourceParserCapabilities
{
    UNIQUE_ID("{5E9E4252-2CC4-4992-8A86-381EE5AE611D}")
	CLASS_DESCRIPTION("DICOM parser", "DICOM parser")
	
public:
    DicomParser();
    virtual ~DicomParser();

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
    utils::ObjectWrapperPtr image;
};

}
#endif
