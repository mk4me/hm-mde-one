/********************************************************************
    created:  2012/02/27
    created:  27:2:2012   13:59
    filename: AuthorizationService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___AUTHORIZATIONSERVICE_H__
#define HEADER_GUARD___AUTHORIZATIONSERVICE_H__

#include <webserviceslib/IAuthorizationWS.h>

namespace webservices
{

class AuthorizationWS : public IAuthorizationWS
{
public:
    /**
	Konstruktor klasy MotionBasicQueriesService.
	*/
	AuthorizationWS(const WSConnectionPtr & connection = WSConnectionPtr());
	/**
	Wirtualny destruktor klasy MotionBasicQueriesService.
	*/
	virtual ~AuthorizationWS();

    //! \param connection Połączenie przez które będzie realizowany serwis
    virtual void setConnection(const WSConnectionPtr & connection);
    //! \return Połączenie przez które jest realizowany serwis
    virtual const WSConnectionPtr & connection();
    //! \return Połączenie przez które jest realizowany serwis
    virtual const WSConnectionConstPtr & connection() const;

    //! \return Zwraca prawdę jeżeli użytkownik tej usługi występuje w bazie danych
    virtual const bool checkMyLogin() const;
    //! \param name Nazwa użytkownika do utworzenia
    //! \param surname Nazwisko użytkownika do utworzenia
    virtual void createUserAccount(const std::string & name, const std::string & surname);
    //! \return Zarejestrowani użytkownicy
    virtual void listUsers() const;

    //! \param sessionID Identyikator sesji dla którego pobieram listę praw dostępu
    //! \return Lista praw dostępu do zadanej sesji
    virtual void listSessionPrivileges(int sessionID) const;

    //! \param grantedUserLogin Login użytkownika którego prawa modyfikujemy
    //! \param sessionID Identyfikator sesji której dostępność modyfikujemy
    //! \param write Czy użytkownik ma prawo modyfikować sesję
    virtual void grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write);
    //! \param grantedUserLogin Login użytkownika którego prawa ograniczamy
    //! \param sessionID Identyfikator sesji której dostępność modyfikujemy
    virtual void removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID);
    //! \param sessionID Identyfikator sesji której dostępność modyfikujemy
    //! \param isPublic Czy sesja jest publicznie dostepna każdemu
    //! \param isWritable Czy każdy może modyfikować zadaną sesję
    virtual void alterSessionVisibility(int sessionID, bool isPublic, bool isWritable);

private:
	mutable OpenThreads::ReentrantMutex * mutex;
	WSConnectionPtr connection_;
	WSConnectionConstPtr constConnection_;
};

}

#endif	//	HEADER_GUARD___AUTHORIZATIONSERVICE_H__
