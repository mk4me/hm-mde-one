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
		//! Domyœlny konsturktor
		HMDBShallowCopyDataContext();
		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyDataContext();
		//! \return Aktualna p³ytka kopia bazy danych
		virtual const ShallowCopyConstPtr shallowCopy() const;
		//! \return Manager statusów
		virtual const IHMDBStatusManager * dataStatusManager() const;
		//! \return Manager statusów
		virtual IHMDBStatusManager * dataStatusManager();
		//! \param shallowCopy P³ytka kopia bazy danych
		virtual void setShallowCopy(const ShallowCopyConstPtr shallowCopy);

	private:
		//! P³ytka kopia bazy danych
		ShallowCopyConstPtr shallowCopy_;
		//! Manager statusów danych
		utils::scoped_ptr<IHMDBStatusManager> statusManager_;
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
		//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych
		//! \param localContext Lokalny kontekst
		HMDBShallowCopyLocalContext(IHMDBShallowCopyDataContext * shallowCopyContext,
			IHMDBLocalContext * localContext);

		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyLocalContext();

		//! \param type Typ danych o które pytamy
		//! \param id Identyfikator danych
		//! \param recursive Czy pobieramy dane w dó³
		//! \return Dane domenowe za³adowane dla tego typu
		virtual const core::ConstVariantsList data(const DataType type,
			const hmdbServices::ID id, const bool recursive = true) const;
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P³ytka kopia wzglêdem której dzia³amy
		//! \return Identyfikatory plików które za³adowano
		virtual const bool load(const DataType type,
			const hmdbServices::ID id);

		//! Metoda ³aduje wszystkie dane
		virtual const bool loadAll();

		//! \param type Typ danych które chcemy wy³adowaæ
		//! \param ids Identyfikatory danych
		//! \param shallowCopy P³ytka kopia wzglêdem której wy³adowujemy
		//! \return Identyfikatory plików które wy³adowano
		virtual const bool unload(const DataType type,
			const hmdbServices::ID id);

		//! Metoda wy³adowuje wszystkie dane
		virtual const bool unloadAll();

		virtual IHMDBShallowCopyDataContext * shallowCopyContext();
		virtual const IHMDBShallowCopyDataContext * shallowCopyContext() const;

		virtual IHMDBLocalContext * localContext();
		virtual const IHMDBLocalContext * localContext() const;

	private:

		const core::VariantPtr findObjectByType(const DataType type, const hmdbServices::ID id,
			const utils::TypeInfo & typeInfo) const;

		PluginSubject::SubjectPtr getSubject(const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		PluginSubject::SessionPtr getSession(const std::pair<Indexes, MotionFiles> & sessionFiles,
			const IndexedData & loadedFilesData,
			PluginSubject::SubjectPtr subPtr,
			core::VariantPtr subOW,
			const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		PluginSubject::MotionPtr getMotion(const Indexes & motionFiles,
			const IndexedData & loadedFilesData,
			PluginSubject::SessionPtr sPtr,
			core::VariantPtr sOW,
			const hmdbServices::ID id,
			utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
			const ShallowCopyConstPtr shallowCopy);

		void removeMotion(const hmdbServices::ID id, const IndexedData & loadedFilesData, const ShallowCopyConstPtr shallowCopy);

		void removeSession(const hmdbServices::ID id, const IndexedData & loadedFilesData, const ShallowCopyConstPtr shallowCopy);

		void removeSubject(const hmdbServices::ID id, const IndexedData & loadedFilesData, const ShallowCopyConstPtr shallowCopy);

		const SubjectFiles groupDataInHierarchy(const IndexedData & loadedFilesData,
			const ShallowCopyConstPtr shallowCopy, Indexes & loadedFiles);
		
		void addPatientObject(const hmdbServices::MedicalShallowCopy::Patient * patient, PluginSubject::SubjectID subjectID,
			core::IMemoryDataManager::IMemoryDataTransaction * transaction);

		void loadSubjectHierarchy(const IndexedData & loadedFilesData);

		void unloadSubjectHierarchy(const DataType type,
			const hmdbServices::ID id);

	private:
		//! Mapa danych
		mutable GroupedData data_;
		//! Kontekst p³ytkiej kopii bazy danych
		IHMDBShallowCopyDataContext * shallowCopyContext_;
		//! Lokalny kontekst
		IHMDBLocalContext * localContext_;
	};

	class HMDBShallowCopyRemoteContext : public IHMDBShallowCopyRemoteContext
	{
	public:
		//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych
		//! \param remoteContext Zdalny kontekst
		HMDBShallowCopyRemoteContext(IHMDBShallowCopyDataContext * shallowCopyContext,
			IHMDBRemoteContext * remoteContext);

		//! Destruktor wirtualny
		virtual ~HMDBShallowCopyRemoteContext();
		//! \param type Typ danych które chcemy za³adowaæ
		//! \param id Identyfikator danych	
		//! \param recursive Czy pobieramy wszystkie dane w dó³ hierarchii
		//! \param forceDownload Czy wymuszamy œci¹ganie wszystkich danych czy tylko brakuj¹cych
		//! \return Przygotowany transfer danych
		virtual const IHMDBRemoteContext::CompoundOperationPtr prepareDownload(const DataType type,
			const hmdbServices::ID id, const bool recursive = true, const bool forceDownload = false);

		//! \param shallowCopy P³ytka kopia bazy danych
		//! \param incrementalBranchShallowCopy Przyrostowa kopia bazy danych
		virtual void synchronize(const ShallowCopyConstPtr shallowCopy,
			const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy = IncrementalBranchShallowCopyConstPtr());

		//! \param downloadOperations Operacje œci¹gania
		virtual void synchronize(const IHMDBRemoteContext::CompoundOperationPtr downloadOperations);

		virtual IHMDBShallowCopyDataContext * shallowCopyContext();
		virtual const IHMDBShallowCopyDataContext * shallowCopyContext() const;

		virtual IHMDBRemoteContext * remoteContext();
		virtual const IHMDBRemoteContext * remoteContext() const;

	private:
		//! Kontekst p³ytkiej kopii bazy danych
		IHMDBShallowCopyDataContext * shallowCopyContext_;
		//! Zdalny kontekst
		IHMDBRemoteContext * remoteContext_;
	};

	class HMDBShallowCopyContext : public IHMDBShallowCopyContext
	{
	public:

		HMDBShallowCopyContext(IHMDBShallowCopyDataContext * scdc,
			IHMDBShallowCopyLocalContext * sclc,
			IHMDBShallowCopyRemoteContext * scrc = nullptr);

		virtual ~HMDBShallowCopyContext();

		virtual IHMDBShallowCopyDataContext * shallowCopyDataContext();
		virtual const IHMDBShallowCopyDataContext * shallowCopyDataContext() const;

		virtual IHMDBShallowCopyLocalContext * shallowCopyLocalContext();
		virtual const IHMDBShallowCopyLocalContext * shallowCopyLocalContext() const;

		virtual IHMDBShallowCopyRemoteContext * shallowCopyRemoteContext();
		virtual const IHMDBShallowCopyRemoteContext * shallowCopyRemoteContext() const;

	private:

		utils::shared_ptr<IHMDBShallowCopyDataContext> dataContext_;
		utils::shared_ptr<IHMDBShallowCopyLocalContext> localContext_;
		utils::shared_ptr<IHMDBShallowCopyRemoteContext> remoteContext_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSHALLOWCOPYCONTEXT_H__