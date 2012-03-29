/********************************************************************
    created:  2012/02/17
    created:  17:2:2012   10:25
    filename: IDownloadRequest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDOWNLOADREQUEST_H__
#define HEADER_GUARD___IDOWNLOADREQUEST_H__

#include <string>
#include <utils/ObserverPattern.h>
#include <core/SmartPtr.h>

namespace communication
{

//! Interfejs dostarczaj�cy informacji o postepie �ci�gania plik�w
class IDownloadRequest : public utils::Observable<IDownloadRequest>
{
public:

    //! Stan zlecenia
    enum State {
		Created,		//! Zadanie stworzono, nie dodano do przetwarzania
        Pending,        //! Dodano do przetwarzania - Oczekuje na rozpocz�cie
        Started,        //! Rozpocz�to
        SingleFinished, //! Zako�czono pojedynczy transfer
        FinishedOK,     //! Zako�czono powodzeniem
        FinishedCancel, //! Zako�czono - przerwano
        FinishedError   //! Zako�czono z b�ed�m
    };

    //! Wirtualny destruktor
    virtual ~IDownloadRequest() {}

    //! \return Aktualny status zlecenia
    virtual const State state() const = 0;

    //! \return Informacja o b��dzie
    virtual const std::string & error() const = 0;

    //! Anyluje zlecenie
    virtual void cancel() = 0;

	//! Rozpocznij zlecenie
	virtual void start() = 0;

    //! \return Ca�kowity post�p �ci�gania w przedziale [0-100]
    virtual const float totalProgress() const = 0;
    //! \return Post�p �ci�gania aktualnego pliku
    virtual const float currentFileProgress() const = 0;
	//! \return Nazwa aktualnie pobieranego pliku
	virtual std::string currentFileName() const = 0;

    //! \return Ca�kowita ilo�� danych do �ci�gni�cia w bajtach
    virtual const long totalSize() const = 0;
    //! \return Rozmiar aktualnego pliku
    virtual const long currentFileSize() const = 0;

    //! \return Sumaryczna ilo�c plik�w do �ci�gni�cia
    virtual const int totalFilesToDownload() const = 0;
    //! \retrurn Ilo�c plik�w ju� �ci�gni�tych
    virtual const int filesDownloaded() const = 0;
    //! \return Aktualnie �ci�gany plik (numeracja nie ID)
    virtual const int currentFile() const = 0;
};

typedef core::shared_ptr<IDownloadRequest> DownloadRequestPtr;
typedef core::shared_ptr<const IDownloadRequest> DownloadRequestConstPtr;

}

#endif	//	HEADER_GUARD___IDOWNLOADREQUEST_H__
