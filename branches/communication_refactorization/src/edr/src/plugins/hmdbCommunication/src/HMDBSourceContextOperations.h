/********************************************************************
	created:  2014/06/05	17:05:14
	filename: HMDBSourceContextOperations.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXTOPERATIONS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXTOPERATIONS_H__

#include "HMDBRemoteContext.h"
#include <plugins/hmdbCommunication/IHMDBFtp.h>
#include "ShallowCopyUtils.h"
#include <boost/atomic.hpp>
#include <corelib/IJob.h>
#include <corelib/IThread.h>
#include <corelib/Filesystem.h>
#include <hmdbserviceslib/IncrementalBranchShallowCopy.h>

namespace hmdbServices
{
	class IFileStoremanWS;
}


namespace hmdbCommunication
{

	class IHMDBStatusManager;

	//! Operacja zbiorcza, wspiera rozne polityki wykonania i schematy wykonania
	class CompoundOperation : public virtual IHMDBRemoteContext::ICompoundOperation
	{
	public:

		typedef std::vector<IHMDBRemoteContext::OperationPtr> Operations;

	public:
		//! Konstruktor
		//! \param name Nazwa operacji
		//! \param operation Operacje wchodzacje w skład operacji zbiorczej
		//! \param ep Polityka wykonania
		//! \param es Schemat wykonania
		CompoundOperation(const IHMDBRemoteContext::OperationType opt,
			const ExecutionPolicy ep,
			const ExecutionSchema es,
			const Operations & operations = Operations());

		//! Destruktor wirtualny
		virtual ~CompoundOperation();
		//! \return Typ operacji
		virtual const IHMDBRemoteContext::OperationType operationType() const;
		//! \return Ilość podoperacji
		virtual const unsigned int size() const;
		//! \param idx Index podoperacji
		virtual const IHMDBRemoteContext::IOperation * operation(const unsigned int idx) const;
		//! \param idx Index podoperacji
		virtual IHMDBRemoteContext::IOperation * operation(const unsigned int idx);
		//! \return Polityka wykonania operacji
		virtual const ExecutionPolicy executionPolicy() const;
		//! \return Sposób wykonania operacji
		virtual const ExecutionSchema executionSchema() const;
		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda przerywa operację
		virtual void abort();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! \return Stan operacji
		virtual const Status status() const;
		//! \return Opis błędu
		virtual const std::string error() const;

	protected:

		//! \param suboperations
		void setOperations(const Operations & suboperations);

	private:
		//! Sposoby i polityki wykonania
		void callAnySerial();
		void callAnyParallel();
		void callAllSerial();
		void callAllParallel();
		//! Metoda dla obserwacji
		void observe();

		const Status compoundStatus() const;
		IOperation * findErrorOperation() const;

	private:
		//! Typ operacji
		const IHMDBRemoteContext::OperationType opt;
		//! Operacje wchodzacje w skład operacji zbiorczej
		Operations operations;
		//! Polityka wykonania
		const ExecutionPolicy ep;
		//! Schemat wykonania
		const ExecutionSchema es;
		//! Wątek realizujacy operacje
		core::IThreadPtr workerThread;
		//! Operacja która spowodowała błąd
		mutable IOperation * errorOperation;
		//! Status
		mutable boost::atomic<Status> status_;
	};
	
	//! Operacja realizujca fuktora
	class FunctorOperation : public virtual IHMDBRemoteContext::IOperation
	{
	public:
		//! \param opt Typ operacji
		//! \param functor Zadanie do wykonania
		FunctorOperation(const IHMDBRemoteContext::OperationType opt,
			threadingUtils::FunctorRunnable::Functor functor = threadingUtils::FunctorRunnable::Functor());
		//! Destruktor wirtualny
		virtual ~FunctorOperation();
		//! \return Typ operacji
		virtual const IHMDBRemoteContext::OperationType operationType() const;
		//! \return Post�p realizacji przetwarzania
		virtual const float progress() const;
		//! Metoda anuluje aktualne zadanie
		virtual void abort();
		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! \return Stan operacji
		virtual const Status status() const;
		//! \return Opis błędu
		virtual const std::string error() const;

	protected:
		//! \param functor Operacja do wykonania
		void setFunctor(threadingUtils::FunctorRunnable::Functor functor);

	private:
		//! Woła funktor i aktualizuje status, postęp oraz błędy
		void call();

	private:
		//! Postęp
		boost::atomic<float> progress_;
		//! Typ operacji
		const IHMDBRemoteContext::OperationType opt;
		//! Zadanie do wykonania
		threadingUtils::FunctorRunnable::Functor functor;
		//! Job obsługujący zadanie
		core::IJobPtr job;
		//! Status
		boost::atomic<Status> status_;
		//! Opis błędu
		std::string error_;
	};
	
	//! Operacja pojedynczego transfery
	class TransferOperation : public IHMDBRemoteContext::ITransferOperation
	{
	public:
		//! \param transfer 
		TransferOperation();
		//! Destruktor wirtualny
		virtual ~TransferOperation();
		//! \param transfer Transfer obsugiwany przez operacje
		void setTransfer(IHMDBFtp::TransferPtr transfer);

		//! \return Transfer FTP obsługiwany przez tą operację
		virtual IHMDBFtp::ITransfer * transfer();
		//! \return Transfer FTP obsługiwany przez tą operację
		virtual const IHMDBFtp::ITransfer * transfer() const;
		//! \return Postep
		virtual const float progress() const;
		//! Metoda anuluje aktualne zadanie
		virtual void abort();
		//! Metoda rozpoczyna operację
		virtual void start();
		//! Metoda czeka do zakończenia operacji
		virtual void wait();
		//! \return Stan operacji
		virtual const Status status() const;
		//! \return Opis błędu
		virtual const std::string error() const;
		virtual const IHMDBRemoteContext::OperationType operationType() const;

	private:
		//! Status
		boost::atomic<Status> status_;
		//! Transfer
		IHMDBFtp::TransferPtr transfer_;
	};

	//! Klasa realizująca upload danych, dostarcza informacji o identyfikatorze pliku po zapisie w bazie
	class UploadOperation : public IHMDBRemoteContext::IUploadOperation, public CompoundOperation
	{
	public:
		//! Funktor realizujący zapis pliku do bazy danych
		//! Zwraca identyfikator pliku
		typedef boost::function < int(const std::string & file,
			const std::string & fileDescription,
			const std::string & subdirectory) > HMDBStore;

	public:
		//! Konstruktor
		//! \param fileName Nazwa pliku na ftp
		//! \param source Źródło pliku
		//! \param ftp Połączenie uzywane do wysłania danych
		//! \param store Usługa HMDB zapisująca plik w bazie
		//! \param fileDescription Opis pliku
		//! \param subdirectory
		UploadOperation(const std::string & fileName,
			std::istream * source,
			IHMDBFtp * ftp, HMDBStore store,
			const unsigned long long size = 0,
			const std::string & fileDescription = std::string(),
			const std::string & subdirectory = std::string());

		//! Destruktor wirtualny
		virtual ~UploadOperation();
		//! \return Identyfikator pliku po zapisie w bazie
		virtual const int fileID() const;

	private:
		//! Pobiera identyfikator uploadowanego pliku
		void getFileID();

	private:
		//! ID pliku po upload
		boost::atomic<int> fileID_;
		//! Nazwa pliku w bazie danych
		const std::string fileName;
		//! Zawartośc pliku
		std::istream * source;
		//! Ftp użyty do uploadu
		IHMDBFtp * ftp;
		//! Metoda zapisująca plik
		HMDBStore store;
		//! Opis pliku
		const std::string fileDescription;
		//! Podkatalog pliku
		const std::string subdirectory;
		//! Operacja łączona
		utils::shared_ptr<CompoundOperation> cop;
	};

	//! Interfejs zadania dostarczającego mapy plików do ściągnięcia
	class PrepareFileToDownloadOperation : public FunctorOperation
	{
	private:

		class ClearFileToDownloadOperation;
		friend class ClearFileToDownloadOperation;

	public:
		//! \param session Sesja usług HMDB
		//! \param toPrepare Pliki do przygotowania do ściągnięcia po stronie HMDB
		//! \param Polityka wykonania
		PrepareFileToDownloadOperation(hmdbServices::IFileStoremanWS * fs,
			const hmdbServices::ID fileID);

		//! Destruktor wirtualny
		virtual ~PrepareFileToDownloadOperation();

		//! \return Mapa przygotowanych plików do ściągnięcia
		const std::string & preparedFilePath() const;

		ClearFileToDownloadOperation * clearFileDownloadOperation();

	private:

		void setPath(const std::string & path);

	private:
		hmdbServices::IFileStoremanWS * fs;
		const hmdbServices::ID fileID_;
		std::string preparedFilePath_;
	};

	class IOpenTransferOutputOperation : public virtual IHMDBRemoteContext::IOperation
	{
	public:
		//! Destruktor wirtualny
		virtual ~IOpenTransferOutputOperation() {}

		//! \return Wyjscia dla sciaganych plikow
		virtual std::istream * fileInput() = 0;

		virtual IHMDBRemoteContext::IOperation * closeInputOperation() = 0;
		
	};

	class IDownloadFileOperation : public virtual IHMDBRemoteContext::IOperation
	{
	public:
		//! Destruktor wirtualny
		virtual ~IDownloadFileOperation() {}

		virtual void setDownload(const std::string & path, std::ostream * stream, const hmdbServices::FileSize size = 0) = 0;
	};

	class FTPDownloadFileOperation : public TransferOperation, public IDownloadFileOperation
	{
	public:
		FTPDownloadFileOperation(IHMDBFtp * ftp);
		virtual ~FTPDownloadFileOperation();

		virtual void setDownload(const std::string & path, std::ostream * stream, const hmdbServices::FileSize size);

	private:
		IHMDBFtp * ftp;
	};

	class IStoreOutputOperation : public virtual IHMDBRemoteContext::IOperation
	{
	public:
		//! Destruktor wirtualny
		virtual ~IStoreOutputOperation() {}

		//! \return Wyjscia dla sciaganych plikow
		virtual void setFileInput(std::istream * stream) = 0;
		//! \return Strumien po zapisie
		virtual std::istream * stream() = 0;
	};

	class StoreOutputOperation : public IStoreOutputOperation, public FunctorOperation
	{
	public:
		StoreOutputOperation(IHMDBStorage * storage, const std::string & key);

		virtual ~StoreOutputOperation();

		//! \return Wyjscia dla sciaganych plikow
		virtual void setFileInput(std::istream * stream);

		virtual std::istream * stream();

	private:
		//! Operacja zapisu
		void store();

	private:
		//! Miejsce zapisu danych
		IHMDBStorage * storage;
		//! Klucz pod ktorym zapiszemy dane
		const std::string key;
		//!Strumien do zapisu
		std::istream * stream_;
	};

	class IPrepareTransferOutputOperation : public virtual IHMDBRemoteContext::IOperation
	{
	public:
		//! Destruktor wirtualny
		virtual ~IPrepareTransferOutputOperation() {}
		//! \return Wyjscia dla sciaganych plikow
		virtual std::ostream * fileOutput() = 0;
		//! Operacja zamykająca output (flush danych)
		virtual IHMDBRemoteContext::IOperation * closeOutputOperation() = 0;
		//! Operacja otwierająca zapisane dane do odczytu
		virtual IOpenTransferOutputOperation * openInputOperation() = 0;
		//! Operacja sprzątająca tymczasowe zasoby
		virtual IHMDBRemoteContext::IOperation * cleanOutputOperation() = 0;
	};

	class PrepareTmpFileTransferOutputOperation : public IPrepareTransferOutputOperation,
		public FunctorOperation
	{
	private:

		class OpenTmpFileTransferOutputOperation : public IOpenTransferOutputOperation,
			public FunctorOperation
		{
		public:

			OpenTmpFileTransferOutputOperation(PrepareTmpFileTransferOutputOperation * op);

			//! Destruktor wirtualny
			virtual ~OpenTmpFileTransferOutputOperation();

			//! \return Wyjscia dla sciaganych plikow
			virtual std::istream * fileInput();

			virtual IHMDBRemoteContext::IOperation * closeInputOperation();

		private:
			//! Otiwera do odczytu output
			void openOutput();

		private:
			//! Output otwarty do odczytu
			std::ifstream * stream_;
			//! Operacja przygotowująca output pliku
			PrepareTmpFileTransferOutputOperation * op;
		};

		friend class OpenTmpFileTransferOutputOperation;

	public:
		//! Domyslny konstruktor
		PrepareTmpFileTransferOutputOperation();
		//! Destruktor wirtualny
		virtual ~PrepareTmpFileTransferOutputOperation();
		//! \return Wyjscia dla sciaganych plikow
		virtual std::ostream * fileOutput();
		//! Operacja zamykająca output (flush danych)
		virtual IHMDBRemoteContext::IOperation * closeOutputOperation();
		//! Operacja otwierająca zapisane dane do odczytu
		virtual IOpenTransferOutputOperation * openInputOperation();
		//! Operacja sprzątająca tymczasowe zasoby
		virtual IHMDBRemoteContext::IOperation * cleanOutputOperation();

	private:
		//! Tworzy plik tymczasowy na dysku o losowej nazwie na potrzeby zapisu
		void prepareOutput();

	private:
		//! Strumień do zapisu danych
		std::ofstream * stream;
		//! Ściezka pliku
		core::Filesystem::Path tmpFilePath;

		IOpenTransferOutputOperation * oOp;
	};

	class PrepareMemoryTransferOutputOperation : public IPrepareTransferOutputOperation,
		public FunctorOperation
	{
	private:

		class OpenMemoryTransferOutputOperation : public IOpenTransferOutputOperation,
			public FunctorOperation
		{
		public:

			OpenMemoryTransferOutputOperation(PrepareMemoryTransferOutputOperation * op);

			//! Destruktor wirtualny
			virtual ~OpenMemoryTransferOutputOperation();

			//! \return Wyjscia dla sciaganych plikow
			virtual std::istream * fileInput();

			virtual IHMDBRemoteContext::IOperation * closeInputOperation();

		private:

			//! Otiwera do odczytu output
			void openOutput();			

		private:

			std::istream * stream;

			PrepareMemoryTransferOutputOperation * op;
		};

		friend class OpenMemoryTransferOutputOperation;

	public:

		PrepareMemoryTransferOutputOperation();
		//! Destruktor wirtualny
		virtual ~PrepareMemoryTransferOutputOperation();
		//! \return Wyjscia dla sciaganych plikow
		virtual std::ostream * fileOutput();
		//! Operacja zamykająca output (flush danych)
		virtual IHMDBRemoteContext::IOperation * closeOutputOperation();
		//! Operacja otwierająca zapisane dane do odczytu
		virtual IOpenTransferOutputOperation * openInputOperation();
		//! Operacja sprzątająca tymczasowe zasoby
		virtual IHMDBRemoteContext::IOperation * cleanOutputOperation();

	private:

		void prepareOutput();

	private:
		std::stringstream * stream;
	};

	//! Operacja pozwalajaca realizowac sciaganie plikow z HMDB
	class CompoundFileDownload : public IHMDBRemoteContext::IDownloadOperation, public FunctorOperation
	{
	public:
		//! \param fileToDownload		
		//! \param ptOp
		//! \param storage
		CompoundFileDownload(const IHMDBRemoteContext::CompoundID & fileToDownload,
			PrepareFileToDownloadOperation * pfOp,
			IPrepareTransferOutputOperation * ptOp,
			IDownloadFileOperation * dOp,
			IStoreOutputOperation * soOp);

		//! Destruktor wirtualny
		virtual ~CompoundFileDownload();
		//! \return Psotep operacji
		virtual const float progress() const;
		//! \return Ilość podoperacji
		virtual const unsigned int size() const;
		//! \param idx Index podoperacji
		virtual const IHMDBRemoteContext::IOperation * operation(const unsigned int idx) const;
		//! \param idx Index podoperacji
		virtual IHMDBRemoteContext::IOperation * operation(const unsigned int idx);
		//! \return Polityka wykonania operacji
		virtual const ExecutionPolicy executionPolicy() const;
		//! \return Sposób wykonania operacji
		virtual const ExecutionSchema executionSchema() const;
		//! \return Identyfikator ściąganego pliku
		virtual const IHMDBRemoteContext::CompoundID fileID() const;
		//! \return Czy plik jest dostepny w storage (ściągnięto i poprawnie zapisano, choć reszta operacji mogła pójsć nie tak)
		virtual const bool fileDownloaded() const;
		//! \return Typ operacji
		virtual const IHMDBRemoteContext::OperationType operationType() const;

		virtual std::istream * stream();

	private:
		//! Wykonanie operacji
		void run();

	private:

		const IHMDBRemoteContext::CompoundID file_;

		boost::atomic<bool> downloaded_;

		//! Operacja przygotowujaca pliki w HMDB
		utils::shared_ptr<PrepareFileToDownloadOperation> pfOp;
		//! Operacja przygotowujaca output dla sciaganego pliku
		utils::shared_ptr<IPrepareTransferOutputOperation> ptOp;
		//! Operacja sciagajaca przygotowany plik z FTP
		utils::shared_ptr<IDownloadFileOperation> dOp;
		//! Operacja zamykajca output pliku - utrwala jego stan
		utils::shared_ptr<IHMDBRemoteContext::IOperation> ctoOp;
		//! Operacja czyszczaca pliki na HMDB
		utils::shared_ptr<IHMDBRemoteContext::IOperation> cfOp;
		//! Operacja otwiera sciagniety plik do odczytu
		utils::shared_ptr<IOpenTransferOutputOperation> oiOp;
		//! Operacja utrwalajaca plik w storage
		utils::shared_ptr<IStoreOutputOperation> soOp;
		//! Operacja zamykajaca plik do odczytu
		utils::shared_ptr<IHMDBRemoteContext::IOperation> ciOp;
		//! Operacja czyszczaca output pliku po zakonczeniu transferu
		utils::shared_ptr<IHMDBRemoteContext::IOperation> cltoOp;
		//! Zbior operacji
		std::vector<IOperation*> operations;
		//! Faktyczna operacja
		utils::shared_ptr<FunctorOperation> fOp;
		//! Storage do zapisu ściągniętych danych
		IHMDBStorage * storage_;
	};

	class ExtractShallowcopy : public FunctorOperation
	{
	public:
		ExtractShallowcopy(const std::list<IHMDBRemoteContext::IDownloadOperation*> & downloads);

		virtual ~ExtractShallowcopy();

		virtual const float progress() const;

		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		const ShallowCopyConstPtr shallowCopy() const;
		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const;

	protected:

		void extract();

	private:

		ShallowCopyPtr shallowCopy_;
		IncrementalBranchShallowCopyConstPtr incrementalShallowCopy_;

		boost::atomic<unsigned int> progress_;
		const std::list<IHMDBRemoteContext::IDownloadOperation*> downloads;
	};

	class SynchronizeOperation : public IHMDBRemoteContext::ISynchronizeOperation, public CompoundOperation
	{
	public:
		SynchronizeOperation(const std::list<IHMDBRemoteContext::IDownloadOperation*> & downloads,
			IHMDBStorage * storage, const bool mustDelete);

		virtual ~SynchronizeOperation();

		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		virtual const ShallowCopyConstPtr shallowCopy() const;
		//! \return Pełna płytka kopia bazy danych - cała perspektywa dostepnych danych
		virtual const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy() const;

	private:
		ExtractShallowcopy * eOp;
		IHMDBStorage * storage;
		const bool mustDelete;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXTOPERATIONS_H__