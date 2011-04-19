/**
@author Marek Daniluk
@brief Interfejs ICommunication dostarcza metod zwi�zanych z komunikacj� klient - serwer. Za jego pomoc�
mo�liwe jest pobieranie informacji o pr�bach pomiarowych przechowywanych na serwerze, oraz pobieranie ich
na lokaln� maszyne.
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
	P�ytka kopia bazy danych.
	*/
	virtual void copyDbData() = 0;
	/**
	Pobieranie pojedynczego pliku.
	@param fileID id pliku w bazie danych kt�ry ma by� pobrany
	*/
	virtual void downloadFile(unsigned int fileID) = 0;
	/**
	Pobieranie wszystkich plik�w pr�by pomiarowej.
	@param trialID id triala w bazie danych kt�rego pliki maj� by� pobrane
	*/
	virtual void downloadTrial(unsigned int trialID) = 0;
	/**
	�adowanie lokalnej pr�by pomiarowej do edytora.
	@param localTrial lokalna pr�ba pomiarowa do za�adowania
	*/
	virtual void loadTrial(const core::IDataManager::LocalTrial& localTrial) = 0;
	/**
	�adowanie listy plik�w do edytora.
    @param files lista plik�w do za�adowania
	*/
	virtual void loadFiles(const std::vector<core::IDataManager::Path> files) = 0;
    /**
    Usuwanie listy plik�w z edytora.
    @param files lista plik�w do usuni�cia
    */
    virtual void removeFiles(const std::vector<core::IDataManager::Path> files) = 0;
	/**
	Pingowanie serwera.
	*/
	virtual void ping() = 0;
	/**
	Przerwanie pobierania pliku lub triala.
	*/
	virtual void cancelDownloading() = 0;
};
#endif //HEADER_GUARD_COMMUNICATION_ICOMMUNICATION_H__
