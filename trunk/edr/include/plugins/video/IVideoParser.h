/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   13:02
	filename: IVideoParser.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__IVIDEOPARSER_H__
#define __HEADER_GUARD_VIDEO__IVIDEOPARSER_H__

#include <core/IParser.h>
#include <vidlib/VideoStream.h>
#include <vidlib/osg/VideoImageStream.h>

CORE_DEFINE_WRAPPER(vidlib::VideoImageStream, utils::PtrPolicyOSG);

//! Parser plik�w video korzystaj�cy z vidliba.
class IVideoParser : public core::IParser
{
public:
    //! \return Strumie� zgodny z OSG. Wrapuje rezultat getStream.
    virtual vidlib::VideoImageStream* getOsgStream() = 0;
    //! \return Strumie� video. Instancja jest wsp�dzielona przez wszystkich
    //!         klient�w parsera; dlatego te� gdy chce si� r�cznie kontrolowa�
    //!         czas nale�y sklonowa� strumie�.
    virtual vidlib::VideoStream* getStream() = 0;
};

typedef core::shared_ptr<IVideoParser> IVideoParserPtr;
typedef core::shared_ptr<const IVideoParser> IVideoParserConstPtr;
typedef core::weak_ptr<IVideoParser> IVideoParserWeakPtr;

#endif  // __HEADER_GUARD_VIDEO__IVIDEOPARSER_H__