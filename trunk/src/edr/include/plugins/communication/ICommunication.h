/**
@author Marek Daniluk
@brief Interfejs ICommunication dostarcza metod związanych z komunikacją klient - serwer. Za jego pomocą
możliwe jest pobieranie informacji o próbach pomiarowych przechowywanych na serwerze, oraz pobieranie ich
na lokalną maszyne.
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
	Płytka kopia bazy danych.
	*/
	virtual void copyDbData() = 0;
	/**
	Pobieranie pojedynczego pliku.
	@param fileID id pliku w bazie danych który ma być pobrany
	*/
	virtual void downloadFile(unsigned int fileID) = 0;
	/**
	Pobieranie wszystkich plików próby pomiarowej.
	@param trialID id triala w bazie danych którego pliki mają być pobrane
	*/
	virtual void downloadTrial(unsigned int trialID) = 0;

	/**
	Ładowanie listy plików do edytora.
    @param files lista plików do załadowania
	*/
	virtual void loadFiles(const std::vector<core::Filesystem::Path> files) = 0;
    /**
    Usuwanie listy plików z edytora.
    @param files lista plików do usunięcia
    */
    virtual void removeFiles(const std::vector<core::Filesystem::Path> files) = 0;
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
