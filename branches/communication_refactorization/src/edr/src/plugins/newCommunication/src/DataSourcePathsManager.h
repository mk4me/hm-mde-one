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
#include <corelib/Filesystem.h>
#include <OpenThreads/ReentrantMutex>
#include "DataSourceUser.h"

//! Klasa zarządzająca położeniem danych naszego źródła.
//! Inicjalizacja wymaga podania ścieżek do składowania dancyh ruchu i płytkiej kopii danych.
//! Reszta gałęzi jest zaszyta wewnątrz - można pobrać odpowiednie ścieżki i generować ścieżki plików danych, płytkiej kopii, metadanych oraz zdjęć pacjentów.
class DataSourcePathsManager : public OpenThreads::ReentrantMutex
{
private:

	//! Domyślny konstruktor
	//! Wszystkie ściezki powinny być ukryte/niedostępne dla użytkowników - szyfrowanie?
	//! \param schemasPath Ścieżka do płytkich kopii danych
	//! \param userDataPath Ścieżka do wypakowania danych użytkownika
	DataSourcePathsManager(const core::Filesystem::Path & usersDataPath = "./");

	//! Destruktor
	~DataSourcePathsManager();

public:

	static DataSourcePathsManager * create(const core::Filesystem::Path & usersDataPath = "./");
	static void destroy();
	static DataSourcePathsManager * instance();

	//! \param userDataPath Ścieżka do rozpakowanych danych użytkownika
	void setUsersDataPath(const core::Filesystem::Path & usersDataPath);
	//! \param user Użytkownik którego ścieżki aktualnie obsługujemy
	void setUser(const User & user);
	//! Tworzy niezbędne ścieżki użytkownika w roocie - usersDataPath
	void createUserDataPaths() const;
	//! Czyści lokalny storage usera
	void removeUserDataPaths() const;

    //! \return Ścieżka do danych ruchu ściąganych z serwera (bazy danych)
    const core::Filesystem::Path & motionDataPath() const;
    //! \return Ścieżka do płytkich kopii bazy danych
    const core::Filesystem::Path & schemasPath() const;
	//! \return Ścieżka do roota rozpakowanych danych użytkownika
	const core::Filesystem::Path & usersDataPath() const;
	//! \return Ścieżka do rozpakowanych danych ustawionego użytkownika
	const core::Filesystem::Path & userDataPath() const;
    //! \return Ścieżka do plików zdjęć pacjentów
    const core::Filesystem::Path & photosPath() const;
	//! \return Hash aktualnego użytkownika
	const std::string & userHash() const;

	//! \param user Uzytkownik dla którego pobieramy hash
	//! \return Hash użytkownika
	static std::string generateUserHash(const User & user);

    //! \param sessionName Nazwa sesji dla której generujemy ścieżkę (ostatni człon ścieżki to nazwa sesji)
    //! \return Ścieżka do sesji o podanej nazwie
    const core::Filesystem::Path sessionPath(const std::string & sessionName) const;
    //! \param fileName Nazwa pliku dla którego generujemy ścieżkę (ostatni człon ścieżki to nazwa pliku)
    //! \param sessionName Nazwa sesji do której należy zadany plik
    //! \return Ścieżka do pliku o podanej nazwie należącego do zadanej sesji
    const core::Filesystem::Path filePath(const std::string & fileName, const std::string & sessionName) const;
    //! \param photoName Nazwa zdjęcia dla którego generujemy ścieżkę
    //! \return Ścieżka do zdjęcia o podanej nazwie
    const core::Filesystem::Path photoPath(const std::string & photoname) const;

    //! \param Ścieżka którą zarządza DataSourceLocalStorage
    //! \return Czy ścieżka faktycznie istnieje - katalog lub plik są fizycznie dostępne
    static bool pathExists(const core::Filesystem::Path & path);

	//! \return Ścieżka do płytkiej kopii bazy danych ruchu
	const core::Filesystem::Path & projectsPath() const;
    
    //! \return Ścieżka do przyrostowej płytkiej kopii bazy danych
    const core::Filesystem::Path& getLocalIncrementalBranchShallowCopyPath() const;

    //! \return Ścieżka do płytkiej kopii bazy danych ruchu
    const core::Filesystem::Path & motionShallowCopyPath() const;
    //! \return Ścieżka do metadanych ruchu
    const core::Filesystem::Path & motionMetadataPath() const;
    //! \return Ścieżka do płytkiej kopii bazy danych medycznych
    const core::Filesystem::Path & medicalShallowCopyPath() const;
    //! \return Ścieżka do metadanych medycznych
    const core::Filesystem::Path & medicalMetadataPath() const;
    //! \para path Ścieżka dla której pobieramy ilość wolnego miejsca
    //! \return Ilośc wolnego miejsca dla zadanej ścieżki
    static unsigned long long freeSpace(const core::Filesystem::Path & path);

private:
	//! Odbudowywyjue wszystkie sciezki użytkownika
	void rebuildUserPaths();
	//! Resetuje ścieżki użytkownika do domyślnych wartości
	void clearUserPaths();

private:
    //! -------------------- LOKALNE ŚCIEŻKI DANYCH --------------------------
    //! Ścieżka do miejsca składowania danych ściąganych za pomocą CommunicationManager
    core::Filesystem::Path localDataPath;
    //! Ścieżka do schematów baz dancyh
    core::Filesystem::Path localSchemasPath;
	//! Ścieżka do rozpakowanych dancyh użytkownika
	core::Filesystem::Path localUsersDataPath;
	//! Ścieżka do rozpakowanych dancyh użytkownika
	core::Filesystem::Path localUserDataPath;
	//! Ścieżka do pliku z projektami użytkownika
	core::Filesystem::Path localProjectsPath;
    //! Ściezka do schematu bazy danych ruchu
    core::Filesystem::Path localMotionSchemasPath;
    //! Ściezka do schematu bazy danych pacjentów
    core::Filesystem::Path localMedicalSchemasPath;
    //! Ściezka do katalogu ze zdjęciami pacjentów (nazwa pliku = id pacjenta => id performera)
    core::Filesystem::Path localPatientPhotosPath;
    //! Ścieżka do pliku płytkiej kopii bazy danych ruchu
    core::Filesystem::Path localMotionShallowCopyPath;
    //! Ścieżka do pliku metadanych dla ruchu
    core::Filesystem::Path localMotionMetadataPath;
    //! Ścieżka do pliku płytkiej kopii bazy danych pacjentów
    core::Filesystem::Path localMedicalShallowCopyPath;
    //! Ścieżka do pliku metadancyh dla pacjentów
    core::Filesystem::Path localMedicalMetadataPath;
    //! Ścieżka do pliku metadancyh dla pacjentów
    core::Filesystem::Path localIncrementalBranchShallowCopyPath;
    
    
    //! Użytkownik kótrego ścieżki obsługujemy
	User user_;
	//! Hash użytkownika używany przy tworzeniu unikalnej przestrzeni dyskowej na dane
	std::string userHash_;

	static DataSourcePathsManager * instance_;
};

#endif	//	HEADER_GUARD___DATASOURCEPATHSMANAGER_H__
