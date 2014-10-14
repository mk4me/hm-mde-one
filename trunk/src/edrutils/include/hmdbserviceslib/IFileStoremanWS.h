/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:38
	filename: IFileStoremanWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IFILESTOREMANWS_H__
#define HEADER_GUARD__HMDBSERVICES_IFILESTOREMANWS_H__

#include <string>
#include <utils/SmartPtr.h>
#include <hmdbserviceslib/Entity.h>

namespace hmdbServices
{
	//! Podstawowy interfejs przygotowujący pliki do ściągnięcia i czyszczący po zakończeniu komunikacji
	class IBasicStoremanWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~IBasicStoremanWS() {}

		/**
		Potwierdza ukończenie pobierania. Stanowi zarazem sygnał dla usługi, aby uprzatnąc plik z serwera ftp.
		@param id ID pliku do pobrania
		@param path ścieżka względna do pliku na serwerze podana w rezultacie operacji RetrieveFile
		*/
		virtual void downloadComplete(const int id, const std::string & path) = 0;

		/**
		Wydobywanie pliku z bazy danych o podanym ID do uri zwracanego przez metode.
		@param id ID pliku do pobrania
		@return ścieżka względna do pliku wraz z nazwa pliku
		*/
		virtual const xmlWsdl::FileData retrieve(const int id) = 0;
	};

	typedef utils::shared_ptr<IBasicStoremanWS> BasicStoremanWSPtr;
	typedef utils::shared_ptr<const IBasicStoremanWS> BasicStoremanWSConstPtr;

	//! Interfejs usługi zarządzającej pobieraniem/wysyłaniem plików do serwera
	class IShallowStoremanWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~IShallowStoremanWS() {}

		/**
		Płytka kopia bazy danych.
		@return ścieżka do pliku xml z kopią db na serwerze ftp.
		*/
		virtual const std::string getShallowCopy() = 0;

		/**
		Metadane z bazy danych.
		@return ścieżka do pliku xml z metadanymi na serwerze ftp.
		*/
		virtual const std::string getMetadata() = 0;

		//! \param datTime Czas od którego potrzebujemy zmian jakie zaszły w bazie danych
		//! \return Ścieżka na serwerze ftp do różnicowej kopii danych ruchu począwszy od czasu jaki zadaliśmy
		virtual const std::string getShallowCopyIncrement(const std::string & dateTime) = 0;

		//! \param datTime Czas od którego potrzebujemy zmian jakie zaszły w bazie danych
		//! \return Ścieżka na serwerze ftp do różnicowej kopii danych ruchu począwszy od czasu jaki zadaliśmy
		virtual const std::string getShallowCopyBranchesIncrement(const std::string& dateTime) = 0;

		virtual void downloadComplete(const std::string & path) = 0;
	};

	typedef utils::shared_ptr<IShallowStoremanWS> ShallowStoremanWSPtr;
	typedef utils::shared_ptr<const IShallowStoremanWS> ShallowStoremanWSConstPtr;

	//! Inerfejs obsługujący podstawowe operacje na danych instancji bazy
	class IFileStoremanWS : public IShallowStoremanWS, public IBasicStoremanWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFileStoremanWS() {}
	};

	typedef utils::shared_ptr<IFileStoremanWS> FileStoremanWSPtr;
	typedef utils::shared_ptr<const IFileStoremanWS> FileStoremanWSConstPtr;

	//! Interfejs obsługujący dane bazy ruchu
	class IMotionFileStoremanWS : public IFileStoremanWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMotionFileStoremanWS() {}

		/*
		Realizuje wprowadzenie pojedynczego pliku konfiguracji pomiarowej pod kontrolę bazy danych.
		@param mcID id konfiguracji pomiarowej która wcześniej została juz umieszczona w bazie danych
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeMeasurementConfFile(const int mcID, const std::string & path,
			const std::string & description, const std::string & filename) = 0;

		/*
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeSessionFile(const int sessionID, const std::string & path,
			const std::string & description, const std::string & filename) = 0;
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param trialID id trial
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual const int storeTrialFile(const int trialID, const std::string & path,
			const std::string & description, const std::string & filename) = 0;
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param trialID id trial
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storeTrialFiles(const int trialID, const std::string & path,
			const std::string & description) = 0;

		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param measurementConfID id konfiguracji pomiarowej
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storeMeasurementConfFiles(const int measurementConfID, const std::string & path,
			const std::string & description) = 0;

		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param sessionID id sesji
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storeSessionFiles(const int sessionID, const std::string & path,
			const std::string & description) = 0;

		//TODO
		//uzupełnić dokumentację
		//! \param fileID Identyfikator pliku, który nadpisujemy
		//! \param path Ścieżka pliku
		//! \param filename Nazwa pliku
		virtual void replaceFile(int fileID, const std::string& path, const std::string& filename) = 0;
	};

	typedef utils::shared_ptr<IMotionFileStoremanWS> MotionFileStoremanWSPtr;
	typedef utils::shared_ptr<const IMotionFileStoremanWS> MotionFileStoremanWSConstPtr;

	//! Interfejs obsługujący dane bazy medycznej
	class IMedicalFileStoremanWS : public IFileStoremanWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMedicalFileStoremanWS() {}
	};

	typedef utils::shared_ptr<IMedicalFileStoremanWS> MedicalFileStoremanWSPtr;
	typedef utils::shared_ptr<const IMedicalFileStoremanWS> MedicalFileStoremanWSConstPtr;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IFILESTOREMANWS_H__
