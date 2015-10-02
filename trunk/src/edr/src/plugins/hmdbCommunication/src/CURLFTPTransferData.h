/********************************************************************
	created:  2014/05/28	21:55:20
	filename: CURLFTPTransferData.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__

#include <networkUtils/CURLManager.h>
#include "CURLFTPStatus.h"
#include "CURLFTPProgress.h"
#include <plugins/hmdbCommunication/IHMDBStorage.h>

namespace hmdbCommunication
{
	//! Struktura opisuj�ca transfer
	struct CURLFTPTransferSharedState
	{
		//! Obiekt statusu
		CURLFTPStatus status;
		//! Uchwyt transferu
		networkUtils::CURLPtr curl;
	};

	struct CURLFTPTransferData
	{
		//! Obiekt do czekania
		std::future<CURLcode> result;
		//! Obiekt post�pu
		CURLFTPProgress progress;
		//! Obiekt opisuj�cy podstawy transferu
		utils::shared_ptr<CURLFTPTransferSharedState> sharedState;
	};

	//! Opis uploadu
	struct CURLFTPUploadTransferData : public CURLFTPTransferData
	{
		//! Strumie� �r�d�owy
		IHMDBStorageOperations::IStreamPtr stream;
	};

	//! Opis downloadu
	struct CURLFTPDownloadTransferData : public CURLFTPTransferData
	{
		//! Strumie� docelowy
		IHMDBStorageOperations::OStreamPtr stream;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__