/**
@author Marek Daniluk
@brief Klasa MotionFileStoremanService dziedzicząca po WsdlConnection wykonująca operacje dokumentu MotionFileStoremanService.wsdl.
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
		Potwierdza ukończenie pobierania. Stanowi zarazem sygnał dla usługi, aby uprzatnąc plik z serwera ftp.
		@param fileID id pobranego pliku
		@param path ścieżka względna do pliku na serwerze podana w rezultacie operacji RetrieveFile
		*/
		void fileDownloadComplete(int fileID, const std::string& path);

		/**
		Wydobywanie pliku z bazy danych o podanym ID do uri zwracanego przez metode.
		@param fileID id pliku do pobrania
		@return ścieżka względna do pliku wraz z nazwa pliku
		*/
		std::string retrieveFile(int fileID);

        std::string retrievePhoto(int photoID);
        void photoDownloadComplete(int fileID, const std::string& path);

        /**
		Płytka kopia bazy danych.
		@return ścieżka do pliku xml z kopią db.
		*/
		std::string getShallowCopy();
		/**
		Metadane z bazy danych.
		@return ścieżka do pliku xml z metadanymi.
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
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrolę bazy danych.
		@param performerID id performera
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param performerID id performera
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param trialID id trial
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param trialID id trial
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		void storeTrialFiles(int trialID, const std::string& path);
		/**
		Płytka kopia bazy danych.
		@return ścieżka do pliku xml z kopią db.
		*/
		std::string getShallowCopy();
		/**
		Metadane z bazy danych.
		@return ścieżka do pliku xml z metadanymi.
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
