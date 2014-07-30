/********************************************************************
	created:  2014/05/21	12:10:33
	filename: IHMDBService.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVICE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVICE_H__

#include <string>
#include <corelib/Filesystem.h>
#include <networkUtils/Types.h>
#include <networkUtils/IWSDLService.h>

namespace hmdbCommunication
{
	//! Forward declaration
	class IHMDBSession;
	class IShallowCopyFilterManager;

	class IHMDBService
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBService() {}

		//! \param motionServicesUrl Adres us�ug danych ruchu
		//! \param medicalServicesUrl Adres us�ug danych medycznych
		//! \param user Nazwa uzytkownika
		//! \param password Has�o uzytkownika
		//! \param motionDataUrl Adres danych ruchu
		//! \param medicalDataUrl Adres danych medycznych
		//! \return Sesja serwis�w danych ruchu, nullptr je�eli nie uda�o si� uwierzytelni�
		//! u�ytkownika w us�ugach danych ruchu i medycznych
		virtual IHMDBSession * createSession(const std::string & motionServicesUrl,
			const std::string & medicalServicesUrl,
			const std::string & user,
			const std::string & password,
			const std::string & motionDataUrl,
			const std::string & medicalDataUrl,
			const core::Filesystem::Path & CAPath = core::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone) = 0;

		//! \param motion Czy sesja tylko dla motion? Je�li false to tylko medical
		//! \param servicesUrl Adres us�ug danych ruchu		
		//! \param user Nazwa uzytkownika
		//! \param password Has�o uzytkownika
		//! \param dataUrl Adres danych ruchu		
		//! \return Sesja serwis�w danych ruchu, nullptr je�eli nie uda�o si� uwierzytelni�
		//! u�ytkownika w us�ugach danych ruchu i medycznych
		virtual IHMDBSession * createSession(const bool motion,
			const std::string & servicesUrl,			
			const std::string & user,
			const std::string & password,
			const std::string & dataUrl,			
			const core::Filesystem::Path & CAPath = core::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone) = 0;

		virtual const networkUtils::IWSDLServicePtr createService(
			const std::string & url,
			const std::string & user,
			const std::string & password,
			const core::Filesystem::Path & CAPath = core::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const core::Filesystem::Path & schemaPath = core::Filesystem::Path()) = 0;

		virtual const networkUtils::IWSDLServicePtr createSystemService(
			const std::string & url,
			const core::Filesystem::Path & CAPath = core::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const core::Filesystem::Path & schemaPath = core::Filesystem::Path()) = 0;

		//! \param url Adres serwera do pingowania
		//! \param timeout Maksymalny czas oczekiwania [ms] (maksymalnie 5s)
		//! \param caPath �cie�ka ewentualnego certyfikatu SSL je�eli wymagany
		//! \return Czy serwer odpowiedzia�
		virtual const bool serverOnline(const std::string & url,
			const unsigned int timeout = 250,
			const core::Filesystem::Path & caPath = core::Filesystem::Path()) const = 0;

		//! \param session Sesja us�ug bazy danych ruchu kt�r� chcemy obs�ugiwa�
		virtual void attach(IHMDBSession * session) = 0;

		//! \param session Sesja us�ug bazy danych ruchu kt�r� chcemy obs�ugiwa�
		virtual void detach(IHMDBSession * session) = 0;

		virtual IShallowCopyFilterManager * filterManager() = 0;
		virtual const IShallowCopyFilterManager * filterManager() const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVICE_H__