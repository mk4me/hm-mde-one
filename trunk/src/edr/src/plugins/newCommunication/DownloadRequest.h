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

//! Kalsa wpieraj�ca pobieranie plik�w z bazy danych
class DownloadRequest : public communication::IDownloadRequest
{

private:

    //! Typ mutexa
    typedef OpenThreads::ReentrantMutex ReentrantMutex;
    //! Typ lokalnego blokowania mutexa
    typedef OpenThreads::ScopedLock<ReentrantMutex> ScopedLock;

public:
    //! Struktura opisuj�ca pliki metadanych
    struct ShallowCopyDescriptor
    {
        //! �cie�ka zapisu pliku
        std::string destPath;
        //! Rozmiar pliku
        long size;
    };

    //! Struktura opisuj�ca plik
    struct FileDescriptor : public ShallowCopyDescriptor
    {
        //! Identyfikator pliku
        int id;
    };



private:
    //! Prywatny konstruktor
    DownloadRequest();

public:

    //! \param filesToDownload Mapa identyfikator�w plik�w i deskryptor�w do �ci�gni�cia
    //! \return zlecenie transferu plik�w
    static DownloadRequest * createFilesRequest(const std::map<int, FileDescriptor> & filesToDownload);
    //! \param photosToDownload Mapa identyfikator�w zdj�� i deskryptor�w do �ci�gni�cia
    //! \return zlecenie transferu zdj��
    static DownloadRequest * createPhotosRequest(const std::map<int, FileDescriptor> & photosToDownload);

    //! \param filesToDownload Mapa identyfikator�w plik�w i deskryptor�w do �ci�gni�cia
    //! \param photosToDownload Mapa identyfikator�w zdj�� i deskryptor�w do �ci�gni�cia
    //! \return Zlecenie transferu
    static DownloadRequest * createFilesAndPhotosRequest(const std::map<int, FileDescriptor> & filesToDownload,
        const std::map<int, FileDescriptor> & photosToDownload);

    //! \param Mapa typ�w request�w wraz z ich deskryptorami
    //! \return Zlecenie transferu
    static DownloadRequest * createShallowCopyRequest(const std::map<CommunicationManager::Request, ShallowCopyDescriptor> & shallowCopiesToDownload);

    //! Wirtualny destruktor
    virtual ~DownloadRequest();

    //! \return Aktualny status zlecenia
    virtual const State state() const;

    //! \return Informacja o b��dzie
    virtual const std::string & error() const;

    //! \return Zwraca request w�a�ciwy dla CommunicationManagera
    const CommunicationManager::ComplexRequestPtr & request() const;

    //! Anuluje zlecenie
    virtual void cancel();

	//! Rozpocznij zlecenie
	virtual void start();

    //! \return Ca�kowity post�p �ci�gania w przedziale [0-100]
    virtual const float totalProgress() const;
    //! \return Post�p �ci�gania aktualnego pliku
    virtual const float currentFileProgress() const;
	//! \return Nazwa aktualnie pobieranego pliku
	virtual std::string currentFileName() const;
	//! \return �cie�ka zapisu aktualnie pobieranego pliku
	std::string currentFilePath() const;

    //! \return Ca�kowita ilo�� danych do �ci�gni�cia w bajtach
    virtual const long totalSize() const;
    //! \return Rozmiar aktualnego pliku
    virtual const long currentFileSize() const;

    //! \return Sumaryczna ilo�c plik�w do �ci�gni�cia
    virtual const int totalFilesToDownload() const;
    //! \retrurn Ilo�c plik�w ju� �ci�gni�tych
    virtual const int filesDownloaded() const;
    //! \return Aktualnie �ci�gany plik (numeracja nie ID)
    virtual const int currentFile() const;

private:
    //! \param request Zadanie dla kotrego chcemy pobra� rozmiar �ci�ganego pliku
    //! \return Rozmiar pliku zwi�zanego z zadaniem
    const long getRequestSize(const CommunicationManager::BasicRequestPtr & request) const;

