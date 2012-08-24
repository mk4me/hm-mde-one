/********************************************************************
    created:  2012/02/17
    created:  17:2:2012   10:42
    filename: DownloadRequest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DOWNLOADREQUEST_H__
#define HEADER_GUARD___DOWNLOADREQUEST_H__

#include <map>
#include <plugins/newCommunication/IDownloadRequest.h>
#include <plugins/newCommunication/CommunicationManager.h>
#include <core/Filesystem.h>
#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/ScopedLock>

//! Kalsa wpieraj¹ca pobieranie plików z bazy danych
class DownloadRequest : public communication::IDownloadRequest
{

private:

    //! Typ mutexa
    typedef OpenThreads::ReentrantMutex ReentrantMutex;
    //! Typ lokalnego blokowania mutexa
    typedef OpenThreads::ScopedLock<ReentrantMutex> ScopedLock;

public:
    //! Struktura opisuj¹ca pliki metadanych
    struct ShallowCopyDescriptor
    {
        //! Œcie¿ka zapisu pliku
        std::string destPath;
        //! Rozmiar pliku
        long size;
    };

    //! Struktura opisuj¹ca plik
    struct FileDescriptor : public ShallowCopyDescriptor
    {
        //! Identyfikator pliku
        int id;
    };



private:
    //! Prywatny konstruktor
    DownloadRequest();

public:

    //! \param filesToDownload Mapa identyfikatorów plików i deskryptorów do œci¹gniêcia
    //! \return zlecenie transferu plików
    static DownloadRequest * createFilesRequest(const std::map<int, FileDescriptor> & filesToDownload);
    //! \param photosToDownload Mapa identyfikatorów zdjêæ i deskryptorów do œci¹gniêcia
    //! \return zlecenie transferu zdjêæ
    static DownloadRequest * createPhotosRequest(const std::map<int, FileDescriptor> & photosToDownload);

    //! \param filesToDownload Mapa identyfikatorów plików i deskryptorów do œci¹gniêcia
    //! \param photosToDownload Mapa identyfikatorów zdjêæ i deskryptorów do œci¹gniêcia
    //! \return Zlecenie transferu
    static DownloadRequest * createFilesAndPhotosRequest(const std::map<int, FileDescriptor> & filesToDownload,
        const std::map<int, FileDescriptor> & photosToDownload);

    //! \param Mapa typów requestów wraz z ich deskryptorami
    //! \return Zlecenie transferu
    static DownloadRequest * createShallowCopyRequest(const std::map<CommunicationManager::Request, ShallowCopyDescriptor> & shallowCopiesToDownload);

    //! Wirtualny destruktor
    virtual ~DownloadRequest();

    //! \return Aktualny status zlecenia
    virtual const State state() const;

    //! \return Informacja o b³êdzie
    virtual const std::string & error() const;

    //! \return Zwraca request w³aœciwy dla CommunicationManagera
    const CommunicationManager::ComplexRequestPtr & request() const;

    //! Anuluje zlecenie
    virtual void cancel();

	//! Rozpocznij zlecenie
	virtual void start();

    //! \return Ca³kowity postêp œci¹gania w przedziale [0-100]
    virtual const float totalProgress() const;
    //! \return Postêp œci¹gania aktualnego pliku
    virtual const float currentFileProgress() const;
	//! \return Nazwa aktualnie pobieranego pliku
	virtual std::string currentFileName() const;
	//! \return Œcie¿ka zapisu aktualnie pobieranego pliku
	std::string currentFilePath() const;

    //! \return Ca³kowita iloœæ danych do œci¹gniêcia w bajtach
    virtual const long totalSize() const;
    //! \return Rozmiar aktualnego pliku
    virtual const long currentFileSize() const;

    //! \return Sumaryczna iloœc plików do œci¹gniêcia
    virtual const int totalFilesToDownload() const;
    //! \retrurn Iloœc plików ju¿ œci¹gniêtych
    virtual const int filesDownloaded() const;
    //! \return Aktualnie œci¹gany plik (numeracja nie ID)
    virtual const int currentFile() const;

private:
    //! \param request Zadanie dla kotrego chcemy pobraæ rozmiar œci¹ganego pliku
    //! \return Rozmiar pliku zwi¹zanego z zadaniem
    const long getRequestSize(const CommunicationManager::BasicRequestPtr & request) const;

    //! \param fileID Identyfikator pliku do œci¹gniêcia
    //! \param destPath Œcie¿ka zapisu pliku
    //! \param fileSize Wielkoœæ pliku w bajtach
    //! \param callbacks Callbacki dla zmiany statusu requestów
    void addFileRequest(int fileID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());
    //! \param fileID Identyfikator zdjêcia do œci¹gniêcia
    //! \param destPath Œcie¿ka zapisu zdjêcia
    //! \param fileSize Wielkoœæ zdjêcia w bajtach
    //! \param callbacks Callbacki dla zmiany statusu requestów
    void addPhotoRequest(int photoID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());
    //! \param metadataType Identyfikator requesta: musi byæ jedna z wartoœci [CopyMotionShallowCopy, CopyMotionMetadata, CopyMedicalShallowCopy, CopyMedicalMetadata]
    //! \param destPath Œcie¿ka zapisu metadanych
    //! \param fileSize Wielkoœæ pliku w bajtach
    //! \param callbacks Callbacki dla zmiany statusu requestów
    void addMetadataRequest(CommunicationManager::Request metadataType, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());

    //! ------------------------- Metody ustaiwaj¹ stan zlecenia i dane konieczne do obs³ugi postêpu -------------------------------------

    //! Metoda wywo³ywana kiedy zaczynamy obs³ugiwaæ ca³e zlecenie
    //! \param request Zlecenie którego dotyczy callback
    void onFullDownloadStart(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo³ywana kiedy koñczymy pomyœlnie obs³ugiwaæ ca³e zlecenie
    //! \param request Zlecenie którego dotyczy callback
    void onFullDownloadComplete(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo³ywana kiedy pojedyczny plik rozpoczyna œci¹ganie
    //! \param request Zlecenie którego dotyczy callback
    void onDownloadBegin(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo³ywana kiedy zadanie jest anulowane
    //! \param request Zlecenie którego dotyczy callback
    void onDownloadCancel(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo³ywana kiedy pojedyczny plik zostanie œci¹gniêty
    //! \param request Zlecenie którego dotyczy callback
    void onDownloadComplete(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo³ywana kiedy zadanie koñczy siê b³êdem
    //! \param request Zlecenie którego dotyczy callback
    void onDownloadError(const CommunicationManager::BasicRequestPtr & request, const std::string & error);

private:
    //! Aktualny stan zlecenia
    State state_;
    //! Komunikat b³êdu
    std::string error_;
    //! W³aœciwe zlecenie CommunicationManagera
    CommunicationManager::ComplexRequestPtr request_;
    //! Aktualny pojedynczy transfer
    CommunicationManager::BasicRequestPtr currentTransfer;
    //! Numer aktualnie œci¹ganego pliku
    int currentTransferID;
    //! Rozmiar ca³kowity do œci¹gniêcia
    long totalSize_;
    //! Rozmiar aktualnie œci¹ganego pliku
    long currentSize;
    //! Zbiór callbacków realizuj¹cych obs³ugê stanu zlecenia pojedynczego œci¹gania
    CommunicationManager::RequestCallbacks singleTransferCallbacks;

    //! Zbiór callbacków realizuj¹cych obs³ugê stanu ca³ego pakietu zleceñ
    CommunicationManager::RequestCallbacks complexTransferCallbacks;

    //! Mapa plików do œci¹gniêcia wraz z ich rozmiarem
    std::map<int, long> filesSizes;
    //! Mapa zdjêæ do œci¹gniêcia wraz z ich rozmiarem
    std::map<int, long> photosSizes;
    //! Mapa plików p³ytkiej kopii do œci¹gniêcia wraz z ich rozmiarem
    std::map<CommunicationManager::Request, long> shallowSizes;
    //! Mutex synchrnizuj¹cy operacje na obiekcie
    mutable ReentrantMutex stateMutex;
	//! Mutex synchrnizuj¹cy operacje na obiekcie
	mutable ReentrantMutex cancelMutex;
};

#endif	//	HEADER_GUARD___DOWNLOADREQUEST_H__
