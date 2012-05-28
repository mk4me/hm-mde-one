/********************************************************************
	created:	2011/05/25
	created:	25:5:2011   10:14
	filename: 	C3DParser.h
	author:		Wojciech Knieæ
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PLUGINC3D__C3DPARSER_H__
#define HEADER_GUARD_PLUGINC3D__C3DPARSER_H__

#include <core/SmartPtr.h>
#include <core/Filesystem.h>
#include <core/IParser.h>
#include <c3dlib/C3DParser.h>


//! Klasa wykorzystuje c3dlib do zasilenia systemu w obiekty domenowe pochodz¹ce z plików C3D
class C3DParser : public core::IParser
{
    UNIQUE_ID("{D7801231-BACA-42C6-9A8E-706F561A563F}", "C3D parser");
	typedef core::shared_ptr<c3dlib::C3DParser> ParserPtr;
private:
    //! Obsolete. Wektor zawiera 4 kanaly analogowe z plyt GRF
	std::vector<core::ObjectWrapperPtr> GRFChannels;
    //! Obsolete. Wektor zawiera 16 kanalow analogowych EMG
	std::vector<core::ObjectWrapperPtr> EMGChannels;
    //! Kolekcja z danymi GRF
	core::ObjectWrapperPtr GRFs;
    //! Kolekcja z danymi EMG
	core::ObjectWrapperPtr EMGs;
    //! Kolekcja z markerami
	core::ObjectWrapperPtr markerChannels;
    //! Kolekcja z silami w stawach - dane sa estymowane
	core::ObjectWrapperPtr forceChannels;
    //! Kolekcja z kontami w stawach - dane sa estymowane
	core::ObjectWrapperPtr angleChannels;
    //! Kolekcja z momentami sil w stawach - dane sa estymowane
	core::ObjectWrapperPtr momentChannels;
    //! Kolecka z mocami w stawach - dane sa estymowane
	core::ObjectWrapperPtr powerChannels;
    //! Kolekcja ze zdarzeniami zapisanymi w pliku C3D (np. stapniecie stopy)
    core::ObjectWrapperPtr allEvents;
    //! Wskaznik do wlasciwego parsera z biblioteki c3dlib
	ParserPtr parserPtr;

public:
    C3DParser();
    virtual ~C3DParser();

public:
    //! Parsowanie pliku c3d
    //! \param path poprawna sciezka do pliku
    virtual void parseFile(const core::Filesystem::Path& path);
    //! \return pusty obiekt nowego parsera
    virtual core::IParser* create();
    //! Zwraca obiekty dostarczone przez parser
    //! \param objects kolekcja z obiektami (set)
    virtual void getObjects(core::Objects& objects);
    //! Zwraca rozszerzenia, ktore sa obslugiwane przez parser (tylko asf)
    //! \param extensions kolecja z roszerzeniami
    virtual void getSupportedExtensions(Extensions & extensions) const;
	//! Zapisauje dane w konkretnym miejscu, nie uzywane
	//! \param path sciezka zapisu
	void saveFile(const core::Filesystem::Path& path);
};


#endif