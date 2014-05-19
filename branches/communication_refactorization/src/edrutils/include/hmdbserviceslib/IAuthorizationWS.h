/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:33
	filename: IAuthorizationWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IAUTHORIZATIONWS_H__
#define HEADER_GUARD__HMDBSERVICES_IAUTHORIZATIONWS_H__

#include <hmdbserviceslib/Export.h>
#include <string>
#include <utils/SmartPtr.h>

namespace hmdbServices
{
	//! Interfejs usługi autoryzacji użytkowników, zarządzania ich prawami do danych
	class HMDBSERVICES_EXPORT IAuthorizationWS
	{
	public:
		//! Wirtualny destruktor
		virtual ~IAuthorizationWS() {}

		//! --------------- Zarządzanie użytkownikami -------------------------

		//! \return Lista użytkowników bazy danych
		virtual const std::string listUsers() const = 0;

		//! \return Opis mojego użytkownika (wynika z konfiguracji połączenia!)
		virtual const std::string getMyUserData() const = 0;

		//! --------------- Aktualizacja konta użytkownika -------------------------

		//! \param login Login użytkownika
		//! \param email Adres email
		//! \param pass Aktualne hasło użytkownika
		//! \param newPass Nowe hasło użytkownika
		//! \param firstName Imię
		//! \param lastName Nazwisko
		virtual void updateUserAccount(const std::string & login, const std::string & email,
			const std::string & pass, const std::string & newPass,
			const std::string & firstName, const std::string & lastName) = 0;

		//! --------------- Zarządzanie uprawnieniami (grupy użytkowników i sesji) -------------------------

		//! \param grantedUserLogin Login użytkownika
		//! \param groupName Nazwa grupy użytkowników
		virtual void evokeGroupMembership(const std::string & grantedUserLogin,
			const std::string & groupName) = 0;

		//! \param grantedUserLogin Login użytkownika
		//! \param groupName Nazwa grupy użytkowników
		virtual void revokeGroupMembership(const std::string & grantedUserLogin,
			const std::string & groupName) = 0;

		//! --------------- Zarządzanie uprawnieniami (pojedyncze sesje) -------------------------

		//! \param sessionID Identyikator sesji dla którego pobieram listę praw dostępu
		//! \return Lista praw dostępu do zadanej sesji
		virtual const std::string listSessionPrivileges(const int sessionID) const = 0;

		//! \param grantedUserLogin Login użytkownika którego prawa modyfikujemy
		//! \param sessionID Identyfikator sesji której dostępność modyfikujemy
		//! \param write Czy użytkownik ma prawo modyfikować sesję
		virtual void grantSessionPrivileges(const std::string & grantedUserLogin,
			const int sessionID, const bool write) = 0;
		//! \param grantedUserLogin Login użytkownika którego prawa ograniczamy
		//! \param sessionID Identyfikator sesji której dostępność modyfikujemy
		virtual void removeSessionPrivileges(const std::string & grantedUserLogin,
			const int sessionID) = 0;
		//! \param sessionID Identyfikator sesji której dostępność modyfikujemy
		//! \param isPublic Czy sesja jest publicznie dostepna każdemu
		//! \param isWritable Czy każdy może modyfikować zadaną sesję
		virtual void alterSessionVisibility(const int sessionID, const bool isPublic,
			const bool isWritable) = 0;

		//! ---------------- Sprawdzanie istnienia konta -----------------------

		//! \return Zwraca prawdę jeżeli użytkownik tej usługi występuje w bazie danych
		virtual const bool checkMyLogin() const = 0;
		//! \return Zwraca listę grup do których przynależy mój użytkownik
		virtual const std::string listMyUserGroupsAssigned() const = 0;

		//! Zwraca ciąg znaków dla aktualizacji, kiedy podana wartość nie ulegnie zmianie
		inline static const std::string noChangeValue()
		{
			return std::string("-nochange-");
		}
	};

	typedef utils::shared_ptr<IAuthorizationWS> AuthorizationWSPtr;
	typedef utils::shared_ptr<const IAuthorizationWS> AuthorizationWSConstPtr;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IAUTHORIZATIONWS_H__
