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

//! Struktura opisuj�ca status pingu
struct ServerStatus
{
	//! Adres do pingowania
	std::string serverUrl;
	//! Status serwera
	bool online;
	//! B��d pingowania je�li by�
	std::string error;
	//! Czas ostatniego pinga
	webservices::DateTime time;
};

//! Manager zarz�dzaj�cy od�wie�aniem statusu serwer�w poprzez ich pingowanie
class ServerStatusManager : public OpenThreads::ReentrantMutex
{
private:

	//! Klasa realizuj�ca okresowe sprawdzanie statusu serwer�w
	//! Je�li poprzednie zlecenie nie dobieg�o ko�ca w ca�o�ci to kolejne nie b�dzie realizowane
	class StatusChecker : public OpenThreads::Thread
	{
	public:
		//! Konstruktor
		//! \param manager ServersStatusManager kt�rego serwery testujemy
		StatusChecker(ServerStatusManager * manager);
		//! Destruktor wirtualny
		virtual ~StatusChecker();

		//! Metoda periodycznie od�wie�aj�ca statusy serwer�w
		virtual void run();
		//! Ka�emy zako�czy� od�wie�anie status�w przy najbli�szym przej�ciu p�tli
		//! Po tym wywo�aniu najpewniej zrobi� join i b�d� czeka� na zako�czenie!!
		void finish();

		//! \param ms Op�nienie po sprawdzeniu status�w serwer�w
		void setCheckDelay(unsigned int ms);
		unsigned int checkDelay();

	private:
		//! ServerStatusManager dla kt�rego od�wie�amy statusy serwer�w
		ServerStatusManager * manager;
		//! Przerwa w milisekundach pomi�dzy kolejnymi sprawdzeniami - je�li poprzednie zlecenia nie zosta�y sko�czone kolejne nie b�d� generowane
		unsigned int checkDelay_;
		//! Czy ko�czy� od�wie�anie i zabija� w�tek?
		bool finish_;
	};

	friend class StatusChecker;

public:
	//! Konstruktor
	//! \param manager Communication Manager przez kt�rego wszystko jest realizowane
	ServerStatusManager(CommunicationManager * manager);
	//! Destruktor
	~ServerStatusManager();

	//! \param url Adres serwera do testowania do dodania
	void addServer(const std::string & url);
	//! \param url Adres serwera do usuni�cia
	void removeServer(const std::string & url);

	//! \param url Adres serwera dla kt�ego pobieramy status
	//! \return Status serwera
	ServerStatus serverStatus(const std::string & url) const;
	//! \param i Indeks serwera kt�rego status chcemy dosta� !!UWAGA!! indeks mo�e si� zmiania� dla serwer�w !!
	//! \return Status serwera
	ServerStatus serverStatus(unsigned int i) const;
	//! \return Ilo�� serwer� do testowania
	unsigned int serversCount() const;
	//! \return Czy s� jakie� serwery do testowania?
	bool empty() const;
	//! \param ms Przerwa pomi�dzy kolejnymi sprawdzeniami
	void setCheckDelay(unsigned int ms);
	//! \return przerwa pomi�dzy kolejnymi sprawdzeniami
	unsigned int checkDelay() const;
	//! \return Czy jeszcze od�wie�amy status serwer�w
	bool refreshingServers() const;


private:
	//! Metoda wywo�ywana w momencie zako�czenia poprawnego pingowania
	void onPingEnd(const CommunicationManager::BasicRequestPtr & request);
	//! Metoda wywo�ana w momencie b�edu pingowania
	void onPingError(const CommunicationManager::BasicRequestPtr & request, const std::string & error);

private:
	//! Synchronizacja operacji na grupie serwer�w
	mutable ServerStatusManager * mThis;
	//! Serwery dla kt�rych od�wie�amy statusy
	std::map<std::string, ServerStatus> serverStatuses;
	//! Communication manager przez kt�rego realizujemy statusy serwer�w
	CommunicationManager * manager;
	//! Callbacki dla request�w pinga
	CommunicationManager::RequestCallbacks pingCallbacks;
	//! Ilo�� serwer�w do od�wie�enia
	unsigned int toCheck;
	//! W�tek od�wie�aj�cy statusy
	core::shared_ptr<StatusChecker> statusChecker;
};


#endif	//	HEADER_GUARD___SERVERSTATUSMANAGER_H__
