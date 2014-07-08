/********************************************************************
    created:  2012/02/03
    created:  3:2:2012   17:27
    filename: CommunicationDataSourceStorage.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COMMUNICATIONDATASOURCESTORAGE_H__
#define HEADER_GUARD___COMMUNICATIONDATASOURCESTORAGE_H__

#include <string>
#include <corelib/Filesystem.h>
#include "DataSourceUser.h"
#include <sqlite3.h>
#include <list>

//! Klasa zarządzająca położeniem danych naszego źródła.
//! Inicjalizacja wymaga podania ścieżki do składowania lokanego źródła danych.

//! Źródło to oparte jest na bazie danych - SQLite3.
//! Aktualnie operuje na jednej tabeli - files_table która ma 2 kolumny:
//! fileName - unikalna nazwa pliku (TEXT - wymaga tworzenia unikalnych nazw dla płytkich kopii użytkowników)
//! file - zawartość pliku (BLOB)
class DataSourceLocalStorage
{
private:
    //! Domyślny konstruktor
    //! Wszystkie ściezki powinny być ukryte/niedostępne dla użytkowników - szyfrowanie?
    DataSourceLocalStorage();

    //! Destruktor
    ~DataSourceLocalStorage();

public:

	struct FileDescription
	{
		std::string name;
		unsigned long size;
	};

	typedef std::list<FileDescription> Files;

public:

    //! \param motionDataPath Ścieżka danych ruchu
    //! \param schemasPath Ścieżka płytkich kopii danych
    //! \return Instancja DataSourceLocalStorage
	//! \param userDataPath Ścieżka do wypakowania danych użytkownika
    static DataSourceLocalStorage * create();

    //! Niszczy instację obiektu DataSourceLocalStorage
    static void destroy();
	//! \param localStorageDataPath Ścieżka do pliku z danymi - jeśli brak to tworzymy nową
	void setLocalStorageDataPath(const core::Filesystem::Path & localStorageDataPath);
	//! \return Ścieżka do pliku z danymi
	const core::Filesystem::Path & localStorageDataPath() const;

    //! \return Instancja local storage
    static DataSourceLocalStorage * instance();

	//! \param file Pliki aktualnie znajdujące się w bazie
	void listFiles(Files & files);

	//! \param fileName Unikalna nazwa pliku o który pytamy czy jest dostępny lokalnie
	//! \return Czy plik jest dostępny lokalnie
	bool fileIsLocal(const std::string & fileName) const;

	//! \param path Ścieżka do pliku który dodajemy do ogólnej puli plików
	//! \param fileUniqueName Unikalna nazwa pliku pod którym widzimy plik w puli
	void loadFile(const core::Filesystem::Path & path, const std::string & fileUniqueName = std::string());

	//! \param fileName Nazwa pliku który pobieramy z ogólnej puli plików
	//! \param destPath Ścieżka gdzie zapisujemy zadany plik
	void extractFile(const std::string & fileName, const core::Filesystem::Path & destPath);

	//! \param fileName Nazwa pliku który pobieramy z ogólnej puli plików
	void removeFile(const std::string & fileName);

private:
	//! Sprawdza czy baza jest zaszyfrowana po wykonaniu key
	bool checkIfEncrypted();
	//! Sprawdzam czy zainicjowano bazę danych
	bool checkIfInitialized();
	//! Inicjuje baze danych
	void initialize();
	//! Szyfruje bazę danych
	void encrypt(const core::Filesystem::Path & localStorageDataPath);

private:
	//! Ścieżka do pliku bazy danych
	core::Filesystem::Path localStorageDataPath_;
    //! Instancja obiektu
    static DataSourceLocalStorage * instance_;
	//! Baza danych SQLite3
	sqlite3 * db;
	//! Czy baza otwarta
	bool dbOpen;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCESTORAGE_H__
