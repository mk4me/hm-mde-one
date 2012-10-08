/********************************************************************
    created:  2012/02/16
    created:  16:2:2012   11:01
    filename: IDataSource.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ICOMMUNICATIONDATASOURCE_H__
#define HEADER_GUARD___ICOMMUNICATIONDATASOURCE_H__

#include <utils/ObserverPattern.h>
#include <plugins/newCommunication/IDataSourceUser.h>
#include <plugins/subject/ISubject.h>
#include <plugins/newCommunication/IPatient.h>

namespace communication
{

//! Interfejs źródła danych communication
class ICommunicationDataSource : public utils::Observable<ICommunicationDataSource>
{
public:
	//! Destruktor wirtualny
    virtual ~ICommunicationDataSource() {}

	//! \param offline Czy źródło danych ma działać w trybie offline?
	virtual void setOfflineMode(bool offline = true) = 0;
	//! \return Czy źródło działa w trybie offline
	virtual bool offlineMode() const = 0;

    //! \param user Nazwa użytkownika
    //! \param password Hasło użytkownika
	//! \param localLogin Czy logujemy tylko lokalnie - dla problemów lub braku połaczenia z internetem
    virtual void login(const std::string & user, const std::string & password) = 0;
    //! Wylogowuje użytkownika
    virtual void logout() = 0;
    //! \return prawda jeśli użytkownik zalogowany
    virtual bool isLogged() const = 0;
    ////! Wysyła prośbę o założenie konta w bazie
    //virtual void sendUserRegistrationRequest() = 0;
    //! \return Dane aktualnego użytkownika ( w szczególności pusty obiekt jeśli niezalogowano)
    virtual const IUser * currentUser() const = 0;
	//! Automatycznie ustawia widok karty pacjenta i próbuje wyświetlić danego pacjenta/subjecta jeśli są znani,
	//! jeśli nie to czyści selekcję drzew i próbuje generować na bazie struktury pluginu subject dane,
	//! jeśli to się nie powiedzie nie robi nic (komunikat o błędzie)
	virtual void showPatientCard(const PluginSubject::SubjectConstPtr & subject, const PatientConstPtr & patient = PatientConstPtr()) = 0;
	//! Przełancza widok danych na dane użytkownika
	virtual void showUserDataCard() = 0;
	//! Przełancza widok danych na konfigurację
	virtual void showConfigurationCard() = 0;
};

}

#endif	//	HEADER_GUARD___ICOMMUNICATIONDATASOURCE_H__
