/********************************************************************
    created:  2012/05/25
    created:  25:5:2012   11:52
    filename: ServerStatusManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___SERVERSTATUSMANAGER_H__
#define HEADER_GUARD___SERVERSTATUSMANAGER_H__

#include "CommunicationManager.h"
#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/Thread>
#include <webserviceslib/DateTime.h>

//! Struktura opisująca status pingu
struct ServerStatus
{
	//! Adres do pingowania
	std::string serverUrl;
	//! Status serwera
	bool online;
	//! Błąd pingowania jeśli był
	std::string error;
	//! Czas ostatniego pinga
	webservices::DateTime time;
};

//! Manager zarządzający odświeżaniem statusu serwerów poprzez ich pingowanie
class ServerStatusManager : public OpenThreads::ReentrantMutex
{
private:

	//! Klasa realizująca okresowe sprawdzanie statusu serwerów
	//! Jeśli poprzednie zlecenie nie dobiegło końca w całości to kolejne nie będzie realizowane
	class StatusChecker : public OpenThreads::Thread
	{
	public:
		//! Konstruktor
		//! \param manager ServersStatusManager którego serwery testujemy
		StatusChecker(ServerStatusManager * manager);
		//! Destruktor wirtualny
		virtual ~StatusChecker();

		//! Metoda periodycznie odświeżająca statusy serwerów
		virtual void run();
		//! Każemy zakończyć odświeżanie statusów przy najbliższym przejściu pętli
		//! Po tym wywołaniu najpewniej zrobię join i będę czekał na zakończenie!!
		void finish();

		//! \param ms Opóźnienie po sprawdzeniu statusów serwerów
		void setCheckDelay(unsigned int ms);
		unsigned int checkDelay();

	private:
		//! ServerStatusManager dla którego odświeżamy statusy serwerów
		ServerStatusManager * manager;
		//! Przerwa w milisekundach pomiędzy kolejnymi sprawdzeniami - jeśli poprzednie zlecenia nie zostały skończone kolejne nie będą generowane
		unsigned int checkDelay_;
		//! Czy kończyć odświeżanie i zabijać wątek?
		bool finish_;
	};

	friend class StatusChecker;

public:
	//! Konstruktor
	//! \param manager Communication Manager przez którego wszystko jest realizowane
	ServerStatusManager(CommunicationManager * manager);
	//! Destruktor
	~ServerStatusManager();

	//! \param url Adres serwera do testowania do dodania
	void addServer(const std::string & url);
	//! \param url Adres serwera do usunięcia
	void removeServer(const std::string & url);

	//! \param url Adres serwera dla któego pobieramy status
	//! \return Status serwera
	ServerStatus serverStatus(const std::string & url) const;
	//! \param i Indeks serwera którego status chcemy dostać !!UWAGA!! indeks może się zmianiać dla serwerów !!
	//! \return Status serwera
	ServerStatus serverStatus(unsigned int i) const;
	//! \return Ilość serweró do testowania
	unsigned int serversCount() const;
	//! \return Czy są jakieś serwery do testowania?
	bool empty() const;
	//! \param ms Przerwa pomiędzy kolejnymi sprawdzeniami
	void setCheckDelay(unsigned int ms);
	//! \return przerwa pomiędzy kolejnymi sprawdzeniami
	unsigned int checkDelay() const;
	//! \return Czy jeszcze odświeżamy status serwerów
	bool refreshingServers() const;


private:
	//! Metoda wywoływana w momencie zakończenia poprawnego pingowania
	void onPingEnd(const CommunicationManager::BasicRequestPtr & request);
	//! Metoda wywołana w momencie błedu pingowania
	void onPingError(const CommunicationManager::BasicRequestPtr & request, const std::string & error);

private:
	//! Synchronizacja operacji na grupie serwerów
	mutable ServerStatusManager * mThis;
	//! Serwery dla których odświeżamy statusy
	std::map<std::string, ServerStatus> serverStatuses;
	//! Communication manager przez którego realizujemy statusy serwerów
	CommunicationManager * manager;
	//! Callbacki dla requestów pinga
	CommunicationManager::RequestCallbacks pingCallbacks;
	//! Ilość serwerów do odświeżenia
	unsigned int toCheck;
	//! Wątek odświeżający statusy
	utils::shared_ptr<StatusChecker> statusChecker;
};


#endif	//	HEADER_GUARD___SERVERSTATUSMANAGER_H__
