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

//! Interfejs dostarczaj¹cy informacji o postepie œci¹gania plików
class IDownloadRequest : public utils::Observable<IDownloadRequest>
{
public:

    //! Stan zlecenia
    enum State {
		Created,		//! Zadanie stworzono, nie dodano do przetwarzania
        Pending,        //! Dodano do przetwarzania - Oczekuje na rozpoczêcie
        Started,        //! Rozpoczêto
        SingleFinished, //! Zakoñczono pojedynczy transfer
        FinishedOK,     //! Zakoñczono powodzeniem
        FinishedCancel, //! Zakoñczono - przerwano
        FinishedError   //! Zakoñczono z b³edêm
    };

    //! Wirtualny destruktor
    virtual ~IDownloadRequest() {}

    //! \return Aktualny status zlecenia
    virtual const State state() const = 0;

    //! \return Informacja o b³êdzie
    virtual const std::string & error() const = 0;

    //! Anyluje zlecenie
    virtual void cancel() = 0;

	//! Rozpocznij zlecenie
	virtual void start() = 0;

    //! \return Ca³kowity postêp œci¹gania w przedziale [0-100]
    virtual const float totalProgress() const = 0;
    //! \return Postêp œci¹gania aktualnego pliku
    virtual const float currentFileProgress() const = 0;
	//! \return Nazwa aktualnie pobieranego pliku
	virtual std::string currentFileName() const = 0;

    //! \return Ca³kowita iloœæ danych do œci¹gniêcia w bajtach
    virtual const long totalSize() const = 0;
    //! \return Rozmiar aktualnego pliku
    virtual const long currentFileSize() const = 0;

    //! \return Sumaryczna iloœc plików do œci¹gniêcia
    virtual const int totalFilesToDownload() const = 0;
    //! \retrurn Iloœc plików ju¿ œci¹gniêtych
    virtual const int filesDownloaded() const = 0;
    //! \return Aktualnie œci¹gany plik (numeracja nie ID)
    virtual const int currentFile() const = 0;
};

typedef core::shared_ptr<IDownloadRequest> DownloadRequestPtr;
typedef core::shared_ptr<const IDownloadRequest> DownloadRequestConstPtr;

}

#endif	//	HEADER_GUARD___IDOWNLOADREQUEST_H__
