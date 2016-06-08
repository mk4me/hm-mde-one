/********************************************************************
	created:  2014/07/04	10:12:59
	filename: IHMDBRemoteContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__IHMDBREMOTECONTEXT_H__
#define __HEADER_GUARD_hmdbCommunication__IHMDBREMOTECONTEXT_H__

#include <utils/SmartPtr.h>
#include <hmdbserviceslib/ShallowCopy.h>
#include <threadingUtils/IOperation.h>
#include <threadingUtils/IProgress.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBFtp.h>


namespace hmdbCommunication
{
	//! Interfejs opisuj�cy kontekst bazy danych i mo�lwie operacje na niej
	class IHMDBRemoteContext
	{
	public:

		//! Typ pochodzenia danych (plik�w)
		enum DataReference
		{
			Motion,	//! Dane ruchu
			Medical	//! Dane medyczne
		};

		struct CompoundID
		{
			DataReference dataReference;	//! Pochodzenie danych
			hmdbServices::ID fileID;		//! Identyfikator pliku
		};
		
		//! Struktura opisuj�ca pochodzenie danych - czy dane medyczne/ruchu i ID pliku
		struct FileDescriptor
		{
			CompoundID id;					//! Identyfikator pliku
			std::string fileName;			//! Nazwa pliku
			hmdbServices::FileSize fileSize;//! Rozmiar pliku

			bool operator<(const FileDescriptor a) const
			{
				return fileName < a.fileName;
			}
		};

		//! Interfejs operacji
		class IOperation : public threadingUtils::IAbortableOperation,
			public threadingUtils::INormalizedProgress
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperation() {}
		};

		//! Wska�nik dla operacji
		typedef utils::shared_ptr<IOperation> OperationPtr;
		//! Wska�nik dla operacji
		typedef utils::shared_ptr<const IOperation> OperationConstPtr;

		//! Interfejs opisuj�cy upload danych, dostarcza informacji o identyfikatorze pliku po zapisie w bazie
		class IUploadOperation : public IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~IUploadOperation() {}
			//! \return Identyfikator pliku po zapisie w bazie
			virtual hmdbServices::ID fileID() const = 0;
		};

		//! Wska�nik dla operacji uploadu danych
		typedef utils::shared_ptr<IUploadOperation> UploadOperationPtr;
		//! Wska�nik dla operacji uploadu danych
		typedef utils::shared_ptr<const IUploadOperation> UploadOperationConstPtr;

		//! Interfejs opisuj�cy upload danych, dostarcza informacji o identyfikatorze pliku po zapisie w bazie
		class IDownloadOperation : public IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~IDownloadOperation() {}
			//! \return Identyfikator �ci�ganego pliku
			virtual FileDescriptor fileID() const = 0;
			//! \return Czy plik jest dostepny w storage (�ci�gni�to i poprawnie zapisano, cho� reszta operacji mog�a p�js� nie tak)
			virtual bool fileDownloaded() const = 0;
			//! \return Strumie� �ci�gni�tych danych
			virtual IHMDBStorageOperations::IStreamPtr stream() const = 0;
			//! Metoda czy�ci zapisane dane
			virtual void release() = 0;
		};

		//! Wska�nik dla operacji uploadu danych
		typedef utils::shared_ptr<IDownloadOperation> DownloadOperationPtr;
		//! Wska�nik dla operacji uploadu danych
		typedef utils::shared_ptr<const IDownloadOperation> DownloadOperationConstPtr;

	public:
		
		//! Destruktor wirtualny
		virtual ~IHMDBRemoteContext() {}

		//---------- Opis kontekstu zdalnego ---------------

		//! \return Sesja us�ug hmdb
		virtual IHMDBSessionPtr session() = 0;
		//! \return Sesja us�ug hmdb
		virtual IHMDBSessionConstPtr session() const = 0;

		//---------- Operacje kontekstu -----------------

		//! \return Operacja opisuj�ca od�wie�enia p�ytkiej kopii bazy danych
		virtual std::list<DownloadOperationPtr> prepareSynchronization(const hmdbServices::DateTime & since = hmdbServices::DateTime::now()) = 0;

		//! \param fd Opis pliku do �ci�gni�cia
		//! \return Transfer danych
		virtual DownloadOperationPtr prepareFileDownload(const FileDescriptor & fd) = 0;

		//! \param fileName Nazwa pliku w bazie
		//! \param path �cie�ka po stronie bazy, gdzie zostanie zpaisany plik
		//! \param source Strumie� z zawarto�ci� pliku
		//! \param dataReference Kt�rej bazy dotyczy upload
		//! \param fileSize Rozmiar strumienia kt�ry chcemy uploadowa�
		//! \return Transfer
		virtual IHMDBFtp::TransferPtr prepareFileUpload(const std::string & fileName,
			const std::string & path, IHMDBStorageOperations::IStreamPtr source,
			const DataReference dataReference,
			const hmdbServices::FileSize fileSize = 0) = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBRemoteContext);
}

#endif	// __HEADER_GUARD_hmdbCommunication__IHMDBREMOTECONTEXT_H__
