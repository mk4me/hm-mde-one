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
#include <plugins/newCommunication/IDataSourceFilterManager.h>
#include <plugins/subject/ISubject.h>
#include <plugins/newCommunication/IPatient.h>
#include <plugins/newCommunication/IHierarchyPerspective.h>


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

    virtual void setCompactMode(bool compact = true) = 0;

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
    //! dołączenie zewnętrznej perspektywy tworzącej hierarchiczną reprezentację
    virtual void addHierarchyPerspective(communication::IHierarchyPerspectivePtr perspective) = 0;
	//! \param path Ściezka do ładowanego pliku
	//! \param trialName Trial dla ktorego łądujemy plik
	//! \return Czy udało się załadować plik
    virtual bool uploadMotionFile(const core::Filesystem::Path& path, const std::string& trialName) = 0;
	//! \param filter Filtr dla danych z bazy
	//! \return Indeks filtra po rejestracji
	virtual int addDataFilter(DataSourceFilter* filter) = 0;
	//! \param idx Indeks filtra
	virtual void setCurrentFilter(const int idx) = 0;
	//! \return Indeks aktuialnego filtra
	virtual const int currentFilter() const = 0;
	//! Odswieża aktualny filtr
	virtual void refreshCurrentFilter() = 0;
	//! \return Lista wszystkich adnotacji
	virtual const webservices::xmlWsdl::AnnotationsList getAllAnnotationStatus() const = 0;
	//! \param userID Identyfikator usera dla ktorego ustawia sie status
	//! \param trialID Identyfikator zdjęcia dla ktorego ustawia sie status
	//! \param status Status adnotacji
	//! \param comment Komentarz do adnotacji
	virtual void setAnnotationStatus(const int userID, const int motionID,
		const webservices::xmlWsdl::AnnotationStatus::Type type,
		const std::string & comment = std::string()) = 0;

	//! \return Czas ostatniej aktualizacji po stronie bazy danych
	virtual const webservices::DateTime lastUpdateTime() const = 0;
	//! \return Czy użytkownik to recenzent dla medusy
	virtual const bool userIsReviewer() const = 0;
	//! \return Użytkownicy bazy
	virtual const webservices::xmlWsdl::UsersList listUsers() const = 0;
	//! \param name Nazwa triala
	//! \return Globalny ID triala
	virtual const int trialID(const std::string & name) const = 0;
	//! \return ID wszystkich triali
	virtual const std::set<int> trialsIDs() const = 0;
};
DEFINE_SMART_POINTERS(ICommunicationDataSource);

}

#endif	//	HEADER_GUARD___ICOMMUNICATIONDATASOURCE_H__
