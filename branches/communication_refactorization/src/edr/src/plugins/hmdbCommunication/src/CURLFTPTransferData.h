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

namespace hmdbCommunication
{
	class CURLFTPProgress;

	//! Struktura opisuj¹ca transfer
	struct CURLFTPTransferData
	{
		//! Obiekt do czekania
		utils::shared_ptr<networkUtils::CURLManager::WaitCurl> wait;
		//! Obiekt postêpu
		utils::shared_ptr<CURLFTPProgress> progress;
		//! Obiekt statusu
		utils::shared_ptr<CURLFTPStatus> status;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__