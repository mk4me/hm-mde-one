/********************************************************************
	created:  2012/02/17
	created:  17:2:2012   10:42
	filename: DownloadRequest.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DOWNLOADREQUEST_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DOWNLOADREQUEST_H__

#include <map>
#include <plugins/hmdbCommunication/IDownloadRequest.h>
#include "CommunicationManager.h"
#include <corelib/Filesystem.h>
#include <threadingUtils/SynchronizationPolicies.h>

//! Kalsa wpierająca pobieranie plików z bazy danych
class DownloadRequest : public hmdbCommunication::IDownloadRequest
{
private:

	//! Typ mutexa
	typedef utils::RecursiveSyncPolicy ReentrantMutex;
	//! Typ lokalnego blokowania mutexa
	typedef utils::ScopedLock<ReentrantMutex> ScopedLock;

public:
	//! Struktura opisująca pliki metadanych
	struct ShallowCopyDescriptor
	{
		//! Ścieżka zapisu pliku
		std::string destPath;
		//! Rozmiar pliku
		long size;
	};

	//! Struktura opisująca plik
	struct FileDescriptor : public ShallowCopyDescriptor
	{
		//! Identyfikator pliku
		int id;
	};

private:
	//! Prywatny konstruktor
	DownloadRequest(CommunicationManager * cm);

public:

	//! \param filesToDownload Mapa identyfikatorów plików i deskryptorów do ściągnięcia
	//! \return zlecenie transferu plików
	static DownloadRequest * createFilesRequest(const std::map<int, FileDescriptor> & filesToDownload,
		CommunicationManager * cm);
	//! \param photosToDownload Mapa identyfikatorów zdjęć i deskryptorów do ściągnięcia
	//! \return zlecenie transferu zdjęć
	static DownloadRequest * createPhotosRequest(const std::map<int, FileDescriptor> & photosToDownload,
		CommunicationManager * cm);

	//! \param filesToDownload Mapa identyfikatorów plików i deskryptorów do ściągnięcia
	//! \param photosToDownload Mapa identyfikatorów zdjęć i deskryptorów do ściągnięcia
	//! \return Zlecenie transferu
	static DownloadRequest * createFilesAndPhotosRequest(const std::map<int, FileDescriptor> & filesToDownload,
		const std::map<int, FileDescriptor> & photosToDownload, CommunicationManager * cm);

	//! \param Mapa typów requestów wraz z ich deskryptorami
	//! \return Zlecenie transferu
	static DownloadRequest * createShallowCopyRequest(const std::map<CommunicationManager::Request,
		ShallowCopyDescriptor> & shallowCopiesToDownload, CommunicationManager * cm);

	//! Wirtualny destruktor
	virtual ~DownloadRequest();

	//! \return Aktualny status zlecenia
	virtual const State state() const;

	//! \return Informacja o błędzie
	virtual const std::string & error() const;

	//! \return Zwraca request właściwy dla CommunicationManagera
	const CommunicationManager::ComplexRequestPtr & request() const;

	//! Anuluje zlecenie
	virtual void cancel();

	//! Rozpocznij zlecenie
	virtual void start();

	//! \return Całkowity postęp ściągania w przedziale [0-100]
	virtual const float totalProgress() const;
	//! \return Postęp ściągania aktualnego pliku
	virtual const float currentFileProgress() const;
	//! \return Nazwa aktualnie pobieranego pliku
	virtual std::string currentFileName() const;
	//! \return Ścieżka zapisu aktualnie pobieranego pliku
	std::string currentFilePath() const;

	//! \return Całkowita ilość danych do ściągnięcia w bajtach
	virtual const long totalSize() const;
	//! \return Rozmiar aktualnego pliku
	virtual const long currentFileSize() const;

	//! \return Sumaryczna ilośc plików do ściągnięcia
	virtual const int totalFilesToDownload() const;
	//! \retrurn Ilośc plików już ściągniętych
	virtual const int filesDownloaded() const;
	//! \return Aktualnie ściągany plik (numeracja nie ID)
	virtual const int currentFile() const;

private:
	//! \param request Zadanie dla kotrego chcemy pobrać rozmiar ściąganego pliku
	//! \return Rozmiar pliku związanego z zadaniem
	const long getRequestSize(const CommunicationManager::BasicRequestPtr & request) const;

	//! \param fileID Identyfikator pliku do ściągnięcia
	//! \param destPath Ścieżka zapisu pliku
	//! \param fileSize Wielkość pliku w bajtach
	//! \param callbacks Callbacki dla zmiany statusu requestów
	void addFileRequest(int fileID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());
	//! \param fileID Identyfikator zdjęcia do ściągnięcia
	//! \param destPath Ścieżka zapisu zdjęcia
	//! \param fileSize Wielkość zdjęcia w bajtach
	//! \param callbacks Callbacki dla zmiany statusu requestów
	void addPhotoRequest(int photoID, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());
	//! \param metadataType Identyfikator requesta: musi być jedna z wartości [CopyMotionShallowCopy, CopyMotionMetadata, CopyMedicalShallowCopy, CopyMedicalMetadata]
	//! \param destPath Ścieżka zapisu metadanych
	//! \param fileSize Wielkość pliku w bajtach
	//! \param callbacks Callbacki dla zmiany statusu requestów
	void addMetadataRequest(CommunicationManager::Request metadataType, const std::string & destPath, long fileSize, const CommunicationManager::RequestCallbacks & callbacks = CommunicationManager::RequestCallbacks());

	//! ------------------------- Metody ustaiwają stan zlecenia i dane konieczne do obsługi postępu -------------------------------------

	//! Metoda wywoływana kiedy zaczynamy obsługiwać całe zlecenie
	//! \param request Zlecenie którego dotyczy callback
	void onFullDownloadStart(const CommunicationManager::BasicRequestPtr & request);

	//! Metoda wywoływana kiedy kończymy pomyślnie obsługiwać całe zlecenie
	//! \param request Zlecenie którego dotyczy callback
	void onFullDownloadComplete(const CommunicationManager::BasicRequestPtr & request);

	//! Metoda wywoływana kiedy pojedyczny plik rozpoczyna ściąganie
	//! \param request Zlecenie którego dotyczy callback
	void onDownloadBegin(const CommunicationManager::BasicRequestPtr & request);

	//! Metoda wywoływana kiedy zadanie jest anulowane
	//! \param request Zlecenie którego dotyczy callback
	void onDownloadCancel(const CommunicationManager::BasicRequestPtr & request);

	//! Metoda wywoływana kiedy pojedyczny plik zostanie ściągnięty
	//! \param request Zlecenie którego dotyczy callback
	void onDownloadComplete(const CommunicationManager::BasicRequestPtr & request);

	//! Metoda wywoływana kiedy zadanie kończy się błędem
	//! \param request Zlecenie którego dotyczy callback
	void onDownloadError(const CommunicationManager::BasicRequestPtr & request, const std::string & error);

private:
	//! Aktualny stan zlecenia
	State state_;
	//! Komunikat błędu
	std::string error_;
	//! Właściwe zlecenie CommunicationManagera
	CommunicationManager::ComplexRequestPtr request_;
	//! Aktualny pojedynczy transfer
	CommunicationManager::BasicRequestPtr currentTransfer;
	//! Numer aktualnie ściąganego pliku
	int currentTransferID;
	//! Rozmiar całkowity do ściągnięcia
	long totalSize_;
	//! Rozmiar aktualnie ściąganego pliku
	long currentSize;
	//! Zbiór callbacków realizujących obsługę stanu zlecenia pojedynczego ściągania
	CommunicationManager::RequestCallbacks singleTransferCallbacks;

	//! Zbiór callbacków realizujących obsługę stanu całego pakietu zleceń
	CommunicationManager::RequestCallbacks complexTransferCallbacks;

	//! Mapa plików do ściągnięcia wraz z ich rozmiarem
	std::map<int, long> filesSizes;
	//! Mapa zdjęć do ściągnięcia wraz z ich rozmiarem
	std::map<int, long> photosSizes;
	//! Mapa plików płytkiej kopii do ściągnięcia wraz z ich rozmiarem
	std::map<CommunicationManager::Request, long> shallowSizes;
	//! Mutex synchrnizujący operacje na obiekcie
	mutable ReentrantMutex stateMutex;
	//! Mutex synchrnizujący operacje na obiekcie
	mutable ReentrantMutex cancelMutex;
	//! CommunicationManager
	CommunicationManager * cm;
};

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DOWNLOADREQUEST_H__
