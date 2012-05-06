/********************************************************************
    created:  2012/03/15
    created:  15:3:2012   13:19
    filename: DataSourcePathsManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEPATHSMANAGER_H__
#define HEADER_GUARD___DATASOURCEPATHSMANAGER_H__

#include <string>
#include <core/Filesystem.h>
#include <OpenThreads/ReentrantMutex>
#include "DataSourceUser.h"

//! Klasa zarz�dzaj�ca po�o�eniem danych naszego �r�d�a.
//! Inicjalizacja wymaga podania �cie�ek do sk�adowania dancyh ruchu i p�ytkiej kopii danych.
//! Reszta ga��zi jest zaszyta wewn�trz - mo�na pobra� odpowiednie �cie�ki i generowa� �cie�ki plik�w danych, p�ytkiej kopii, metadanych oraz zdj�� pacjent�w.
class DataSourcePathsManager : public OpenThreads::ReentrantMutex
{
private:

	//! Domy�lny konstruktor
	//! Wszystkie �ciezki powinny by� ukryte/niedost�pne dla u�ytkownik�w - szyfrowanie?
	//! \param schemasPath �cie�ka do p�ytkich kopii danych
	//! \param userDataPath �cie�ka do wypakowania danych u�ytkownika
	DataSourcePathsManager(const core::Filesystem::Path & usersDataPath = "./");

	//! Destruktor
	~DataSourcePathsManager();

public:

	static DataSourcePathsManager * create(const core::Filesystem::Path & usersDataPath = "./");
	static void destroy();
	static DataSourcePathsManager * instance();

	//! \param userDataPath �cie�ka do rozpakowanych danych u�ytkownika
	void setUsersDataPath(const core::Filesystem::Path & usersDataPath);
	//! \param user U�ytkownik kt�rego �cie�ki aktualnie obs�ugujemy
	void setUser(const User & user);
	//! Tworzy niezb�dne �cie�ki u�ytkownika w roocie - usersDataPath
	void createUserDataPaths() const;
	//! Czy�ci lokalny storage usera
	void removeUserDataPaths() const;

    //! \return �cie�ka do danych ruchu �ci�ganych z serwera (bazy danych)
    const core::Filesystem::Path & motionDataPath() const;
    //! \return �cie�ka do p�ytkich kopii bazy danych
    const core::Filesystem::Path & schemasPath() const;
	//! \return �cie�ka do roota rozpakowanych danych u�ytkownika
	const core::Filesystem::Path & usersDataPath() const;
	//! \return �cie�ka do rozpakowanych danych ustawionego u�ytkownika
	const core::Filesystem::Path & userDataPath() const;
    //! \return �cie�ka do plik�w zdj�� pacjent�w
    const core::Filesystem::Path & photosPath() const;
	//! \return Hash aktualnego u�ytkownika
	const std::string & userHash() const;

	//! \param user Uzytkownik dla kt�rego pobieramy hash
	//! \return Hash u�ytkownika
	static std::string generateUserHash(const User & user);

    //! \param sessionName Nazwa sesji dla kt�rej generujemy �cie�k� (ostatni cz�on �cie�ki to nazwa sesji)
    //! \return �cie�ka do sesji o podanej nazwie
    const core::Filesystem::Path sessionPath(const std::string & sessionName) const;
    //! \param fileName Nazwa pliku dla kt�rego generujemy �cie�k� (ostatni cz�on �cie�ki to nazwa pliku)
    //! \param sessionName Nazwa sesji do kt�rej nale�y zadany plik
    //! \return �cie�ka do pliku o podanej nazwie nale��cego do zadanej sesji
    const core::Filesystem::Path filePath(const std::string & fileName, const std::string & sessionName) const;
    //! \param photoName Nazwa zdj�cia dla kt�rego generujemy �cie�k�
    //! \return �cie�ka do zdj�cia o podanej nazwie
    const core::Filesystem::Path photoPath(const std::string & photoname) const;

    //! \param �cie�ka kt�r� zarz�dza DataSourceLocalStorage
    //! \return Czy �cie�ka faktycznie istnieje - katalog lub plik s� fizycznie dostepne
    static bool pathExists(const core::Filesystem::Path & path);

	//! \return �cie�ka do p�ytkiej kopii bazy danych ruchu
	const core::Filesystem::Path & projectsPath() const;

    //! \return �cie�ka do p�ytkiej kopii bazy danych ruchu
    const core::Filesystem::Path & motionShallowCopyPath() const;
    //! \return �cie�ka do metadanych ruchu
    const core::Filesystem::Path & motionMetadataPath() const;
    //! \return �cie�ka do p�ytkiej kopii bazy danych medycznych
    const core::Filesystem::Path & medicalShallowCopyPath() const;
    //! \return �cie�ka do metadanych medycznych
    const core::Filesystem::Path & medicalMetadataPath() const;
    //! \para path �cie�ka dla kt�rej pobieramy ilo�� wolnego miejsca
    //! \return Ilo�c wolnego miejsca dla zadanej �cie�ki
    static unsigned long long freeSpace(const core::Filesystem::Path & path);

private:
	//! Odbudowywyjue wszystkie sciezki u�ytkownika
	void rebuildUserPaths();
	//! Resetuje �cie�ki u�ytkownika do domy�lnych warto�ci
	void clearUserPaths();

private:
    //! -------------------- LOKALNE �CIE�KI DANYCH --------------------------
    //! �cie�ka do miejsca sk�adowania danych �ci�ganych za pomoc� CommunicationManager
    core::Filesystem::Path localDataPath;
    //! �cie�ka do schemat�w baz dancyh
    core::Filesystem::Path localSchemasPath;
	//! �cie�ka do rozpakowanych dancyh u�ytkownika
	core::Filesystem::Path localUsersDataPath;
	//! �cie�ka do rozpakowanych dancyh u�ytkownika
	core::Filesystem::Path localUserDataPath;
	//! �cie�ka do pliku z projektami u�ytkownika
	core::Filesystem::Path localProjectsPath;
    //! �ciezka do schematu bazy danych ruchu
    core::Filesystem::Path localMotionSchemasPath;
    //! �ciezka do schematu bazy danych pacjent�w
    core::Filesystem::Path localMedicalSchemasPath;
    //! �ciezka do katalogu ze zdj�ciami pacjent�w (nazwa pliku = id pacjenta => id performera)
    core::Filesystem::Path localPatientPhotosPath;
    //! �cie�ka do pliku p�ytkiej kopii bazy danych ruchu
    core::Filesystem::Path localMotionShallowCopyPath;
    //! �cie�ka do pliku metadanych dla ruchu
    core::Filesystem::Path localMotionMetadataPath;
    //! �cie�ka do pliku p�ytkiej kopii bazy danych pacjent�w
    core::Filesystem::Path localMedicalShallowCopyPath;
    //! �cie�ka do pliku metadancyh dla pacjent�w
    core::Filesystem::Path localMedicalMetadataPath;
	//! U�ytkownik k�trego �cie�ki obs�ugujemy
	User user_;
	//! Hash u�ytkownika u�ywany przy tworzeniu unikalnej przestrzeni dyskowej na dane
	std::string userHash_;

	static DataSourcePathsManager * instance_;
};

#endif	//	HEADER_GUARD___DATASOURCEPATHSMANAGER_H__