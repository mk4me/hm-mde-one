/**
@author Marek Daniluk
@brief Klasa MotionFileStoremanService dziedzicz¹ca po WsdlConnection wykonuj¹ca operacje dokumentu MotionFileStoremanService.wsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_FILESTOREMANSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_FILESTOREMANSERVICE_H__

#include <plugins/communication/WsdlConnection.h>

namespace communication
{
    class FileStoremanServiceBase : public WsdlConnection
    {
    public:
        FileStoremanServiceBase() {}
        virtual ~FileStoremanServiceBase() {}
        /**
		Potwierdza ukoñczenie pobierania. Stanowi zarazem sygna³ dla us³ugi, aby uprzatn¹c plik z serwera ftp.
		@param fileID id pobranego pliku
		@param path œcie¿ka wzglêdna do pliku na serwerze podana w rezultacie operacji RetrieveFile
		*/
		void fileDownloadComplete(int fileID, const std::string& path);

		/**
		Wydobywanie pliku z bazy danych o podanym ID do uri zwracanego przez metode.
		@param fileID id pliku do pobrania
		@return œcie¿ka wzglêdna do pliku wraz z nazwa pliku
		*/
		std::string retrieveFile(int fileID);

        std::string retrievePhoto(int photoID);
        void photoDownloadComplete(int fileID, const std::string& path);

        /**
		P³ytka kopia bazy danych.
		@return œcie¿ka do pliku xml z kopi¹ db.
		*/
		std::string getShallowCopy();
		/**
		Metadane z bazy danych.
		@return œcie¿ka do pliku xml z metadanymi.
		*/
		std::string getMetadata();
    };

	class MotionFileStoremanService : public FileStoremanServiceBase
	{
	public:
		/**
		Konstruktor klasy MotionFileStoremanService.
		*/
		MotionFileStoremanService() {}
		/**
		Wirtualny destruktor klasy MotionFileStoremanService.
		*/
		virtual ~MotionFileStoremanService() {}
		
		/**
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrolê bazy danych.
		@param performerID id performera
		@param path wzglêdna œcie¿ka do pliku na dysku serwera w stosunku do korzenia obszaru Us³ugi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolê bazy danych.
		@param performerID id performera
		@param path œcie¿ka do katalogu z plikami do wgrania na serwer
		*/
		void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolê bazy danych.
		@param sessionID id sesji która wczeœniej zosta³a juz umieszczona w bazie danych
		@param path wzglêdna œcie¿ka do pliku na dysku serwera w stosunku do korzenia obszaru Us³ugi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolê bazy danych.
		@param sessionID id sesji która wczeœniej zosta³a juz umieszczona w bazie danych
		@param path œcie¿ka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolê bazy danych.
		@param trialID id trial
		@param path wzglêdna œcie¿ka do pliku na dysku serwera w stosunku do korzenia obszaru Us³ugi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolê bazy danych.
		@param trialID id trial
		@param path œcie¿ka do katalogu z plikami do wgrania na serwer
		*/
		void storeTrialFiles(int trialID, const std::string& path);
		/**
		P³ytka kopia bazy danych.
		@return œcie¿ka do pliku xml z kopi¹ db.
		*/
		std::string getShallowCopy();
		/**
		Metadane z bazy danych.
		@return œcie¿ka do pliku xml z metadanymi.
		*/
		std::string getMetadata();
	};

    class MedicalFileStoremanService : public FileStoremanServiceBase
    {
    public:
        /**
		Konstruktor klasy MedicalFileStoremanService.
		*/
		MedicalFileStoremanService() {}
		/**
		Wirtualny destruktor klasy MedicalFileStoremanService.
		*/
        virtual ~MedicalFileStoremanService() {}
    };
}
#endif //HEADER_GUARD_COMMUNICATION_FILESTOREMANSERVICE_H__
