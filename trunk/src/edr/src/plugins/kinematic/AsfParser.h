/********************************************************************
	created:	2012/05/24
	created:	24:5:2012   20:53
	filename: 	AsfParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__ASFPARSER_H__
#define HEADER_GUARD_KINEMATIC__ASFPARSER_H__

#include <core/IParser.h>
#include <core/IDataManager.h>

//! klasa wykorzystuje bibliotekê kinematiclib aby wczytywaæ pliki asf
class AsfParser : public core::IParser
{
    UNIQUE_ID("{0E3B8309-AA5B-4ECD-B941-8FA64F8C9625}", "Asf parser");
private:
    //! object wrapper z wczytanym plikiem
    core::ObjectWrapperPtr skeletalModel;

public:
    AsfParser();
    virtual ~AsfParser();

public:
    //! Parsowanie pliku
    //! \param path scie¿ka do parsowanego pliku
    virtual void parseFile(const core::Filesystem::Path& path);
    //! \return pusty obiekt nowego parsera
    virtual core::IParser* create();
    //! Zwraca rozszerzenia, które s¹ obs³ugiwane przez parser (tylko asf)
    //! \param extensions kolecja z roszerzeniami
    virtual void getSupportedExtensions(Extensions & extensions) const;
    //! Zwraca obiekty dostarczone przez parser
    //! \param objects kolekcja z obiektami (set)
    virtual void getObjects(core::Objects& objects);
};


#endif
