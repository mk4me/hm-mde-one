/**
@author Marek Daniluk
@brief Klasa TransportWSDL_FTPS jest klasa fasadowa dla obslugi wysylania plikow przy pomocy protokolu ftp oraz technologii wsdl.
Klasa implementuje interfejs ITransportable.
*/

#ifndef _TRANSPORTWSDL_FTPS_H_
#define _TRANSPORTWSDL_FTPS_H_

#include "FileStoremanService.h"
#include "ITransportable.h"
#include "FTPS_cURL.h"

namespace communication
{

	class TransportWSDL_FTPS : public ITransportable
	{
	protected:
		/**
		Wskaznik typu FTPS_cURL pozwalajacy na korzystanie z uslug szyfrowanego polaczenia FTP.
		*/
		IFTP* ftp;
		/**
		Wskaznik na obiekt klasy FileStoremanService pozwalajacy na korzystanie z web serwisu
		zdefiniowanego do komunikacji z baza danych operujacego na plikach
		*/
		FileStoremanService* wsdl;
		/**
		Pole klasy przechowujace nazwe pliku do pobrania lub wyslania z/do serwera. Pole jest typu string.
		*/
		//std::string filename;
	public:
		/**
		Konstruktor klasy TransportWSDL_FTPS.
		*/
		TransportWSDL_FTPS();
		/**
		Wirtualny destruktor klasy TransportWSDL_FTPS.
		*/
		virtual ~TransportWSDL_FTPS();
		/**
		Metoda setWSCredentials ustala dane potrzebne do autoryzacji do web serwisu.
		@param uri adres zasobu typu string
		@param usr nazwa uzytkownika typu string
		@param pswd haslo uzytkownika typu string
		*/
		void setWSCredentials(const std::string& uri, const std::string& usr, const std::string& pswd);
		/**
		Metoda setFTPCredentials ustala dane potrzebne do autoryzacji do serwera ftp.
		@param addr adres ip lub alias hosta typu string
		@param usr nazwa uzytkownika typu string
		@param pswd haslo uzytkownika typu string
		*/
		void setFTPCredentials(const std::string& addr, const std::string& usr, const std::string& pswd);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrole bazy danych.
		@param sessionID id sesji ktora wczesniej zostala juz umieszczona w bazie danych
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param sessionID id sesji ktora wczesniej zostala juz umieszczona w bazie danych
		@param path sciezka do katalogu z plikami do wgrania na serwer
		@param description
		@return ???
		*/
		int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrole bazy danych.
		@param performerID id performera
		@param path wzgledna sciezka do pliku na dysku serwera w stosunku do korzenia obszaru Uslugi Transportu Plikow
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param performerID id performera
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrole bazy danych.
		@param trialID id trial
		@param path wzgledna sciezka do pliku na dysku serwera w stosunku do korzenia obszaru Uslugi Transportu Plikow
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param trialID id trial
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		void storeTrialFiles(int trialID, const std::string& path);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param fileID
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		const std::string downloadFile(int fileID, const std::string& path);
	};
}
#endif
