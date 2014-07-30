/********************************************************************
	created:  2014/07/04	13:54:43
	filename: HMDBRemoteContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBREMOTECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBREMOTECONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <threadingUtils/SynchronizationPolicies.h>

namespace hmdbCommunication
{
	class HMDBRemoteContext : public IHMDBRemoteContext
	{
	public:
		HMDBRemoteContext(IHMDBSession * session,
			const std::string & userHash);
		virtual ~HMDBRemoteContext();

		//! \return Sesja us³ug hmdb
		virtual IHMDBSession * session();
		//! \return Sesja us³ug hmdb
		virtual const IHMDBSession * session() const;

		//---------- Operacje kontekstu -----------------

		//! \return Operacja opisuj¹ca odœwie¿enia p³ytkiej kopii bazy danych
		virtual const SynchronizeOperationPtr prepareShallowCopyDownload(IHMDBStorage * storage);

		//! \param fileIDs Identyfikatory plików do œci¹gniêcia
		//! \param ep Polityka wykonania
		//! \return Transfer danych
		virtual const DownloadOperationPtr prepareFileDownload(const CompoundID & fileID,
			IHMDBStorage * storage);

		//! \param fileName Nazwa pliku w bazie
		//! \param path Œcie¿ka po stronie bazy, gdzie zostanie zpaisany plik
		//! \param source Strumieñ z zawartoœci¹ pliku
		//! \param dataReference Której bazy dotyczy upload
		//! \return Transfer
		virtual const TransferOperationPtr prepareFileUpload(const std::string & fileName,
			const std::string & path, std::istream * source,
			const DataReference dataReference);

	private:
		//! Sesja z baz¹ danych
		IHMDBSession * session_;

		const std::string userHash;
		//! Obiekt synchronizuj¹cy
		mutable threadingUtils::RecursiveSyncPolicy synch;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBREMOTECONTEXT_H__