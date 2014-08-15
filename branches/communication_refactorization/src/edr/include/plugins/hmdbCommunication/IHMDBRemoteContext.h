/********************************************************************
	created:  2014/07/04	10:12:59
	filename: IHMDBRemoteContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBREMOTECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBREMOTECONTEXT_H__

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
		
		//! Struktura opisuj�ca pochodzenie danych - czy dane medyczne/ruchu i ID pliku
		struct CompoundID
		{
			DataReference dataReference;	//! Pochodzenie danych
			hmdbServices::ID fileID;		//! Identyfikator pliku
			std::string fileName;			//! Nazwa pliku
			hmdbServices::FileSize fileSize;//! Rozmiar pliku

			bool operator<(const CompoundID a) const
			{
				return fileName < a.fileName;
			}
		};

		//! Interfejs operacji
		class IOperation : public threadingUtils::IAbortableOperation,
			public threadingUtils::IProgress
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
			virtual const hmdbServices::ID fileID() const = 0;
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
			virtual const CompoundID fileID() const = 0;
			//! \return Czy plik jest dostepny w storage (�ci�gni�to i poprawnie zapisano, cho� reszta operacji mog�a p�js� nie tak)
			virtual const bool fileDownloaded() const = 0;
			//! \return Storage
			virtual const IHMDBStoragePtr storage() = 0;
			//! \return Storage
			virtual const IHMDBStorageConstPtr storage() const = 0;
		};

		//! Wska�nik dla operacji uploadu danych
		typedef utils::shared_ptr<IDownloadOperation> DownloadOperationPtr;
		//! Wska�nik dla operacji uploadu danych
		typedef utils::shared_ptr<const IDownloadOperation> DownloadOperationConstPtr;

		//! Interfejs realizuj�cy operacj� synchronizacji perspektywy danych
		class ISynchronizeOperation : public IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~ISynchronizeOperation() {}
			//! \return Pe�na p�ytka kopia bazy danych - ca�a perspektywa dostepnych danych
			virtual const ShallowCopyConstPtr shallowCopy() const = 0;
			//! \return Pe�na p�ytka kopia bazy danych - ca�a perspektywa dostepnych danych
			virtual const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const = 0;
		};

		//! Wska�nik dla operacji synchronizacji
		typedef utils::shared_ptr<ISynchronizeOperation> SynchronizeOperationPtr;
		//! Wska�nik dla operacji synchronizacji
		typedef utils::shared_ptr<const ISynchronizeOperation> SynchronizeOperationConstPtr;

	public:
		
		//! Destruktor wirtualny
		virtual ~IHMDBRemoteContext() {}

		//---------- Opis kontekstu zdalnego ---------------

		//! \return Sesja us�ug hmdb
		virtual const IHMDBSessionPtr session() = 0;
		//! \return Sesja us�ug hmdb
		virtual const IHMDBSessionConstPtr session() const = 0;

		//---------- Operacje kontekstu -----------------

		//! \return Operacja opisuj�ca od�wie�enia p�ytkiej kopii bazy danych
		virtual const SynchronizeOperationPtr prepareSynchronization(IHMDBStoragePtr storage = IHMDBStoragePtr()) = 0;

		//! \param fileIDs Identyfikatory plik�w do �ci�gni�cia
		//! \param ep Polityka wykonania
		//! \return Transfer danych
		virtual const DownloadOperationPtr prepareFileDownload(const CompoundID & fileID,
			IHMDBStoragePtr storage) = 0;

		//! \param fileName Nazwa pliku w bazie
		//! \param path �cie�ka po stronie bazy, gdzie zostanie zpaisany plik
		//! \param source Strumie� z zawarto�ci� pliku
		//! \param dataReference Kt�rej bazy dotyczy upload
		//! \return Transfer
		virtual const IHMDBFtp::TransferPtr prepareFileUpload(const std::string & fileName,
			const std::string & path, IHMDBStorage::IStreamPtr source,
			const DataReference dataReference) = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBRemoteContext);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBREMOTECONTEXT_H__