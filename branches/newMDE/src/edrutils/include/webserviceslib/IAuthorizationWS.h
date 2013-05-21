/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:33
    filename: IAuthorizationWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IAUTHORIZATIONWS_H__
#define HEADER_GUARD___IAUTHORIZATIONWS_H__

#include <webserviceslib/IWebService.h>
#include <webserviceslib/Entity.h>

namespace webservices
{

//! Interfejs usługi autoryzacji użytkowników, zarządzania ich prawami do danych
class IAuthorizationWS : public IWebService
{
public:
    //! Wirtualny destruktor
    virtual ~IAuthorizationWS() {}

    //! \return Zwraca prawdę jeżeli użytkownik tej usługi występuje w bazie danych
    virtual const bool checkMyLogin() const = 0;
    //! \param name Nazwa użytkownika do utworzenia
    //! \param surname Nazwisko użytkownika do utworzenia
    virtual void createUserAccount(const std::string & name, const std::string & surname) = 0;
    //! \return Zarejestrowani użytkownicy
    virtual void listUsers() const = 0;
    
    //! \param sessionID Identyikator sesji dla którego pobieram listę praw dostępu
    //! \return Lista praw dostępu do zadanej sesji
    virtual void listSessionPrivileges(int sessionID) const = 0;

    //! \param grantedUserLogin Login użytkownika którego prawa modyfikujemy
    //! \param sessionID Identyfikator sesji której dostępność modyfikujemy
    //! \param write Czy użytkownik ma prawo modyfikować sesję
    virtual void grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write) = 0;
    //! \param grantedUserLogin Login użytkownika którego prawa ograniczamy
    //! \param sessionID Identyfikator sesji której dostępność modyfikujemy
    virtual void removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID) = 0;
    //! \param sessionID Identyfikator sesji której dostępność modyfikujemy
    //! \param isPublic Czy sesja jest publicznie dostepna każdemu
    //! \param isWritable Czy każdy może modyfikować zadaną sesję
    virtual void alterSessionVisibility(int sessionID, bool isPublic, bool isWritable) = 0;
};

typedef boost::shared_ptr<IAuthorizationWS> AuthorizationWSPtr;
typedef boost::shared_ptr<const IAuthorizationWS> AuthorizationWSConstPtr;

}

#endif	//	HEADER_GUARD___IAUTHORIZATIONWS_H__
