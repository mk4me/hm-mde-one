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
	//! Interfejs opisuj¹cy kontekst bazy danych i mo¿lwie operacje na niej
	class IHMDBRemoteContext
	{
	public:

		//! Typ pochodzenia danych (plików)
		enum DataReference
		{
			Motion,	//! Dane ruchu
			Medical	//! Dane medyczne
		};
		
		//! Struktura opisuj¹ca pochodzenie danych - czy dane medyczne/ruchu i ID pliku
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

		//! WskaŸnik dla operacji
		typedef utils::shared_ptr<IOperation> OperationPtr;
		//! WskaŸnik dla operacji
		typedef utils::shared_ptr<const IOperation> OperationConstPtr;

		//! Interfejs opisuj¹cy upload danych, dostarcza informacji o identyfikatorze pliku po zapisie w bazie
		class IUploadOperation : public IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~IUploadOperation() {}
			//! \return Identyfikator pliku po zapisie w bazie
			virtual const hmdbServices::ID fileID() const = 0;
		};

		//! WskaŸnik dla operacji uploadu danych
		typedef utils::shared_ptr<IUploadOperation> UploadOperationPtr;
		//! WskaŸnik dla operacji uploadu danych
		typedef utils::shared_ptr<const IUploadOperation> UploadOperationConstPtr;

		//! Interfejs opisuj¹cy upload danych, dostarcza informacji o identyfikatorze pliku po zapisie w bazie
		class IDownloadOperation : public IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~IDownloadOperation() {}
			//! \return Identyfikator œci¹ganego pliku
			virtual const CompoundID fileID() const = 0;
			//! \return Czy plik jest dostepny w storage (œci¹gniêto i poprawnie zapisano, choæ reszta operacji mog³a pójsæ nie tak)
			virtual const bool fileDownloaded() const = 0;
			//! \return Storage
			virtual const IHMDBStoragePtr storage() = 0;
			//! \return Storage
			virtual const IHMDBStorageConstPtr storage() const = 0;
		};

		//! WskaŸnik dla operacji uploadu danych
		typedef utils::shared_ptr<IDownloadOperation> DownloadOperationPtr;
		//! WskaŸnik dla operacji uploadu danych
		typedef utils::shared_ptr<const IDownloadOperation> DownloadOperationConstPtr;

		//! Interfejs realizuj¹cy operacjê synchronizacji perspektywy danych
		class ISynchronizeOperation : public IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~ISynchronizeOperation() {}
			//! \return Pe³na p³ytka kopia bazy danych - ca³a perspektywa dostepnych danych
			virtual const ShallowCopyConstPtr shallowCopy() const = 0;
			//! \return Pe³na p³ytka kopia bazy danych - ca³a perspektywa dostepnych danych
			virtual const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const = 0;
		};

		//! WskaŸnik dla operacji synchronizacji
		typedef utils::shared_ptr<ISynchronizeOperation> SynchronizeOperationPtr;
		//! WskaŸnik dla operacji synchronizacji
		typedef utils::shared_ptr<const ISynchronizeOperation> SynchronizeOperationConstPtr;

	public:
		
		//! Destruktor wirtualny
		virtual ~IHMDBRemoteContext() {}

		//---------- Opis kontekstu zdalnego ---------------

		//! \return Sesja us³ug hmdb
		virtual const IHMDBSessionPtr session() = 0;
		//! \return Sesja us³ug hmdb
		virtual const IHMDBSessionConstPtr session() const = 0;

		//---------- Operacje kontekstu -----------------

		//! \return Operacja opisuj¹ca odœwie¿enia p³ytkiej kopii bazy danych
		virtual const SynchronizeOperationPtr prepareSynchronization(IHMDBStoragePtr storage = IHMDBStoragePtr()) = 0;

		//! \param fileIDs Identyfikatory plików do œci¹gniêcia
		//! \param ep Polityka wykonania
		//! \return Transfer danych
		virtual const DownloadOperationPtr prepareFileDownload(const CompoundID & fileID,
			IHMDBStoragePtr storage) = 0;

		//! \param fileName Nazwa pliku w bazie
		//! \param path Œcie¿ka po stronie bazy, gdzie zostanie zpaisany plik
		//! \param source Strumieñ z zawartoœci¹ pliku
		//! \param dataReference Której bazy dotyczy upload
		//! \return Transfer
		virtual const IHMDBFtp::TransferPtr prepareFileUpload(const std::string & fileName,
			const std::string & path, IHMDBStorage::IStreamPtr source,
			const DataReference dataReference) = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBRemoteContext);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBREMOTECONTEXT_H__