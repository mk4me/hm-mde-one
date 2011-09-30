/**
@author Marek Daniluk
@brief Klasa TransportWSDL_FTPS jest klasa fasadowa dla obslugi wysylania plikow przy pomocy protokolu ftp oraz technologii wsdl.
Klasa implementuje interfejs ITransportable.
*/

#ifndef HEADER_GUARD_COMMUNICATION_TRANSPORTWSDL_FTPS_H__
#define HEADER_GUARD_COMMUNICATION_TRANSPORTWSDL_FTPS_H__

#include <plugins/communication/FileStoremanService.h>
#include <plugins/communication/FtpsConnection.h>

namespace communication
{
	class TransportWSDL_FTPS
	{
    
    protected:

        // RAII dla danych na FTP -> automatycznie zwalnia pobrane juz zasoby lub po wystapieniu bledu
        class SafeWSDL_FTPManager
        {
        public:
            SafeWSDL_FTPManager(int fileID, const core::shared_ptr<FileStoremanService> & wsdl, const std::string & file)
                : fileID(fileID), wsdl(wsdl), file(file)
            {

            }

            const std::string & getFtpFilePath() const
            {
                return file;
            }

            virtual ~SafeWSDL_FTPManager()
            {
                wsdl->downloadComplete(fileID, file);
            }

        protected:
            int fileID;
            core::shared_ptr<FileStoremanService> wsdl;
            std::string file;
        };

	protected:
		/**
		Szyfrowane po³¹czenie FTP.
		*/
        core::shared_ptr<FtpsConnection> ftp;
		/**
		Funkcjonalnoœæ web serwisu FileStoremanService.
		*/
		core::shared_ptr<FileStoremanService> wsdl;

        /**
		Fakt anulowania operacji œci¹gania plików po FTP
		*/
        bool aborted;

        unsigned int filesToDownload;

        unsigned int downloadedFiles;
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
		virtual int storeSessionFile(int sessionID, const std::string& remoteDestination, const std::string& description, const std::string& localSource);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param sessionID id sesji ktora wczesniej zostala juz umieszczona w bazie danych
		@param path sciezka do katalogu z plikami do wgrania na serwer
		@param description
		@return ???
		*/
		virtual int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrole bazy danych.
		@param performerID id performera
		@param path wzgledna sciezka do pliku na dysku serwera w stosunku do korzenia obszaru Uslugi Transportu Plikow
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual int storePerformerFile(int performerID, const std::string& remoteDestination, const std::string& description, const std::string& localSource);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param performerID id performera
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrole bazy danych.
		@param trialID id trial
		@param path wzgledna sciezka do pliku na dysku serwera w stosunku do korzenia obszaru Uslugi Transportu Plikow
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual int storeTrialFile(int trialID, const std::string& remoteDestination, const std::string& description, const std::string& localSource);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param trialID id trial
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storeTrialFiles(int trialID, const std::string& path);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param fileID
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		virtual std::string downloadFile(int fileID, const std::string& path);

		virtual int getProgress() const;

		virtual void abort();
		/**
		P³ytka kopia bazy danych.
		@return œcie¿ka do pliku xml z kopi¹ db.
		*/
		virtual void getShallowCopy(const std::string & path);
		/**
		Metadane z bazy danych.
		@return œcie¿ka do pliku xml z metadanymi.
		*/
		virtual void getMetadata(const std::string & path);

        unsigned int getFilesToDownloadCount() const
        {
            return filesToDownload;
        }

        unsigned int getDownloadedFilesCount() const
        {
            return downloadedFiles;
        }
	};
}
#endif //HEADER_GUARD_COMMUNICATION_TRANSPORTWSDL_FTPS_H__
