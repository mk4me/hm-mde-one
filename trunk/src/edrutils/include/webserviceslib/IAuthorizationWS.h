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

//! Interfejs us³ugi autoryzacji u¿ytkowników, zarz¹dzania ich prawami do danych
class IAuthorizationWS : public IWebService
{
public:
    //! Wirtualny destruktor
    virtual ~IAuthorizationWS() {}

    //! \return Zwraca prawdê je¿eli u¿ytkownik tej us³ugi wystêpuje w bazie danych
    virtual const bool checkMyLogin() const = 0;
    //! \param name Nazwa u¿ytkownika do utworzenia
    //! \param surname Nazwisko uzytkownika do utworzenia
    virtual void createUserAccount(const std::string & name, const std::string & surname) = 0;
    //! \return Zarejestrowani u¿ytkownicy
    virtual void listUsers() const = 0;
    
    //! \param sessionID Identyikator sesji dla którego pobieram listê praw dostêpu
    //! \return Lista praw dostêpu do zadanej sesji
    virtual void listSessionPrivileges(int sessionID) const = 0;

    //! \param grantedUserLogin Login u¿ytkownika którego prawa modyfikujemy
    //! \param sessionID Identyfikator sesji której dostêpnoœæ modyfikujemy
    //! \param write Czy u¿ytkownik ma prawo modyfikowaæ sesjê
    virtual void grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write) = 0;
    //! \param grantedUserLogin Login u¿ytkownika którego prawa ograniczamy
    //! \param sessionID Identyfikator sesji której dostêpnoœæ modyfikujemy
    virtual void removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID) = 0;
    //! \param sessionID Identyfikator sesji której dostêpnoœæ modyfikujemy
    //! \param isPublic Czy sesja jest publicznie dostepna ka¿demu
    //! \param isWritable Czy ka¿dy mo¿e modyfikowaæ zadan¹ sesjê
    virtual void alterSessionVisibility(int sessionID, bool isPublic, bool isWritable) = 0;
};

typedef boost::shared_ptr<IAuthorizationWS> AuthorizationWSPtr;
typedef boost::shared_ptr<const IAuthorizationWS> AuthorizationWSConstPtr;

}

#endif	//	HEADER_GUARD___IAUTHORIZATIONWS_H__
