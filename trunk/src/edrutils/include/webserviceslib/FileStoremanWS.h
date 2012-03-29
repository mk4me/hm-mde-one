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
	//! Klasa przygotowuj¹ca pliki do pobrania
    class SimpleFileStoremanWS : public IBasicStoremanWS
    {
    public:

        SimpleFileStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~SimpleFileStoremanWS();

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

        virtual void downloadComplete(int id, const std::string & path);
        
        virtual const std::string retrieve(int id);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
    };

	//! Klasa przygotowuj¹ca zdjêcia do pobrania
    class PhotoStoremanWS : public IBasicStoremanWS
    {
    public:

        PhotoStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~PhotoStoremanWS();

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

        virtual void downloadComplete(int id, const std::string & path);

        virtual const std::string retrieve(int id);

	private:
		mutable OpenThreads::ReentrantMutex * mutex;
		WSConnectionPtr connection_;
		WSConnectionConstPtr constConnection_;
    };

	//! Klasa przygotowuj¹ca p³ytkie kopie do pobrania
    class ShallowStoremanWS : public IShallowStoremanWS
    {
    public:

        ShallowStoremanWS(const WSConnectionPtr & connection = WSConnectionPtr());

        virtual ~ShallowStoremanWS();

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
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

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionConstPtr & connection() const;

		virtual void downloadComplete(int fileID, const std::string& path);

		virtual void downloadComplete(const std::string & path);

		virtual const std::string retrieve(int fileID);

		virtual const std::string getShallowCopy();

        virtual const std::string getShallowCopyIncrement(const DateTime & dateTime);
		
		virtual const std::string getMetadata();
        /**
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrolê bazy danych.
		@param performerID id performera
		@param path wzglêdna œcie¿ka do pliku na dysku serwera w stosunku do korzenia obszaru Us³ugi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolê bazy danych.
		@param performerID id performera
		@param path œcie¿ka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolê bazy danych.
		@param sessionID id sesji która wczeœniej zosta³a juz umieszczona w bazie danych
		@param path wzglêdna œcie¿ka do pliku na dysku serwera w stosunku do korzenia obszaru Us³ugi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolê bazy danych.
		@param sessionID id sesji która wczeœniej zosta³a juz umieszczona w bazie danych
		@param path œcie¿ka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		virtual const int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolê bazy danych.
		@param trialID id trial
		@param path wzglêdna œcie¿ka do pliku na dysku serwera w stosunku do korzenia obszaru Us³ugi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plików pod kontrolê bazy danych.
		@param trialID id trial
		@param path œcie¿ka do katalogu z plikami do wgrania na serwer
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

		//! \param connection Po³¹czenie przez które bêdzie realizowany serwis
		virtual void setConnection(const WSConnectionPtr & connection);
		//! \return Po³¹czenie przez które jest realizowany serwis
		virtual const WSConnectionPtr & connection();
		//! \return Po³¹czenie przez które jest realizowany serwis
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
