/********************************************************************
	created:	2011/05/25
	created:	25:5:2011   10:14
	filename: 	C3DParser.h
	author:		Wojciech Knie�
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PLUGINC3D__C3DPARSER_H__
#define HEADER_GUARD_PLUGINC3D__C3DPARSER_H__

#include <core/SmartPtr.h>
#include <core/Filesystem.h>
#include <core/IParser.h>
#include <c3dlib/C3DParser.h>


//! Klasa wykorzystuje c3dlib do zasilenia systemu w obiekty domenowe pochodz�ce z plik�w C3D
class C3DParser : public core::IParser
{
    UNIQUE_ID("{D7801231-BACA-42C6-9A8E-706F561A563F}", "C3D parser");
	typedef core::shared_ptr<c3dlib::C3DParser> ParserPtr;
private:
    //! Obsolete. Wektor zawiera 4 kana�y analogowe z p�yt GRF
	std::vector<core::ObjectWrapperPtr> GRFChannels;
    //! Obsolete. Wektor zawiera 16 kana��w analogowych EMG
	std::vector<core::ObjectWrapperPtr> EMGChannels;
    //! Kolekcja z danymi GRF
	core::ObjectWrapperPtr GRFs;
    //! Kolekcja z danymi EMG
	core::ObjectWrapperPtr EMGs;
    //! Kolekcja z markerami
	core::ObjectWrapperPtr markerChannels;
    //! Kolekcja z si�ami w stawach - dane s� estymowane
	core::ObjectWrapperPtr forceChannels;
    //! Kolekcja z k�tami w stawach - dane s� estymowane
	core::ObjectWrapperPtr angleChannels;
    //! Kolekcja z momentami si� w stawach - dane s� estymowane
	core::ObjectWrapperPtr momentChannels;
    //! Kolekcja z mocami w stawach - dane s� estymowane
	core::ObjectWrapperPtr powerChannels;
    //! Kolekcja ze zdarzeniami zapisanymi w pliku C3D (np. st�pni�cie stopy)
    core::ObjectWrapperPtr allEvents;
    //! wska�nik do w�a�ciwego parsera z biblioteki c3dlib
	ParserPtr parserPtr;

public:
    C3DParser();
    virtual ~C3DParser();

public:
    //! Parsowanie pliku c3d
    //! \param path poprawna �cie�ka do pliku
    virtual void parseFile(const core::Filesystem::Path& path);
    //! \return pusty obiekt nowego parsera
    virtual core::IParser* create();
    //! Zwraca obiekty dostarczone przez parser
    //! \param objects kolekcja z obiektami (set)
    virtual void getObjects(core::Objects& objects);
    //! Zwraca rozszerzenia, kt�re s� obs�ugiwane przez parser (tylko asf)
    //! \param extensions kolecja z roszerzeniami
    virtual void getSupportedExtensions(Extensions & extensions) const;
	//! Zapisauje dane w konkretnym miejscu, nie u�ywane
	//! \param path �cie�ka zapisu
	void saveFile(const core::Filesystem::Path& path);
};


#endif