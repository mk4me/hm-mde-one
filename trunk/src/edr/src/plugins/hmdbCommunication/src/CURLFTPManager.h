/********************************************************************
	created:  2014/05/28	17:47:47
	filename: CURLFTPManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPMANAGER_H__

#include <networkUtils/CURLManager.h>

namespace hmdbCommunication
{
	class CURLFTPManager : public networkUtils::CURLManager
	{
	public:
		//! Domyœlny konstruktor
		CURLFTPManager();
		//! \param multi Transfer multi
		explicit CURLFTPManager(CURLM * multi);
		//! Desturktor wirtualny
		virtual ~CURLFTPManager();

	private:
		//! \param curl Uchwyt który w³aœnie dodano
		virtual void onAddRequest(CURL * curl);
		//! \param curl Uchwyt który w³aœnie usuniêto
		virtual void onRemoveRequest(CURL * curl);
		//! \param curl Uchwyt który zosta³ anulowany
		virtual void onCancelRequest(CURL * curl);
		//! \param curl Uchwyt który spowodowa³ b³¹d
		//! \param error Opis b³edu
		virtual void onErrorRequest(CURL * curl, const std::string & error);
		//! \param curl Uchwyt który zakoñczy³ przetwarzanie danych poprawnie
		virtual void onFinishRequest(CURL * curl);
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPMANAGER_H__