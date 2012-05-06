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

//! Klasa zarz¹dzaj¹ca po³o¿eniem danych naszego Ÿród³a.
//! Inicjalizacja wymaga podania œcie¿ek do sk³adowania dancyh ruchu i p³ytkiej kopii danych.
//! Reszta ga³êzi jest zaszyta wewn¹trz - mo¿na pobraæ odpowiednie œcie¿ki i generowaæ œcie¿ki plików danych, p³ytkiej kopii, metadanych oraz zdjêæ pacjentów.
class DataSourcePathsManager : public OpenThreads::ReentrantMutex
{
private:

	//! Domyœlny konstruktor
	//! Wszystkie œciezki powinny byæ ukryte/niedostêpne dla u¿ytkowników - szyfrowanie?
	//! \param schemasPath Œcie¿ka do p³ytkich kopii danych
	//! \param userDataPath Œcie¿ka do wypakowania danych u¿ytkownika
	DataSourcePathsManager(const core::Filesystem::Path & usersDataPath = "./");

	//! Destruktor
	~DataSourcePathsManager();

public:

	static DataSourcePathsManager * create(const core::Filesystem::Path & usersDataPath = "./");
	static void destroy();
	static DataSourcePathsManager * instance();

	//! \param userDataPath Œcie¿ka do rozpakowanych danych u¿ytkownika
	void setUsersDataPath(const core::Filesystem::Path & usersDataPath);
	//! \param user U¿ytkownik którego œcie¿ki aktualnie obs³ugujemy
	void setUser(const User & user);
	//! Tworzy niezbêdne œcie¿ki u¿ytkownika w roocie - usersDataPath
	void createUserDataPaths() const;
	//! Czyœci lokalny storage usera
	void removeUserDataPaths() const;

    //! \return Œcie¿ka do danych ruchu œci¹ganych z serwera (bazy danych)
    const core::Filesystem::Path & motionDataPath() const;
    //! \return Œcie¿ka do p³ytkich kopii bazy danych
    const core::Filesystem::Path & schemasPath() const;
	//! \return Œcie¿ka do roota rozpakowanych danych u¿ytkownika
	const core::Filesystem::Path & usersDataPath() const;
	//! \return Œcie¿ka do rozpakowanych danych ustawionego u¿ytkownika
	const core::Filesystem::Path & userDataPath() const;
    //! \return Œcie¿ka do plików zdjêæ pacjentów
    const core::Filesystem::Path & photosPath() const;
	//! \return Hash aktualnego u¿ytkownika
	const std::string & userHash() const;

	//! \param user Uzytkownik dla którego pobieramy hash
	//! \return Hash u¿ytkownika
	static std::string generateUserHash(const User & user);

    //! \param sessionName Nazwa sesji dla której generujemy œcie¿kê (ostatni cz³on œcie¿ki to nazwa sesji)
    //! \return Œcie¿ka do sesji o podanej nazwie
    const core::Filesystem::Path sessionPath(const std::string & sessionName) const;
    //! \param fileName Nazwa pliku dla którego generujemy œcie¿kê (ostatni cz³on œcie¿ki to nazwa pliku)
    //! \param sessionName Nazwa sesji do której nale¿y zadany plik
    //! \return Œcie¿ka do pliku o podanej nazwie nale¿¹cego do zadanej sesji
    const core::Filesystem::Path filePath(const std::string & fileName, const std::string & sessionName) const;
    //! \param photoName Nazwa zdjêcia dla którego generujemy œcie¿kê
    //! \return Œcie¿ka do zdjêcia o podanej nazwie
    const core::Filesystem::Path photoPath(const std::string & photoname) const;

    //! \param Œcie¿ka któr¹ zarz¹dza DataSourceLocalStorage
    //! \return Czy œcie¿ka faktycznie istnieje - katalog lub plik s¹ fizycznie dostepne
    static bool pathExists(const core::Filesystem::Path & path);

	//! \return Œcie¿ka do p³ytkiej kopii bazy danych ruchu
	const core::Filesystem::Path & projectsPath() const;

    //! \return Œcie¿ka do p³ytkiej kopii bazy danych ruchu
    const core::Filesystem::Path & motionShallowCopyPath() const;
    //! \return Œcie¿ka do metadanych ruchu
    const core::Filesystem::Path & motionMetadataPath() const;
    //! \return Œcie¿ka do p³ytkiej kopii bazy danych medycznych
    const core::Filesystem::Path & medicalShallowCopyPath() const;
    //! \return Œcie¿ka do metadanych medycznych
    const core::Filesystem::Path & medicalMetadataPath() const;
    //! \para path Œcie¿ka dla której pobieramy iloœæ wolnego miejsca
    //! \return Iloœc wolnego miejsca dla zadanej œcie¿ki
    static unsigned long long freeSpace(const core::Filesystem::Path & path);

private:
	//! Odbudowywyjue wszystkie sciezki u¿ytkownika
	void rebuildUserPaths();
	//! Resetuje œcie¿ki u¿ytkownika do domyœlnych wartoœci
	void clearUserPaths();

private:
    //! -------------------- LOKALNE ŒCIE¯KI DANYCH --------------------------
    //! Œcie¿ka do miejsca sk³adowania danych œci¹ganych za pomoc¹ CommunicationManager
    core::Filesystem::Path localDataPath;
    //! Œcie¿ka do schematów baz dancyh
    core::Filesystem::Path localSchemasPath;
	//! Œcie¿ka do rozpakowanych dancyh u¿ytkownika
	core::Filesystem::Path localUsersDataPath;
	//! Œcie¿ka do rozpakowanych dancyh u¿ytkownika
	core::Filesystem::Path localUserDataPath;
	//! Œcie¿ka do pliku z projektami u¿ytkownika
	core::Filesystem::Path localProjectsPath;
    //! Œciezka do schematu bazy danych ruchu
    core::Filesystem::Path localMotionSchemasPath;
    //! Œciezka do schematu bazy danych pacjentów
    core::Filesystem::Path localMedicalSchemasPath;
    //! Œciezka do katalogu ze zdjêciami pacjentów (nazwa pliku = id pacjenta => id performera)
    core::Filesystem::Path localPatientPhotosPath;
    //! Œcie¿ka do pliku p³ytkiej kopii bazy danych ruchu
    core::Filesystem::Path localMotionShallowCopyPath;
    //! Œcie¿ka do pliku metadanych dla ruchu
    core::Filesystem::Path localMotionMetadataPath;
    //! Œcie¿ka do pliku p³ytkiej kopii bazy danych pacjentów
    core::Filesystem::Path localMedicalShallowCopyPath;
    //! Œcie¿ka do pliku metadancyh dla pacjentów
    core::Filesystem::Path localMedicalMetadataPath;
	//! U¿ytkownik kótrego œcie¿ki obs³ugujemy
	User user_;
	//! Hash u¿ytkownika u¿ywany przy tworzeniu unikalnej przestrzeni dyskowej na dane
	std::string userHash_;

	static DataSourcePathsManager * instance_;
};

#endif	//	HEADER_GUARD___DATASOURCEPATHSMANAGER_H__