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

//! Klasa zarz�dzaj�ca po�o�eniem danych naszego �r�d�a.
//! Inicjalizacja wymaga podania �cie�ki do sk�adowania lokanego �r�d�a danych.

//! �r�d�o to oparte jest na bazie danych - SQLite3.
//! Aktualnie operuje na jednej tabeli - files_table kt�ra ma 2 kolumny:
//! fileName - unikalna nazwa pliku (TEXT - wymaga tworzenia unikalnych nazw dla p�ytkich kopii u�ytkownik�w)
//! file - zawarto�� pliku (BLOB)
class DataSourceLocalStorage
{
private:
    //! Domy�lny konstruktor
    //! Wszystkie �ciezki powinny by� ukryte/niedost�pne dla u�ytkownik�w - szyfrowanie?
    DataSourceLocalStorage();

    //! Destruktor
    ~DataSourceLocalStorage();

public:

    //! \param motionDataPath �cie�ka danych ruchu
    //! \param schemasPath �cie�ka p�ytkich kopii danych
    //! \return Instancja DataSourceLocalStorage
	//! \param userDataPath �cie�ka do wypakowania danych u�ytkownika
    static DataSourceLocalStorage * create();

    //! Niszczy instacj� obiektu DataSourceLocalStorage
    static void destroy();
	//! \param localStorageDataPath �cie�ka do pliku z danymi - jesli brak to tworzymy now�
	void setLocalStorageDataPath(const core::Filesystem::Path & localStorageDataPath);
	//! \return �cie�ka do pliku z danymi
	const core::Filesystem::Path & localStorageDataPath() const;

    //! \return Instancja local storage
    static DataSourceLocalStorage * instance();

	//! \param fileName Unikalna nazwa pliku o kt�ry pytamy czy jest dost�pny lokalnie
	//! \return Czy plik jest dostepny lokalnie
	bool fileIsLocal(const std::string & fileName) const;

	//! \param path �cie�ka do pliku kt�ry dodajemy do og�lnej puli plik�w
	//! \param fileUniqueName Unikalna nazwa pliku pod kt�rym widzimy plik w puli
	void loadFile(const core::Filesystem::Path & path, const std::string & fileUniqueName = std::string());

	//! \param fileName Nazwa pliku kt�ry pobieramy z og�lnej puli plik�w
	//! \param destPath �cie�ka gdzie zapisujemy zadany plik
	void extractFile(const std::string & fileName, const core::Filesystem::Path & destPath);

	//! \param fileName Nazwa pliku kt�ry pobieramy z og�lnej puli plik�w
	void removeFile(const std::string & fileName);

private:
	//! Sprawdzam czy struktura bazy danych jest ok - je�li nie to pr�buj� j� utworzy� (tabela files_table)
	void verifyAndRebuildDBStructure();

private:
	//! �cie�ka do pliku bazy danych
	core::Filesystem::Path localStorageDataPath_;
    //! Instancja obiektu
    static DataSourceLocalStorage * instance_;
	//! Baza danych SQLite3
	sqlite3 * db;
	//! Czy baza otwarta
	bool dbOpen;
};

#endif  //  HEADER_GUARD___COMMUNICATIONDATASOURCESTORAGE_H__