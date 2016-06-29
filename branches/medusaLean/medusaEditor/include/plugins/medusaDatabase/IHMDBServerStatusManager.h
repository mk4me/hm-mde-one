/********************************************************************
	created:  2014/07/11	19:08:35
	filename: IHMDBServerStatusManager.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVERSTATUSMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVERSTATUSMANAGER_H__

#include <corelib/Filesystem.h>

namespace hmdbCommunication
{
	class IHMDBServerStatusManager
	{
	public:
		//! Struktura opisuj�ca status pingu
		struct ServerStatus
		{
			//! Adres do pingowania
			std::string serverUrl;
			//! Status serwera
			bool online;
			//! B��d pingowania je�li by�
			std::string error;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBServerStatusManager() {}
		//! \param url Adres serwera do pingowania
		//! \param caPath �cie�ka ewentualnego certyfikatu SSL je�eli wymagany
		//! \return Status serwera
		virtual const ServerStatus serverStatus(const std::string & url, const core::Filesystem::Path & caPath = core::Filesystem::Path()) const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVERSTATUSMANAGER_H__