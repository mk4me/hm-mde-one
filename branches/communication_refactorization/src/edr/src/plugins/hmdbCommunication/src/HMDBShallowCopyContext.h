/********************************************************************
	created:  2014/07/25	12:40:16
	filename: HMDBShallowCopyContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSHALLOWCOPYCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSHALLOWCOPYCONTEXT_H__

#include <utils/SmartPtr.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/subject/Types.h>

namespace hmdbCommunication
{
	class HMDBShallowCopyDataContext : public IHMDBShallowCopyDataContext
	{
	public:
		//! Domy�lny konsturktor
		HMDBShallowCopyDataContext();
		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyDataContext();
		//! \return Aktualna p�ytka kopia bazy danych
		virtual const ShallowCopyConstPtr shallowCopy() const;
		//! \return Manager status�w
		virtual const IHMDBStatusManagerConstPtr dataStatusManager() const;
		//! \return Manager status�w
		virtual const IHMDBStatusManagerPtr dataStatusManager();
		//! \param shallowCopy P�ytka kopia bazy danych
		virtual void setShallowCopy(const ShallowCopyConstPtr shallowCopy);

	private:
		//! P�ytka kopia bazy danych
		ShallowCopyConstPtr shallowCopy_;
		//! Manager status�w danych
		IHMDBStatusManagerPtr statusManager_;
	};

	class HMDBShallowCopyLocalContext : public IHMDBShallowCopyLocalContext
	{
	private:

		typedef std::set<hmdbServices::ID> Indexes;
		typedef std::map<hmdbServices::ID, Indexes> MotionFiles;
		typedef std::map<hmdbServices::ID, std::pair<Indexes, MotionFiles>> SessionFiles;
		typedef std::map<hmdbServices::ID, SessionFiles> SubjectFiles;

		typedef std::map<hmdbServices::ID, core::ConstVariantsList> IndexedData;
		typedef std::map<DataType, IndexedData> GroupedData;

	public:
		//! \param shallowCopyContext Kontekst p�ytkiej kopii bazy danych
		//! \param localContext Lokalny kontekst
		HMDBShallowCopyLocalContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
			IHMDBLocalContextPtr localContext);

		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyLocalContext();

		//! \param type Typ danych o kt�re pytamy
		//! \param id Identyfikator danych
		//! \param recursive Czy pobieramy dane w d�
		//! \return Dane domenowe za�adowane dla tego typu
		virtual const core::ConstVariantsList data(const DataType type,
			const hmdbServices::ID id, const bool recursive = true) const;
		//! \param type Typ danych kt�re chcemy za�adowa�
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P�ytka kopia wzgl�dem kt�rej dzia�amy
		//! \return Identyfikatory plik�w kt�re za�adowano
		virtual const bool load(const DataType type,
			const hmdbServices::ID id);

		//! Metoda �aduje wszystkie dane
		virtual const bool loadAll();

		//! \param type Typ danych kt�re chcemy wy�adowa�
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P�ytka kopia wzgl�dem kt�rej wy�adowujemy
		//! \return Identyfikatory plik�w kt�re wy�adowano
		virtual const bool unload(const DataType type,
			const hmdbServices::ID id);

		//! Metoda wy�adowuje wszystkie dane
		virtual const bool unloadAll();

		virtual const IHMDBShallowCopyDataContextPtr shallowCopyContext();
		virtual const IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const;

		virtual const IHMDBLocalContextPtr localContext();
		virtual const IHMDBLocalContextConstPtr localContext() const;

	private:

		const core::VariantPtr findObjectByType(const DataType type,
			const hmdbServices::ID id,
			const utils::TypeInfo & typeInfo) const;

		core::VariantPtr getSubject(const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		core::VariantPtr getSession(const std::pair<Indexes, MotionFiles> & sessionFiles,
			const IndexedData & loadedFilesData,
			PluginSubject::SubjectPtr subPtr,
			core::VariantPtr subOW,
			const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		core::VariantPtr getMotion(const Indexes & motionFiles,
			const IndexedData & loadedFilesData,
			PluginSubject::SessionPtr sPtr,
			core::VariantPtr sOW,
			const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		void removeMotion(const hmdbServices::ID id,
			const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);

		void removeSession(const hmdbServices::ID id,
			const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);

		void removeSubject(const hmdbServices::ID id,
			const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);

		const SubjectFiles groupDataInHierarchy(const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy);
		
		void addPatientObject(const hmdbServices::MedicalShallowCopy::Patient * patient,
			PluginSubject::SubjectID subjectID);

		void loadSubjectHierarchy(const IndexedData & loadedFilesData);

		void unloadSubjectHierarchy(const IndexedData & unloadedFiles);

	private:
		//! Mapa danych
		mutable GroupedData data_;
		//! Kontekst p�ytkiej kopii bazy danych
		IHMDBShallowCopyDataContextPtr shallowCopyContext_;
		//! Lokalny kontekst
		IHMDBLocalContextPtr localContext_;
	};

	class HMDBShallowCopyRemoteContext : public IHMDBShallowCopyRemoteContext
	{
	public:
		//! \param shallowCopyContext Kontekst p�ytkiej kopii bazy danych
		//! \param remoteContext Zdalny kontekst
		HMDBShallowCopyRemoteContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
			IHMDBRemoteContextPtr remoteContext);

		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyRemoteContext();
		//! \param type Typ danych kt�re chcemy za�adowa�
		//! \param id Identyfikator danych	
		//! \param recursive Czy pobieramy wszystkie dane w d� hierarchii
		//! \param forceDownload Czy wymuszamy �ci�ganie wszystkich danych czy tylko brakuj�cych
		//! \return Przygotowany transfer danych
		virtual const std::list<IHMDBRemoteContext::DownloadOperationPtr> prepareDownload(const DataType type,
			const hmdbServices::ID id, const bool recursive = true, const bool forceDownload = false);
		//! \return Operacja synchronizacji
		virtual const SynchronizeOperationPtr prepareSynchronization(IHMDBStoragePtr storage);
		//! \param shallowCopy P�ytka kopia bazy danych
		//! \param incrementalBranchShallowCopy Przyrostowa kopia bazy danych
		virtual void synchronize(const ShallowCopyConstPtr shallowCopy,
			const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy = IncrementalBranchShallowCopyConstPtr());

		//! \param downloadOperations Operacje �ci�gania
		virtual void synchronize(const SynchronizeOperationPtr downloadOperations);

		virtual const IHMDBShallowCopyDataContextPtr shallowCopyContext();
		virtual const IHMDBShallowCopyDataContextConstPtr shallowCopyContext() const;

		virtual const IHMDBRemoteContextPtr remoteContext();
		virtual const IHMDBRemoteContextConstPtr remoteContext() const;

	private:
		//! Kontekst p�ytkiej kopii bazy danych
		IHMDBShallowCopyDataContextPtr shallowCopyContext_;
		//! Zdalny kontekst
		IHMDBRemoteContextPtr remoteContext_;
	};

	class HMDBShallowCopyContext : public IHMDBShallowCopyContext
	{
	public:

		HMDBShallowCopyContext(IHMDBShallowCopyDataContextPtr scdc,
			IHMDBShallowCopyLocalContextPtr sclc,
			IHMDBShallowCopyRemoteContextPtr scrc = nullptr);

		virtual ~HMDBShallowCopyContext();

		virtual const IHMDBShallowCopyDataContextPtr shallowCopyDataContext();
		virtual const IHMDBShallowCopyDataContextConstPtr shallowCopyDataContext() const;

		virtual const IHMDBShallowCopyLocalContextPtr shallowCopyLocalContext();
		virtual const IHMDBShallowCopyLocalContextConstPtr shallowCopyLocalContext() const;

		virtual const IHMDBShallowCopyRemoteContextPtr shallowCopyRemoteContext();
		virtual const IHMDBShallowCopyRemoteContextConstPtr shallowCopyRemoteContext() const;

	private:

		IHMDBShallowCopyDataContextPtr dataContext_;
		IHMDBShallowCopyLocalContextPtr localContext_;
		IHMDBShallowCopyRemoteContextPtr remoteContext_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSHALLOWCOPYCONTEXT_H__