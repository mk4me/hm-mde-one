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
    class SimpleFileStoremanWS : public IBasicStoremanWS
    {
    public:

        SimpleFileStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~SimpleFileStoremanWS();

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

        virtual void downloadComplete(int id, const std::string & path);
        
        virtual const std::string retrieve(int id);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
    };

	//! Klasa przygotowująca zdjęcia do pobrania
    class PhotoStoremanWS : public IBasicStoremanWS
    {
    public:

        PhotoStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~PhotoStoremanWS();

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

        virtual void downloadComplete(int id, const std::string & path);

        virtual const std::string retrieve(int id);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
    };

	//! Klasa przygotowująca płytkie kopie do pobrania
    class ShallowStoremanWS : public IShallowStoremanWS
    {
    public:

        ShallowStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~ShallowStoremanWS();

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

        virtual const std::string getShallowCopy();

        virtual const std::string getMetadata();

		virtual void downloadComplete(const std::string & path);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
		SimpleFileStoremanWS fileStoremanWS;
    };

    class MotionFileStoremanWS : public IMotionFileStoremanWS
    {
    public:

        MotionFileStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~MotionFileStoremanWS();

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		virtual void downloadComplete(int fileID, const std::string& path);

		virtual void downloadComplete(const std::string & path);

		virtual const std::string retrieve(int fileID);

		virtual const std::string getShallowCopy();

        virtual const std::string getShallowCopyIncrement(const DateTime & dateTime);
		
		virtual const std::string getMetadata();
        /**
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrolę bazy danych.
		@param performerID id performera
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param performerID id performera
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		virtual const int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param trialID id trial
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param trialID id trial
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storeTrialFiles(int trialID, const std::string& path);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
		SimpleFileStoremanWS simpleFileStoremanWS_;
		ShallowStoremanWS shallowStoremanWS_;
    };

	class MedicalFileStoremanWS : public IMedicalFileStoremanWS
	{
	public:

		MedicalFileStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

		virtual ~MedicalFileStoremanWS();

		//! \param connection Połączenie przez które będzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Połączenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		virtual void downloadComplete(int fileID, const std::string& path);

		virtual void downloadComplete(const std::string & path);

		virtual const std::string retrieve(int fileID);

		virtual const std::string getShallowCopy();

		virtual const std::string getMetadata();

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
		PhotoStoremanWS photoStoremanWS_;
		ShallowStoremanWS shallowStoremanWS_;
	};
}

#endif	//	HEADER_GUARD___FILESTOREMANWS_H__
