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
		//! Domy�lny konstruktor
		CURLFTPManager();
		//! \param multi Transfer multi
		explicit CURLFTPManager(CURLM * multi);
		//! Desturktor wirtualny
		virtual ~CURLFTPManager();

	private:
		//! \param curl Uchwyt kt�ry w�a�nie dodano
		virtual void onAddRequest(CURL * curl);
		//! \param curl Uchwyt kt�ry w�a�nie usuni�to
		virtual void onRemoveRequest(CURL * curl);
		//! \param curl Uchwyt kt�ry zosta� anulowany
		virtual void onCancelRequest(CURL * curl);
		//! \param curl Uchwyt kt�ry spowodowa� b��d
		//! \param error Opis b�edu
		virtual void onErrorRequest(CURL * curl, const std::string & error);
		//! \param curl Uchwyt kt�ry zako�czy� przetwarzanie danych poprawnie
		virtual void onFinishRequest(CURL * curl);
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPMANAGER_H__