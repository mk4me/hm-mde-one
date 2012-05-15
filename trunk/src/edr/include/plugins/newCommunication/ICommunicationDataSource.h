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
#include <core/ISource.h>
#include <plugins/newCommunication/IDataSourceUser.h>
#include <plugins/subject/ISubject.h>
#include <plugins/newCommunication/IPatient.h>

namespace communication
{

//! Interfejs Ÿród³a danych communication
class ICommunicationDataSource : public core::ISource, public utils::Observable<ICommunicationDataSource>
{
public:
    virtual ~ICommunicationDataSource() {}

    //! \param user Nazwa u¿ytkownika
    //! \param password Has³o u¿ytkownika
    virtual void login(const std::string & user, const std::string & password) = 0;
    //! Wylogowuje u¿ytkownika
    virtual void logout() = 0;
    //! \return prawda jeœli u¿ytkownik zalogowany
    virtual bool isLogged() const = 0;
    ////! Wysy³a proœbê o za³o¿enie konta w bazie
    //virtual void sendUserRegistrationRequest() = 0;
    //! \return Dane aktualnego u¿ytkownika ( w szczególnoœci pusty obiekt jesli niezalogowano)
    virtual const IUser * currentUser() const = 0;
	//! Automatycznie ustawia widok karty pacjenta i próbuje wyœwietliæ danego pacjenta/subjecta jeœli s¹ znani,
	//! jeœli nie to czyœci selekcjê drzew i próbuje generowaæ na bazie struktury pluginu subject dane,
	//! Jesli to siê nie powiedzie nie robi nic (komunikat o b³êdzie)
	virtual void showPatientCard(const PluginSubject::SubjectConstPtr & subject, const PatientConstPtr & patient = PatientConstPtr()) = 0;
	//! Prze³ancza widok danych na dane u¿ytkownika
	virtual void showUserDataCard() = 0;
	//! Prze³ancza widok danych na konfiguracjê
	virtual void showConfigurationCard() = 0;
};

}

#endif	//	HEADER_GUARD___ICOMMUNICATIONDATASOURCE_H__
