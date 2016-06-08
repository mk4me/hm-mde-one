/********************************************************************
	created:  2014/07/04	13:54:43
	filename: HMDBRemoteContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__HMDBREMOTECONTEXT_H__
#define __HEADER_GUARD_hmdbCommunication__HMDBREMOTECONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>

namespace hmdbCommunication
{
	class HMDBRemoteContext : public IHMDBRemoteContext
	{
	public:
		//! \param session Sesja z baz� danych
		//! \param userHash Skr�t u�ytkownika na potrzeby obs�ugi p�ytkiej kopii bazy danych
		HMDBRemoteContext(IHMDBSessionPtr session, const std::string & userHash);
		//! Destrutkor wirtualny
		virtual ~HMDBRemoteContext();

		//! \return Sesja us�ug hmdb
		virtual IHMDBSessionPtr session();
		//! \return Sesja us�ug hmdb
		virtual IHMDBSessionConstPtr session() const;

		//---------- Operacje kontekstu -----------------

		//! \return Operacja opisuj�ca od�wie�enia p�ytkiej kopii bazy danych
		virtual std::list<DownloadOperationPtr> prepareSynchronization(const hmdbServices::DateTime & since);

		//! \param fileIDs Identyfikatory plik�w do �ci�gni�cia
		//! \param ep Polityka wykonania
		//! \return Transfer danych
		virtual DownloadOperationPtr prepareFileDownload(const FileDescriptor & fileID);

		//! \param fileName Nazwa pliku w bazie
		//! \param path �cie�ka po stronie bazy, gdzie zostanie zpaisany plik
		//! \param source Strumie� z zawarto�ci� pliku
		//! \param dataReference Kt�rej bazy dotyczy upload
		//! \return Transfer
		virtual IHMDBFtp::TransferPtr prepareFileUpload(const std::string & fileName,
			const std::string & path, IHMDBStorageOperations::IStreamPtr source,
			const DataReference dataReference,
			const hmdbServices::FileSize fileSize);

	private:
		//! Sesja z baz� danych
		IHMDBSessionPtr session_;
		//! Skr�t u�ytkownika na potrzeby obs�ugi p�ytkiej kopii bazy danych
		const std::string userHash;
	};
}

#endif	// __HEADER_GUARD_hmdbCommunication__HMDBREMOTECONTEXT_H__
