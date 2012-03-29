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

//! Interfejs us�ugi autoryzacji u�ytkownik�w, zarz�dzania ich prawami do danych
class IAuthorizationWS : public IWebService
{
public:
    //! Wirtualny destruktor
    virtual ~IAuthorizationWS() {}

    //! \return Zwraca prawd� je�eli u�ytkownik tej us�ugi wyst�puje w bazie danych
    virtual const bool checkMyLogin() const = 0;
    //! \param name Nazwa u�ytkownika do utworzenia
    //! \param surname Nazwisko uzytkownika do utworzenia
    virtual void createUserAccount(const std::string & name, const std::string & surname) = 0;
    //! \return Zarejestrowani u�ytkownicy
    virtual void listUsers() const = 0;
    
    //! \param sessionID Identyikator sesji dla kt�rego pobieram list� praw dost�pu
    //! \return Lista praw dost�pu do zadanej sesji
    virtual void listSessionPrivileges(int sessionID) const = 0;

    //! \param grantedUserLogin Login u�ytkownika kt�rego prawa modyfikujemy
    //! \param sessionID Identyfikator sesji kt�rej dost�pno�� modyfikujemy
    //! \param write Czy u�ytkownik ma prawo modyfikowa� sesj�
    virtual void grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write) = 0;
    //! \param grantedUserLogin Login u�ytkownika kt�rego prawa ograniczamy
    //! \param sessionID Identyfikator sesji kt�rej dost�pno�� modyfikujemy
    virtual void removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID) = 0;
    //! \param sessionID Identyfikator sesji kt�rej dost�pno�� modyfikujemy
    //! \param isPublic Czy sesja jest publicznie dostepna ka�demu
    //! \param isWritable Czy ka�dy mo�e modyfikowa� zadan� sesj�
    virtual void alterSessionVisibility(int sessionID, bool isPublic, bool isWritable) = 0;
};

typedef boost::shared_ptr<IAuthorizationWS> AuthorizationWSPtr;
typedef boost::shared_ptr<const IAuthorizationWS> AuthorizationWSConstPtr;

}

#endif	//	HEADER_GUARD___IAUTHORIZATIONWS_H__
