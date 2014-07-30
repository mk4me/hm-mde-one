/********************************************************************
	created:  2014/07/04	10:12:59
	filename: IHMDBRemoteContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBREMOTECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBREMOTECONTEXT_H__

//#include <plugins/hmdbCommunication/IHMDBSourceContext.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBFtp.h>
#include <utils/SmartPtr.h>
#include <hmdbserviceslib/ShallowCopy.h>
#include <threadingUtils/IOperation.h>
#include <threadingUtils/IProgress.h>

namespace hmdbCommunication
{
	class IHMDBSession;
	class IHMDBStorage;

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

		enum OperationType
		{
			WebServiceCall,		//! Wo³anie us³ug HMDB
			PreparingHMDB,		//! Przygotowanie plików do œci¹gniêcia
			PreparingOutput,	//! Przygotowanie strumieni do zapisu plików
			TransferingData,	//! Transfer danych
			ClosingOutput,		//! Zamkniêcie strumieni ze œci¹gniêtymi danymi
			OpeningInput,		//! Otwarcie outputa do odczytu
			StoringData,		//! Zapis danych w storage
			ClosingInput,		//! Zamkykanie inputa do odczytu
			CleaningOutput,		//! Usuniêcie strumieni danych
			CleaningHMDB,		//! Oczyszczenie bazy danych po transferze
			Synchronizing,		//! Synchronizacja
			ExtractingShallowCopy,
			Downloading,		//! Œci¹ganie danych do storage
			Uploading			//! £adownaie danych do HMDB
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
			//! \return Typ operacji
			virtual const OperationType operationType() const = 0;
		};

		//! WskaŸnik dla operacji
		typedef utils::shared_ptr<IOperation> OperationPtr;
		//! WskaŸnik dla operacji
		typedef utils::shared_ptr<const IOperation> OperationConstPtr;

		//! Interfejs opisuj¹cy wiele operacji
		class ICompoundOperation : public virtual IOperation
		{
		public:
			//! Polityka wykonania
			enum ExecutionPolicy
			{
				Any,			//! Pojawiaj¹ce siê b³êdy nie przeszkodz¹ pozosta³ym operacjom
				AllOrNothing	//! Pierwszy napotkany b³¹d przerywa pozosta³e i wycofuje wszystkie zmiany
			};

			//! Sposób wykonania
			enum ExecutionSchema
			{
				Serial,		//! Zadania wykonywane sa jedno po drugim
				Parallel	//! Zadania wykonywane s¹ równolegle
			};

		public:
			//! Destruktor wirtualny
			virtual ~ICompoundOperation() {}
			//! \return Postêp
			virtual const float progress() const
			{
				const auto s = size();
				float ret = 0.0;
				for (unsigned int i = 0; i < s; ++i){
					ret += operation(i)->progress();
				}

				return s > 0 ? ret / (float)s : 1.0;
			}
			//! \return Iloœæ podoperacji
			virtual const unsigned int size() const = 0;
			//! \param idx Index podoperacji
			virtual const IOperation * operation(const unsigned int idx) const = 0;
			//! \param idx Index podoperacji
			virtual IOperation * operation(const unsigned int idx) = 0;
			//! \return Polityka wykonania operacji
			virtual const ExecutionPolicy executionPolicy() const = 0;
			//! \return sposób wykonania operacji
			virtual const ExecutionSchema executionSchema() const = 0;
		};

		//! WskaŸnik dla z³o¿onej operacji
		typedef utils::shared_ptr<ICompoundOperation> CompoundOperationPtr;
		//! WskaŸnik dla z³o¿onej operacji
		typedef utils::shared_ptr<const ICompoundOperation> CompoundOperationConstPtr;

		//! Operacja realizuj¹ca transfer danych
		class ITransferOperation : public virtual IOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITransferOperation() {}

			//! \return Transfer FTP obs³ugiwany przez t¹ operacjê
			virtual const IHMDBFtp::ITransfer * transfer() const = 0;
			//! \return Postep
			virtual const float progress() const
			{
				return transfer()->progress();
			}
		};

		//! WskaŸnik dla operacji transferu
		typedef utils::shared_ptr<ITransferOperation> TransferOperationPtr;
		//! WskaŸnik dla operacji transferu
		typedef utils::shared_ptr<const ITransferOperation> TransferOperationConstPtr;

		//! Interfejs opisuj¹cy upload danych, dostarcza informacji o identyfikatorze pliku po zapisie w bazie
		class IUploadOperation : public ICompoundOperation
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
		class IDownloadOperation : public ICompoundOperation
		{
		public:
			//! Destruktor wirtualny
			virtual ~IDownloadOperation() {}
			//! \return Identyfikator œci¹ganego pliku
			virtual const CompoundID fileID() const = 0;
			//! \return Czy plik jest dostepny w storage (œci¹gniêto i poprawnie zapisano, choæ reszta operacji mog³a pójsæ nie tak)
			virtual const bool fileDownloaded() const = 0;
			//! \return Strumie
			virtual std::istream * stream() = 0;
		};

		//! WskaŸnik dla operacji uploadu danych
		typedef utils::shared_ptr<IDownloadOperation> DownloadOperationPtr;
		//! WskaŸnik dla operacji uploadu danych
		typedef utils::shared_ptr<const IDownloadOperation> DownloadOperationConstPtr;

		//! Interfejs realizuj¹cy operacjê synchronizacji perspektywy danych
		class ISynchronizeOperation : public virtual ICompoundOperation
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
		virtual IHMDBSession * session() = 0;
		//! \return Sesja us³ug hmdb
		virtual const IHMDBSession * session() const = 0;

		//---------- Operacje kontekstu -----------------

		//! \return Operacja opisuj¹ca odœwie¿enia p³ytkiej kopii bazy danych
		virtual const SynchronizeOperationPtr prepareShallowCopyDownload(IHMDBStorage * storage = nullptr) = 0;

		//! \param fileIDs Identyfikatory plików do œci¹gniêcia
		//! \param ep Polityka wykonania
		//! \return Transfer danych
		virtual const DownloadOperationPtr prepareFileDownload(const CompoundID & fileID,
			IHMDBStorage * storage) = 0;

		//! \param fileName Nazwa pliku w bazie
		//! \param path Œcie¿ka po stronie bazy, gdzie zostanie zpaisany plik
		//! \param source Strumieñ z zawartoœci¹ pliku
		//! \param dataReference Której bazy dotyczy upload
		//! \return Transfer
		virtual const TransferOperationPtr prepareFileUpload(const std::string & fileName,
			const std::string & path, std::istream * source,
			const DataReference dataReference) = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBREMOTECONTEXT_H__