/********************************************************************
	created:  2012/02/27
	created:  27:2:2012   13:59
	filename: AuthorizationService.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_AUTHORIZATIONSERVICE_H__
#define HEADER_GUARD__HMDBSERVICES_AUTHORIZATIONSERVICE_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/IAuthorizationWS.h>
#include <networkUtils/IWSDLService.h>

namespace hmdbServices
{
	class HMDBSERVICES_EXPORT AuthorizationWS : public IAuthorizationWS
	{
	public:
		/**
		Konstruktor klasy MotionBasicQueriesService.
		*/
		AuthorizationWS(networkUtils::IWSDLServicePtr wsdlService);
		/**
		Wirtualny destruktor klasy MotionBasicQueriesService.
		*/
		virtual ~AuthorizationWS();

		//! \return Zarejestrowani użytkownicy
		virtual const std::string listUsers() const;

		//! \return Opis mojego użytkownika (wynika z konfiguracji połączenia!)
		virtual const std::string getMyUserData() const;

		//! \param login Login użytkownika
		//! \param email Adres email
		//! \param pass Aktualne hasło użytkownika
		//! \param newPass Nowe hasło użytkownika
		//! \param firstName Imię
		//! \param lastName Nazwisko
		virtual void updateUserAccount(const std::string & login, const std::string & email,
			const std::string & pass, const std::string & newPass,
			const std::string & firstName, const std::string & lastName);

		//! \param grantedUserLogin Login użytkownika
		//! \param groupName Nazwa grupy użytkowników
		virtual void evokeGroupMembership(const std::string & grantedUserLogin,
			const std::string & groupName);

		//! \param grantedUserLogin Login użytkownika
		//! \param groupName Nazwa grupy użytkowników
		virtual void revokeGroupMembership(const std::string & grantedUserLogin,
			const std::string & groupName);

		//! \param sessionID Identyikator sesji dla którego pobieram listę praw dostępu
		//! \return Lista praw dostępu do zadanej sesji
		virtual const std::string listSessionPrivileges(const int sessionID) const;

		//! \param grantedUserLogin Login użytkownika którego prawa modyfikujemy
		//! \param sessionID Identyfikator sesji której dostępność modyfikujemy
		//! \param write Czy użytkownik ma prawo modyfikować sesję
		virtual void grantSessionPrivileges(const std::string & grantedUserLogin,
			const int sessionID, const bool write);
		//! \param grantedUserLogin Login użytkownika którego prawa ograniczamy
		//! \param sessionID Identyfikator sesji której dostępność modyfikujemy
		virtual void removeSessionPrivileges(const std::string & grantedUserLogin,
			const int sessionID);
		//! \param sessionID Identyfikator sesji której dostępność modyfikujemy
		//! \param isPublic Czy sesja jest publicznie dostepna każdemu
		//! \param isWritable Czy każdy może modyfikować zadaną sesję
		virtual void alterSessionVisibility(const int sessionID, const bool isPublic,
			const bool isWritable);

		//! \return Zwraca prawdę jeżeli użytkownik tej usługi występuje w bazie danych
		virtual const bool checkMyLogin() const;
		//! \return Zwraca listę grup do których przynależy mój użytkownik
		virtual const std::string listMyUserGroupsAssigned() const;

	private:
		networkUtils::IWSDLServicePtr wsdlService;
	};
}

#endif	//	HEADER_GUARD__HMDBSERVICES_AUTHORIZATIONSERVICE_H__
