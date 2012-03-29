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

    //! \param connection Po��czenie przez kt�re b�dzie realizowany serwis
    virtual void setConnection(const WSConnectionPtr & connection);
    //! \return Po��czenie przez kt�re jest realizowany serwis
    virtual const WSConnectionPtr & connection();
    //! \return Po��czenie przez kt�re jest realizowany serwis
    virtual const WSConnectionConstPtr & connection() const;

    //! \return Zwraca prawd� je�eli u�ytkownik tej us�ugi wyst�puje w bazie danych
    virtual const bool checkMyLogin() const;
    //! \param name Nazwa u�ytkownika do utworzenia
    //! \param surname Nazwisko uzytkownika do utworzenia
    virtual void createUserAccount(const std::string & name, const std::string & surname);
    //! \return Zarejestrowani u�ytkownicy
    virtual void listUsers() const;

    //! \param sessionID Identyikator sesji dla kt�rego pobieram list� praw dost�pu
    //! \return Lista praw dost�pu do zadanej sesji
    virtual void listSessionPrivileges(int sessionID) const;

    //! \param grantedUserLogin Login u�ytkownika kt�rego prawa modyfikujemy
    //! \param sessionID Identyfikator sesji kt�rej dost�pno�� modyfikujemy
    //! \param write Czy u�ytkownik ma prawo modyfikowa� sesj�
    virtual void grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write);
    //! \param grantedUserLogin Login u�ytkownika kt�rego prawa ograniczamy
    //! \param sessionID Identyfikator sesji kt�rej dost�pno�� modyfikujemy
    virtual void removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID);
    //! \param sessionID Identyfikator sesji kt�rej dost�pno�� modyfikujemy
    //! \param isPublic Czy sesja jest publicznie dostepna ka�demu
    //! \param isWritable Czy ka�dy mo�e modyfikowa� zadan� sesj�
    virtual void alterSessionVisibility(int sessionID, bool isPublic, bool isWritable);

private:
	mutable OpenThreads::ReentrantMutex * mutex;
	WSConnectionPtr connection_;
	WSConnectionConstPtr constConnection_;
};

}

#endif	//	HEADER_GUARD___AUTHORIZATIONSERVICE_H__
