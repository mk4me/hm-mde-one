/**
@author Marek Daniluk
@brief Interfejs ICommunication dostarcza metod zwi¹zanych z komunikacj¹ klient - serwer. Za jego pomoc¹
mo¿liwe jest pobieranie informacji o próbach pomiarowych przechowywanych na serwerze, oraz pobieranie ich
na lokaln¹ maszyne.
*/

#ifndef HEADER_GUARD_COMMUNICATION_ICOMMUNICATION_H__
#define HEADER_GUARD_COMMUNICATION_ICOMMUNICATION_H__

#include <core/IDataManager.h>

class ICommunication
{
public:
	/**
	Wirtualny destruktor
	*/
    virtual ~ICommunication() {}
	/**
	Pobieranie informacji o próbach pomiarowych z serwera.
	*/
	virtual void updateSessionContents() = 0;
	/**
	P³ytka kopia bazy danych.
	*/
	virtual void updateShallowCopy() = 0;
	/**
	Metadane z bazy danych.
	*/
	virtual void updateMetadata() = 0;
	/**
	Pobieranie pojedynczego pliku.
	@param fileID id pliku w bazie danych który ma byæ pobrany
	*/
	virtual void downloadFile(unsigned int fileID) = 0;
	/**
	Pobieranie wszystkich plików próby pomiarowej.
	@param trialID id triala w bazie danych którego pliki maj¹ byæ pobrane
	*/
	virtual void downloadTrial(unsigned int trialID) = 0;
	/**
	£adowanie lokalnej próby pomiarowej do edytora.
	@param localTrial lokalna próba pomiarowa do za³adowania
	*/
	virtual void loadTrial(const core::IDataManager::LocalTrial& localTrial) = 0;
	/**
	Pingowanie serwera.
	*/
	virtual void ping() = 0;
	/**
	Przerwanie pobierania pliku lub triala.
	*/
	virtual void cancelDownloading() = 0;
};
#endif
