/**
@author Marek Daniluk
@brief Klasa FileStoremanService dziedziczaca po EDRWSDL wykonujaca operacje dokumentu FileStoremanService.wsdl.
Klasa powstala na bazie dokumentacji i kodu zrodlowego zespolu bazodanowego.
*/

#ifndef _FILESTOREMANSERVICE_H_
#define _FILESTOREMANSERVICE_H_

#include "WSDL_Wsdlpull.h"
#include "ITransportable.h"

namespace communication
{

	class FileStoremanService : public WSDL_Wsdlpull
	{
	protected:
	public:
		/**
		Konstruktor klasy FileStoremanService.
		*/
		FileStoremanService();
		/**
		Wirtualny destruktor klasy FileStoremanService.
		*/
		virtual ~FileStoremanService();
		/**
		Potwierdza ukonczenie pobierania. Stanowi zarazem sygnal dla uslugi, aby uprzatnac plik.
		@param fileID id pobranego pliku
		@param path sciezka wzgledna do pliku na serwerze podana w rezultacie RetrieveFile
		*/
		void downloadComplete(int fileID, const std::string& path);
		/**
		Wydobywanie pliku z bazy danych o podanym ID do uri zwracanego przez metode.
		@param fileID id pliku do pobrania
		@return sciezka wzgledna do pliku wraz z nazwa pliku
		*/
		const std::string& retrieveFile(int fileID);
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
		@param sessionID id sesji ktora wczesniej zostala juz umieszczona w bazie danych
		@param path wzgledna sciezka do pliku na dysku serwera w stosunku do korzenia obszaru Uslugi Transportu Plikow
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
		*/
		int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
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
	};
}
#endif
