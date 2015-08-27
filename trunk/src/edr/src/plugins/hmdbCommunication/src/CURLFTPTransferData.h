/********************************************************************
	created:  2014/05/28	21:55:20
	filename: CURLFTPTransferData.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__

#include <networkUtils/CURLManager.h>
#include <networkUtils/CURLFTPHelper.h>
#include "CURLFTPStatus.h"
#include "CURLFTPProgress.h"
#include <plugins/hmdbCommunication/IHMDBStorage.h>

namespace hmdbCommunication
{
	class CURLFTPProgress;

	//! Struktura opisuj¹ca transfer
	struct CURLFTPTransferData
	{
	public:
		CURLFTPTransferData() : progress(new CURLFTPProgress), status(new CURLFTPStatus)
		{

		}

		//! Obiekt do czekania
		std::future<CURLcode> wait;
		//! Obiekt postêpu
		utils::shared_ptr<CURLFTPProgress> progress;
		//! Obiekt statusu
		utils::shared_ptr<CURLFTPStatus> status;
	};

	struct CURLFTPUploadTransferData : public CURLFTPTransferData
	{
	public:
		IHMDBStorageOperations::IStreamPtr stream;
	};

	struct CURLFTPDownloadTransferData : public CURLFTPTransferData
	{
	public:
		IHMDBStorageOperations::OStreamPtr stream;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__