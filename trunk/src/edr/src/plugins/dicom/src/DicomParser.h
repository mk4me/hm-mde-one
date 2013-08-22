/********************************************************************
	created:	2013/08/12
	created:	12:8:2013   22:54
	filename: 	DicomParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMPARSER_H__
#define HEADER_GUARD_DICOM__DICOMPARSER_H__

#include <utils/SmartPtr.h>
#include <corelib/Filesystem.h>
#include <corelib/IParser.h>

//! Klasa wykorzystuje dcmtk do zasilenia systemu w obiekty domenowe DICOM
class DicomParser : public plugin::IParser, public plugin::ISourceParserCapabilities
{
    UNIQUE_ID("{A0173382-1638-4AFA-BC31-123FC6C55461}")
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
    utils::ObjectWrapperPtr wrapper;
    utils::ObjectWrapperPtr image;
};


#endif