    //! \param fileID Identyfikator pliku do �ci�gni�cia
    //! \param destPath �cie�ka zapisu pliku
    //! \param fileSize Wielko�� pliku w bajtach
    //! \param callbacks Callbacki dla zmiany statusu request�w
    void addFileRequest(int fileID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());
    //! \param fileID Identyfikator zdj�cia do �ci�gni�cia
    //! \param destPath �cie�ka zapisu zdj�cia
    //! \param fileSize Wielko�� zdj�cia w bajtach
    //! \param callbacks Callbacki dla zmiany statusu request�w
    void addPhotoRequest(int photoID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());
    //! \param metadataType Identyfikator requesta: musi by� jedna z warto�ci [CopyMotionShallowCopy, CopyMotionMetadata, CopyMedicalShallowCopy, CopyMedicalMetadata]
    //! \param destPath �cie�ka zapisu metadanych
    //! \param fileSize Wielko�� pliku w bajtach
    //! \param callbacks Callbacki dla zmiany statusu request�w
    void addMetadataRequest(CommunicationManager::Request metadataType, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());

    //! ------------------------- Metody ustaiwaj� stan zlecenia i dane konieczne do obs�ugi post�pu -------------------------------------

    //! Metoda wywo�ywana kiedy zaczynamy obs�ugiwa� ca�e zlecenie
    //! \param request Zlecenie kt�rego dotyczy callback
    void onFullDownloadStart(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo�ywana kiedy ko�czymy pomy�lnie obs�ugiwa� ca�e zlecenie
    //! \param request Zlecenie kt�rego dotyczy callback
    void onFullDownloadComplete(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo�ywana kiedy pojedyczny plik rozpoczyna �ci�ganie
    //! \param request Zlecenie kt�rego dotyczy callback
    void onDownloadBegin(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo�ywana kiedy zadanie jest anulowane
    //! \param request Zlecenie kt�rego dotyczy callback
    void onDownloadCancel(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo�ywana kiedy pojedyczny plik zostanie �ci�gni�ty
    //! \param request Zlecenie kt�rego dotyczy callback
    void onDownloadComplete(const CommunicationManager::BasicRequestPtr & request);

    //! Metoda wywo�ywana kiedy zadanie ko�czy si� b��dem
    //! \param request Zlecenie kt�rego dotyczy callback
    void onDownloadError(const CommunicationManager::BasicRequestPtr & request, const std::string & error);

private:
    //! Aktualny stan zlecenia
    State state_;
    //! Komunikat b��du
    std::string error_;
    //! W�a�ciwe zlecenie CommunicationManagera
    CommunicationManager::ComplexRequestPtr request_;
    //! Aktualny pojedynczy transfer
    CommunicationManager::BasicRequestPtr currentTransfer;
    //! Numer aktualnie �ci�ganego pliku
    int currentTransferID;
    //! Rozmiar ca�kowity do �ci�gni�cia
    long totalSize_;
    //! Rozmiar aktualnie �ci�ganego pliku
    long currentSize;
    //! Zbi�r callback�w realizuj�cych obs�ug� stanu zlecenia pojedynczego �ci�gania
    CommunicationManager::RequestCallbacks singleTransferCallbacks;

    //! Zbi�r callback�w realizuj�cych obs�ug� stanu ca�ego pakietu zlece�
    CommunicationManager::RequestCallbacks complexTransferCallbacks;

    //! Mapa plik�w do �ci�gni�cia wraz z ich rozmiarem
    std::map<int, long> filesSizes;
    //! Mapa zdj�� do �ci�gni�cia wraz z ich rozmiarem
    std::map<int, long> photosSizes;
    //! Mapa plik�w p�ytkiej kopii do �ci�gni�cia wraz z ich rozmiarem
    std::map<CommunicationManager::Request, long> shallowSizes;
    //! Mutex synchrnizuj�cy operacje na obiekcie
    mutable ReentrantMutex stateMutex;
	//! Mutex synchrnizuj�cy operacje na obiekcie
	mutable ReentrantMutex cancelMutex;
};

#endif	//	HEADER_GUARD___DOWNLOADREQUEST_H__
