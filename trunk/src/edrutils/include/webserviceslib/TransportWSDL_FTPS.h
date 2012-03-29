/**
@author Marek Daniluk
@brief Klasa MotionTransportWSDL_FTPS jest klasa fasadowa dla obslugi wysylania plikow przy pomocy protokolu ftp oraz technologii wsdl.
Klasa implementuje interfejs ITransportable.
*/

#ifndef HEADER_GUARD_COMMUNICATION_TRANSPORTWSDL_FTPS_H__
#define HEADER_GUARD_COMMUNICATION_TRANSPORTWSDL_FTPS_H__

#include <webserviceslib/FileStoremanWS.h>
#include <webserviceslib/FtpsConnection.h>
#include <boost/function.hpp>

namespace communication
{
    class TransportWSDL_FTPSBase
    {
    protected:

        typedef boost::function<void(void)> ReleaseResourcesWsdlCallback;

        // RAII dla danych na FTP -> automatycznie zwalnia pobrane juz zasoby lub po wystapieniu bledu
        class SafeWSDL_FTPManagerBase
        {
        public:
            SafeWSDL_FTPManagerBase(const ReleaseResourcesWsdlCallback & relaseResourcesCallback)
                : relaseResourcesCallback(relaseResourcesCallback)
            {

            }

            virtual ~SafeWSDL_FTPManagerBase()
            {
                //hack na potrzeby zwalniania zasobów bazy medycznej - aktualnie nie wspierane przez bazê
                try{
                    relaseResourcesCallback();
                }catch(...){
                    
                }
            }

        protected:
            ReleaseResourcesWsdlCallback relaseResourcesCallback;
        };

    protected:

        std::string errorMessage;

        /**
		Szyfrowane po³¹czenie FTP.
		*/
        core::shared_ptr<FtpsConnection> ftp;

        /**
		Funkcjonalnoœæ web serwisu MotionFileStoremanService.
		*/
		core::shared_ptr<FileStoremanServiceBase> wsdl;

    public:

        TransportWSDL_FTPSBase();
        virtual ~TransportWSDL_FTPSBase() {}

        void init(const core::shared_ptr<FileStoremanServiceBase> & wsdl)
        {
            this->wsdl = wsdl;
        }

        /**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param fileID
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		FtpsConnection::OperationStatus downloadFile(int fileID, const std::string& path, communication::FtpsConnection::IProgress * progress = nullptr);

        /**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param fileID
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		FtpsConnection::OperationStatus downloadPhoto(int photoID, const std::string& path, communication::FtpsConnection::IProgress * progress = nullptr);

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

		int getProgress() const;

		void abort();
		/**
		P³ytka kopia bazy danych ruchu.
		@return œcie¿ka do pliku xml z kopi¹ db ruchu.
		*/
		FtpsConnection::OperationStatus getShallowCopy(const std::string & path, communication::FtpsConnection::IProgress * progress = nullptr);
		/**
		Metadane z bazy danych ruchu.
		@return œcie¿ka do pliku xml z metadanymi ruchu.
		*/
		FtpsConnection::OperationStatus getMetadata(const std::string & path, communication::FtpsConnection::IProgress * progress = nullptr);

        const std::string & getLastError() const;
    };

	class MotionTransportWSDL_FTPS : public TransportWSDL_FTPSBase
	{
	protected:
		
		/**
		Funkcjonalnoœæ web serwisu MotionFileStoremanService.
		*/
		core::shared_ptr<MotionFileStoremanService> motionWsdl;

	public:
		/**
		Konstruktor klasy MotionTransportWSDL_FTPS.
		*/
		MotionTransportWSDL_FTPS();
		/**
		Wirtualny destruktor klasy MotionTransportWSDL_FTPS.
		*/
		virtual ~MotionTransportWSDL_FTPS();

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
		virtual int storePerformerFiles(int performerID, const std::string& path);
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
		virtual int storeTrialFiles(int trialID, const std::string& path);
	};

    class MedicalTransportWSDL_FTPS : public TransportWSDL_FTPSBase
    {
    public:
        MedicalTransportWSDL_FTPS();
        ~MedicalTransportWSDL_FTPS() {}

    protected:
		/**
		Funkcjonalnoœæ web serwisu MotionFileStoremanService.
		*/
		core::shared_ptr<MedicalFileStoremanService> medicalWsdl;
    };
}
#endif //HEADER_GUARD_COMMUNICATION_TRANSPORTWSDL_FTPS_H__
