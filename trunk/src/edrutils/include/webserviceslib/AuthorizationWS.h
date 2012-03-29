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

    //! \param connection Po³¹czenie przez które bêdzie realizowany serwis
    virtual void setConnection(const WSConnectionPtr & connection);
    //! \return Po³¹czenie przez które jest realizowany serwis
    virtual const WSConnectionPtr & connection();
    //! \return Po³¹czenie przez które jest realizowany serwis
    virtual const WSConnectionConstPtr & connection() const;

    //! \return Zwraca prawdê je¿eli u¿ytkownik tej us³ugi wystêpuje w bazie danych
    virtual const bool checkMyLogin() const;
    //! \param name Nazwa u¿ytkownika do utworzenia
    //! \param surname Nazwisko uzytkownika do utworzenia
    virtual void createUserAccount(const std::string & name, const std::string & surname);
    //! \return Zarejestrowani u¿ytkownicy
    virtual void listUsers() const;

    //! \param sessionID Identyikator sesji dla którego pobieram listê praw dostêpu
    //! \return Lista praw dostêpu do zadanej sesji
    virtual void listSessionPrivileges(int sessionID) const;

    //! \param grantedUserLogin Login u¿ytkownika którego prawa modyfikujemy
    //! \param sessionID Identyfikator sesji której dostêpnoœæ modyfikujemy
    //! \param write Czy u¿ytkownik ma prawo modyfikowaæ sesjê
    virtual void grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write);
    //! \param grantedUserLogin Login u¿ytkownika którego prawa ograniczamy
    //! \param sessionID Identyfikator sesji której dostêpnoœæ modyfikujemy
    virtual void removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID);
    //! \param sessionID Identyfikator sesji której dostêpnoœæ modyfikujemy
    //! \param isPublic Czy sesja jest publicznie dostepna ka¿demu
    //! \param isWritable Czy ka¿dy mo¿e modyfikowaæ zadan¹ sesjê
    virtual void alterSessionVisibility(int sessionID, bool isPublic, bool isWritable);

private:
	mutable OpenThreads::ReentrantMutex * mutex;
	WSConnectionPtr connection_;
	WSConnectionConstPtr constConnection_;
};

}

#endif	//	HEADER_GUARD___AUTHORIZATIONSERVICE_H__
