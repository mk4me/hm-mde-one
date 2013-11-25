/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:31
    filename: SimpleFileStoremanWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___FILESTOREMANWS_H__
#define HEADER_GUARD___FILESTOREMANWS_H__

#include <webserviceslib/IFileStoremanWS.h>

namespace webservices
{
	//! Klasa przygotowująca pliki do pobrania
    class SimpleFileStoremanWS : public WebServiceT<IBasicStoremanWS>
    {
    public:

        SimpleFileStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~SimpleFileStoremanWS();

        virtual void downloadComplete(const int id, const std::string & path);
        
        virtual const xmlWsdl::FileData retrieve(const int id);
    };

	//! Klasa przygotowująca zdjęcia do pobrania
    class PhotoStoremanWS : public WebServiceT<IBasicStoremanWS>
    {
    public:

        PhotoStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~PhotoStoremanWS();		

        virtual void downloadComplete(const int id, const std::string & path);

        virtual const xmlWsdl::FileData retrieve(const int id);	
    };

	//! Klasa przygotowująca płytkie kopie do pobrania
    class ShallowStoremanWS : public WebServiceT<IShallowStoremanWS>
    {
    public:

        ShallowStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~ShallowStoremanWS();	

        virtual const std::string getShallowCopy();

        virtual const std::string getMetadata();

		virtual void downloadComplete(const std::string & path);	
    };

    class MotionFileStoremanWS : public WebServiceT<IMotionFileStoremanWS>
    {
    public:

        MotionFileStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~MotionFileStoremanWS();

		virtual void downloadComplete(const int fileID, const std::string & path);

		virtual void downloadComplete(const std::string & path);

		virtual const xmlWsdl::FileData retrieve(const int fileID);

		virtual const std::string getShallowCopy();

        virtual const std::string getShallowCopyIncrement(const DateTime & dateTime);
		
		virtual const std::string getMetadata();
        
		virtual const int storeMeasurementConfFile(const int mcID, const std::string & path,
			const std::string & description, const std::string & filename);

		virtual void storeMeasurementConfFiles(const int measurementConfID,
			const std::string & path, const std::string & description);

		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeSessionFile(const int sessionID, const std::string & path,
			const std::string & description, const std::string & filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		virtual void storeSessionFiles(const int sessionID, const std::string & path,
			const std::string & description);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param trialID id trial
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeTrialFile(const int trialID, const std::string & path,
			const std::string & description, const std::string & filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param trialID id trial
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storeTrialFiles(const int trialID, const std::string & path,
			const std::string & description);
    };

	class MedicalFileStoremanWS : public WebServiceT<IMedicalFileStoremanWS>
	{
	public:

		MedicalFileStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

		virtual ~MedicalFileStoremanWS();	

		virtual void downloadComplete(const int fileID, const std::string & path);

		virtual void downloadComplete(const std::string & path);

		virtual const xmlWsdl::FileData retrieve(const int fileID);

		virtual const std::string getShallowCopy();

		virtual const std::string getMetadata();	
	};
}

#endif	//	HEADER_GUARD___FILESTOREMANWS_H__
