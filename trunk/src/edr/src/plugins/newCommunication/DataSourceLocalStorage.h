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
#include <core/Filesystem.h>
#include "DataSourceUser.h"
#include <sqlite3.h>

//! Klasa zarzπdzajπca po≥oøeniem danych naszego ürÛd≥a.
//! Inicjalizacja wymaga podania úcieøki do sk≥adowania lokanego ürÛd≥a danych.

//! èrÛd≥o to oparte jest na bazie danych - SQLite3.
//! Aktualnie operuje na jednej tabeli - files_table ktÛra ma 2 kolumny:
//! fileName - unikalna nazwa pliku (TEXT - wymaga tworzenia unikalnych nazw dla p≥ytkich kopii uøytkownikÛw)
//! file - zawartoúÊ pliku (BLOB)
class DataSourceLocalStorage
{
private:
    //! Domyúlny konstruktor
    //! Wszystkie úciezki powinny byÊ ukryte/niedostÍpne dla uøytkownikÛw - szyfrowanie?
    DataSourceLocalStorage();

    //! Destruktor
    ~DataSourceLocalStorage();

public:

    //! \param motionDataPath åcieøka danych ruchu
    //! \param schemasPath åcieøka p≥ytkich kopii danych
    //! \return Instancja DataSourceLocalStorage
	//! \param userDataPath åcieøka do wypakowania danych uøytkownika
    static DataSourceLocalStorage * create();

    //! Niszczy instacjÍ obiektu DataSourceLocalStorage
    static void destroy();
	//! \param localStorageDataPath åcieøka do pliku z danymi - jesli brak to tworzymy nowπ
	void setLocalStorageDataPath(const core::Filesystem::Path & localStorageDataPath);
	//! \return åcieøka do pliku z danymi
	const core::Filesystem::Path & localStorageDataPath() const;

    //! \return Instancja local storage
    static DataSourceLocalStorage * instance();

	//! \param fileName Unikalna nazwa pliku o ktÛry pytamy czy jest dostÍpny lokalnie
	//! \return Czy plik jest dostepny lokalnie
	bool fileIsLocal(const std::string & fileName) const;

	//! \param path åcieøka do pliku ktÛry dodajemy do ogÛlnej puli plikÛw
	//! \param fileUniqueName Unikalna nazwa pliku pod ktÛrym widzimy plik w puli
	void loadFile(const core::Filesystem::Path & path, const std::string & fileUniqueName = std::string());

	//! \param fileName Nazwa pliku ktÛry pobieramy z ogÛlnej puli plikÛw
	//! \param destPath åcieøka gdzie zapisujemy zadany plik
	void extractFile(const std::string & fileName, const core::Filesystem::Path & destPath);

	//! \param fileName Nazwa pliku ktÛry pobieramy z ogÛlnej puli plikÛw
	void removeFile(const std::string & fileName);

private:
	//! Sprawdzam czy struktura bazy danych jest ok - jeúli nie to prÛbujÍ jπ utworzyÊ (tabela files_table)
	void verifyAndRebuildDBStructure();

private:
	//! åcieøka do pliku bazy danych
	core::Filesystem::Path localStorageDataPath_;
    //! Instancja obiektu
    static DataSourceLocalStorage * instance_;
	//! Baza danych SQLite3
	sqlite3 * db;
	//! Czy baza otwarta
	bool dbOpen;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCESTORAGE_H__