/********************************************************************
	created:  2014/05/28	17:47:47
	filename: CURLFTPManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_medusaDatabase__CURLFTPMANAGER_H__
#define __HEADER_GUARD_medusaDatabase__CURLFTPMANAGER_H__

#include <networkUtils/CURLManager.h>
#include "CURLFTPTransferData.h"

namespace medusaDatabase
{
	class CURLFTPManager
	{
		class StateCURLManager;		

	public:
		//! Domy�lny konstruktor
		CURLFTPManager();
		//! \param multi Transfer multi
		explicit CURLFTPManager(networkUtils::CURLMPtr multi);
		//! Desturktor wirtualny
		virtual ~CURLFTPManager();

		//! \param curl Skonfigurowany uchwyt do obs�u�enia
		//! \param wait Obiekt pozwalaj�cy zaczeka� na zako��enie operacji curla
		std::future<CURLcode> addRequest(utils::shared_ptr<CURLFTPTransferSharedState> transfer);
		//! \param curl Uchwyt do usuni�cia
		void removeRequest(networkUtils::CURLPtr curl);
		//! Przetwarza zapytania
		void run();
		//! Ko�czy potencjalne przetwarzanie
		void finalize();

	private:
		//! Obiekt realizujacy ca�a funkcjonalno��
		utils::shared_ptr<StateCURLManager> curlManager;
	};
}

#endif	// __HEADER_GUARD_medusaDatabase__CURLFTPMANAGER_H__
