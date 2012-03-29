/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:38
    filename: IFileStoremanWS.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFILESTOREMANWS_H__
#define HEADER_GUARD___IFILESTOREMANWS_H__

#include <webserviceslib/IWebService.h>
#include <webserviceslib/DateTime.h>
#include <boost/shared_ptr.hpp>

namespace webservices
{
//! Podstawowy interfejs przygotowuj�cy pliki do �ci�gni�cia i czyszcz�cy po zako�czeniu komunikacji
class IBasicStoremanWS : public virtual IWebService
{
public:
    //! Destruktor wirtualny
    virtual ~IBasicStoremanWS() {}

    /**
	Potwierdza uko�czenie pobierania. Stanowi zarazem sygna� dla us�ugi, aby uprzatn�c plik z serwera ftp.
	@param id ID pliku do pobrania
	@param path �cie�ka wzgl�dna do pliku na serwerze podana w rezultacie operacji RetrieveFile
	*/
	virtual void downloadComplete(int id, const std::string & path) = 0;

    /**
	Wydobywanie pliku z bazy danych o podanym ID do uri zwracanego przez metode.
	@param id ID pliku do pobrania
	@return �cie�ka wzgl�dna do pliku wraz z nazwa pliku
	*/
	virtual const std::string retrieve(int id) = 0;
};

typedef boost::shared_ptr<IBasicStoremanWS> BasicStoremanWSPtr;
typedef boost::shared_ptr<const IBasicStoremanWS> BasicStoremanWSConstPtr;

//! Interfejs us�ugi zarz�dzaj�cej pobieraniem/wysy�aniem plik�w do serwera
class IShallowStoremanWS : public virtual IWebService
{
public:
    //! Destruktor wirtualny
    virtual ~IShallowStoremanWS() {}

    /**
	P�ytka kopia bazy danych.
	@return �cie�ka do pliku xml z kopi� db na serwerze ftp.
	*/
	virtual const std::string getShallowCopy() = 0;

	/**
	Metadane z bazy danych.
	@return �cie�ka do pliku xml z metadanymi na serwerze ftp.
	*/
	virtual const std::string getMetadata() = 0;

	virtual void downloadComplete(const std::string & path) = 0;
};

typedef boost::shared_ptr<IShallowStoremanWS> ShallowStoremanWSPtr;
typedef boost::shared_ptr<const IShallowStoremanWS> ShallowStoremanWSConstPtr;

class IFileStoremanWS : public virtual IWebService, public IShallowStoremanWS, public IBasicStoremanWS
{
public:

	virtual ~IFileStoremanWS() {}
};

typedef boost::shared_ptr<IFileStoremanWS> FileStoremanWSPtr;
typedef boost::shared_ptr<const IFileStoremanWS> FileStoremanWSConstPtr;

class IMotionFileStoremanWS : public IFileStoremanWS
{
public:

    virtual ~IMotionFileStoremanWS() {}

    //! \param datTime Czas od kt�rego potrzebujemy zmian jakie zasz�y w bazie danych
    //! \return �cie�ka na serwerze ftp do r�nicowej kopii danych ruchu pocz�wszy od czasu jaki zadali�my
    virtual const std::string getShallowCopyIncrement(const DateTime & dateTime) = 0;

    /**
	Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrol� bazy danych.
	@param performerID id performera
	@param path wzgl�dna �cie�ka do pliku na dysku serwera w stosunku do korzenia obszaru Us�ugi Transportu Plik�w
	@param description opis pliku
	@param filename nazwa pliku
	@return id pliku nadany w ramach tabeli "Plik" w bazie danych
	*/
	virtual const int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename) = 0;
	/**
	Realizuje wprowadzenie plik�w pod kontrol� bazy danych.
	@param performerID id performera
	@param path �cie�ka do katalogu z plikami do wgrania na serwer
	*/
	virtual void storePerformerFiles(int performerID, const std::string& path) = 0;
	/**
	Realizuje wprowadzenie pojedynczego pliku sesji pod kontrol� bazy danych.
	@param sessionID id sesji kt�ra wcze�niej zosta�a juz umieszczona w bazie danych
	@param path wzgl�dna �cie�ka do pliku na dysku serwera w stosunku do korzenia obszaru Us�ugi Transportu Plik�w
	@param description opis pliku
	@param filename nazwa pliku
	@return id pliku nadany w ramach tabeli "Plik" w bazie danych
	*/
	virtual const int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename) = 0;
	/**
	Realizuje wprowadzenie plik�w pod kontrol� bazy danych.
	@param sessionID id sesji kt�ra wcze�niej zosta�a juz umieszczona w bazie danych
	@param path �cie�ka do katalogu z plikami do wgrania na serwer
	@param description
	*/
	virtual const int storeSessionFiles(int sessionID, const std::string& path, const std::string& description) = 0;
	/**
	Realizuje wprowadzenie pojedynczego pliku sesji pod kontrol� bazy danych.
	@param trialID id trial
	@param path wzgl�dna �cie�ka do pliku na dysku serwera w stosunku do korzenia obszaru Us�ugi Transportu Plik�w
	@param description opis pliku
	@param filename nazwa pliku
	@return id pliku nadany w ramach tabeli "Plik" w bazie danych
	*/
	virtual const int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename) = 0;
	/**
	Realizuje wprowadzenie plik�w pod kontrol� bazy danych.
	@param trialID id trial
	@param path �cie�ka do katalogu z plikami do wgrania na serwer
	*/
	virtual void storeTrialFiles(int trialID, const std::string& path) = 0;
};

typedef boost::shared_ptr<IMotionFileStoremanWS> MotionFileStoremanWSPtr;
typedef boost::shared_ptr<const IMotionFileStoremanWS> MotionFileStoremanWSConstPtr;

class IMedicalFileStoremanWS  : public IFileStoremanWS
{
public:
	virtual ~IMedicalFileStoremanWS() {}
};

typedef boost::shared_ptr<IMedicalFileStoremanWS> MedicalFileStoremanWSPtr;
typedef boost::shared_ptr<const IMedicalFileStoremanWS> MedicalFileStoremanWSConstPtr;

}

#endif	//	HEADER_GUARD___IFILESTOREMANWS_H__
