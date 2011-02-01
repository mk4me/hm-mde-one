/**
@author Marek Daniluk
@brief Klasa FileStoremanService dziedzicz�ca po WSDL_Wsdlpull wykonuj�ca operacje dokumentu FileStoremanService.wsdl.
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
		Potwierdza uko�czenie pobierania. Stanowi zarazem sygna� dla us�ugi, aby uprzatn�c plik z serwera ftp.
		@param fileID id pobranego pliku
		@param path �cie�ka wzgl�dna do pliku na serwerze podana w rezultacie operacji RetrieveFile
		*/
		void downloadComplete(int fileID, const std::string& path);
		/**
		Wydobywanie pliku z bazy danych o podanym ID do uri zwracanego przez metode.
		@param fileID id pliku do pobrania
		@return �cie�ka wzgl�dna do pliku wraz z nazwa pliku
		*/
		const std::string& retrieveFile(int fileID);
		/**
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrol� bazy danych.
		@param performerID id performera
		@param path wzgl�dna �cie�ka do pliku na dysku serwera w stosunku do korzenia obszaru Us�ugi Transportu Plik�w
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plik�w pod kontrol� bazy danych.
		@param performerID id performera
		@param path �cie�ka do katalogu z plikami do wgrania na serwer
		*/
		void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrol� bazy danych.
		@param sessionID id sesji kt�ra wcze�niej zosta�a juz umieszczona w bazie danych
		@param path wzgl�dna �cie�ka do pliku na dysku serwera w stosunku do korzenia obszaru Us�ugi Transportu Plik�w
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plik�w pod kontrol� bazy danych.
		@param sessionID id sesji kt�ra wcze�niej zosta�a juz umieszczona w bazie danych
		@param path �cie�ka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrol� bazy danych.
		@param trialID id trial
		@param path wzgl�dna �cie�ka do pliku na dysku serwera w stosunku do korzenia obszaru Us�ugi Transportu Plik�w
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plik�w pod kontrol� bazy danych.
		@param trialID id trial
		@param path �cie�ka do katalogu z plikami do wgrania na serwer
		*/
		void storeTrialFiles(int trialID, const std::string& path);
	};
}
#endif