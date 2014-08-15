/********************************************************************
	created:  2014/07/04	13:54:43
	filename: HMDBRemoteContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBREMOTECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBREMOTECONTEXT_H__

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
		virtual const IHMDBSessionPtr session();
		//! \return Sesja us�ug hmdb
		virtual const IHMDBSessionConstPtr session() const;

		//---------- Operacje kontekstu -----------------

		//! \return Operacja opisuj�ca od�wie�enia p�ytkiej kopii bazy danych
		virtual const SynchronizeOperationPtr prepareSynchronization(IHMDBStoragePtr storage);

		//! \param fileIDs Identyfikatory plik�w do �ci�gni�cia
		//! \param ep Polityka wykonania
		//! \return Transfer danych
		virtual const DownloadOperationPtr prepareFileDownload(const CompoundID & fileID,
			IHMDBStoragePtr storage);

		//! \param fileName Nazwa pliku w bazie
		//! \param path �cie�ka po stronie bazy, gdzie zostanie zpaisany plik
		//! \param source Strumie� z zawarto�ci� pliku
		//! \param dataReference Kt�rej bazy dotyczy upload
		//! \return Transfer
		virtual const IHMDBFtp::TransferPtr prepareFileUpload(const std::string & fileName,
			const std::string & path, IHMDBStorage::IStreamPtr source,
			const DataReference dataReference);

	private:
		//! Sesja z baz� danych
		IHMDBSessionPtr session_;
		//! Skr�t u�ytkownika na potrzeby obs�ugi p�ytkiej kopii bazy danych
		const std::string userHash;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBREMOTECONTEXT_H__