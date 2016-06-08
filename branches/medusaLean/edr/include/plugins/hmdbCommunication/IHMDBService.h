/********************************************************************
	created:  2014/05/21	12:10:33
	filename: IHMDBService.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__IHMDBSERVICE_H__
#define __HEADER_GUARD_hmdbCommunication__IHMDBSERVICE_H__

#include <string>
#include <utils/Filesystem.h>
#include <networkUtils/Types.h>
#include <networkUtils/IWSDLService.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>

namespace hmdbCommunication
{
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
		//! \param caPath �cie�ka certyfikatu uwierzytelniaj�cego us�uge
		//! \param hostVerification Spos�b weryfikacji hosta
		//! \return Sesja serwis�w danych ruchu, nullptr je�eli nie uda�o si� uwierzytelni�
		//! u�ytkownika w us�ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const std::string & motionServicesUrl,
			const std::string & medicalServicesUrl,
			const std::string & user,
			const std::string & password,
			const std::string & motionDataUrl,
			const std::string & medicalDataUrl,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone) = 0;

		//! \param motion Czy sesja tylko dla motion? Je�li false to tylko medical
		//! \param servicesUrl Adres us�ug danych ruchu		
		//! \param user Nazwa uzytkownika
		//! \param password Has�o uzytkownika
		//! \param dataUrl Adres danych ruchu	
		//! \param caPath �cie�ka certyfikatu uwierzytelniaj�cego us�uge
		//! \param hostVerification Spos�b weryfikacji hosta
		//! \return Sesja serwis�w danych ruchu, nullptr je�eli nie uda�o si� uwierzytelni�
		//! u�ytkownika w us�ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const bool motion,
			const std::string & servicesUrl,			
			const std::string & user,
			const std::string & password,
			const std::string & dataUrl,			
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone) = 0;
		
		//! \param url Adres us�ugi
		//! \param user Nazwa uzytkownika
		//! \param password Has�o uzytkownika
		//! \param caPath �cie�ka certyfikatu uwierzytelniaj�cego us�uge
		//! \param hostVerification Spos�b weryfikacji hosta
		//! \param schemaPath �cie�ka do zapisu schematu serwisu
		//! \return Serwis WSDL obs�uguj�cy dan� us�ug�
		virtual const networkUtils::IWSDLServicePtr createHMDBService(
			const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path()) = 0;

		//! \param url Adres us�ugi systemowej
		//! \param caPath �cie�ka certyfikatu uwierzytelniaj�cego us�uge
		//! \param hostVerification Spos�b weryfikacji hosta
		//! \param schemaPath �cie�ka do zapisu schematu serwisu
		//! \return Serwis WSDL obs�uguj�cy dan� us�ug� systemow�
		virtual const networkUtils::IWSDLServicePtr createHMDBSystemService(
			const std::string & url,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path()) = 0;

		//! \param url Adres serwera do pingowania
		//! \param timeout Maksymalny czas oczekiwania [ms]
		//! \param caPath �cie�ka ewentualnego certyfikatu SSL je�eli wymagany
		//! \return Czy serwer odpowiedzia�
		virtual const bool serverOnline(const std::string & url,
			const unsigned int timeout = 250,
			const utils::Filesystem::Path & caPath = utils::Filesystem::Path()) const = 0;

		//! \param session Sesja us�ug bazy danych ruchu kt�r� chcemy obs�ugiwa�
		virtual void attach(IHMDBSessionPtr session) = 0;

		//! \param session Sesja us�ug bazy danych ruchu kt�r� chcemy obs�ugiwa�
		virtual void detach(IHMDBSessionPtr session) = 0;
	};
}

#endif	// __HEADER_GUARD_hmdbCommunication__IHMDBSERVICE_H__
