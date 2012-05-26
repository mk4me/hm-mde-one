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

//! Struktura opisuj¹ca status pingu
struct ServerStatus
{
	//! Adres do pingowania
	std::string serverUrl;
	//! Status serwera
	bool online;
	//! B³¹d pingowania jeœli by³
	std::string error;
	//! Czas ostatniego pinga
	webservices::DateTime time;
};

//! Manager zarz¹dzaj¹cy odœwie¿aniem statusu serwerów poprzez ich pingowanie
class ServerStatusManager : public OpenThreads::ReentrantMutex
{
private:

	//! Klasa realizuj¹ca okresowe sprawdzanie statusu serwerów
	//! Jeœli poprzednie zlecenie nie dobieg³o koñca w ca³oœci to kolejne nie bêdzie realizowane
	class StatusChecker : public OpenThreads::Thread
	{
	public:
		//! Konstruktor
		//! \param manager ServersStatusManager którego serwery testujemy
		StatusChecker(ServerStatusManager * manager);
		//! Destruktor wirtualny
		virtual ~StatusChecker();

		//! Metoda periodycznie odœwie¿aj¹ca statusy serwerów
		virtual void run();
		//! Ka¿emy zakoñczyæ odœwie¿anie statusów przy najbli¿szym przejœciu pêtli
		//! Po tym wywo³aniu najpewniej zrobiê join i bêdê czeka³ na zakoñczenie!!
		void finish();

		//! \param ms OpóŸnienie po sprawdzeniu statusów serwerów
		void setCheckDelay(unsigned int ms);
		unsigned int checkDelay();

	private:
		//! ServerStatusManager dla którego odœwie¿amy statusy serwerów
		ServerStatusManager * manager;
		//! Przerwa w milisekundach pomiêdzy kolejnymi sprawdzeniami - jeœli poprzednie zlecenia nie zosta³y skoñczone kolejne nie bêd¹ generowane
		unsigned int checkDelay_;
		//! Czy koñczyæ odœwie¿anie i zabijaæ w¹tek?
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
	//! \param url Adres serwera do usuniêcia
	void removeServer(const std::string & url);

	//! \param url Adres serwera dla któego pobieramy status
	//! \return Status serwera
	ServerStatus serverStatus(const std::string & url) const;
	//! \param i Indeks serwera którego status chcemy dostaæ !!UWAGA!! indeks mo¿e siê zmianiaæ dla serwerów !!
	//! \return Status serwera
	ServerStatus serverStatus(unsigned int i) const;
	//! \return Iloœæ serweró do testowania
	unsigned int serversCount() const;
	//! \return Czy s¹ jakieœ serwery do testowania?
	bool empty() const;
	//! \param ms Przerwa pomiêdzy kolejnymi sprawdzeniami
	void setCheckDelay(unsigned int ms);
	//! \return przerwa pomiêdzy kolejnymi sprawdzeniami
	unsigned int checkDelay() const;
	//! \return Czy jeszcze odœwie¿amy status serwerów
	bool refreshingServers() const;


private:
	//! Metoda wywo³ywana w momencie zakoñczenia poprawnego pingowania
	void onPingEnd(const CommunicationManager::BasicRequestPtr & request);
	//! Metoda wywo³ana w momencie b³edu pingowania
	void onPingError(const CommunicationManager::BasicRequestPtr & request, const std::string & error);

private:
	//! Synchronizacja operacji na grupie serwerów
	mutable ServerStatusManager * mThis;
	//! Serwery dla których odœwie¿amy statusy
	std::map<std::string, ServerStatus> serverStatuses;
	//! Communication manager przez którego realizujemy statusy serwerów
	CommunicationManager * manager;
	//! Callbacki dla requestów pinga
	CommunicationManager::RequestCallbacks pingCallbacks;
	//! Iloœæ serwerów do odœwie¿enia
	unsigned int toCheck;
	//! W¹tek odœwie¿aj¹cy statusy
	core::shared_ptr<StatusChecker> statusChecker;
};


#endif	//	HEADER_GUARD___SERVERSTATUSMANAGER_H__
