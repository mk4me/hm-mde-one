/********************************************************************
	created:  2014/09/23	19:09:35
	filename: SQLiteDB.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEDB_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEDB_H__

#include <sqliteUtils/Export.h>
#include <sqliteUtils/WrapperT.h>
#include <sqlite3.h>
#include <string>

namespace sqliteUtils
{
	class SQLITEUTILS_EXPORT SQLiteDB
	{
	public:

		class SQLITEUTILS_EXPORT Close
		{
		public:
			//! \param force Czy wymusi� zamkni�cie przez przerwanie zapyata�
			Close(const bool force = false, const unsigned int maxStatements = 0,
				const unsigned int retriesCount = 0,
				const unsigned int stepWaitTime = 150);

			const bool operator()(sqlite3 * db);

		private:
			const bool force;
			const unsigned int maxStatements;
			const unsigned int retriesCount;
			const unsigned int stepWaitTime;
		};

		typedef WrapperT<sqlite3, Close> Wrapper;

		//! \param db Weryfikowana baza danych
		//! \return Prawda je�li uda�o si� zweryfikowa� poprawno�� uchwytu bazy danych
		static const bool verify(sqlite3 * db);
		//! \param path �cie�ka do pliku z baz� danych
		//! \param key Klucz kt�rym zaszyfrowana jest baza danych
		//! \param flags Flagi z jakimi otwieramy baze danych
		//! \return Uchwyt bazy danych
		static sqlite3 * open(const std::string & path, const std::string & key,
			const int flags = SQLITE_OPEN_READWRITE);
		//! \param path �cie�ka do pliku z baz� danych		
		//! \param flags Flagi z jakimi otwieramy baze danych
		//! \return Uchwyt bazy danych
		static sqlite3 * open(const std::string & path,
			const int flags = SQLITE_OPEN_READWRITE);
		//! Metoda pr�buje wymusi� zamkni�cie uchwytu poprzez zamykanie oczekuj�cych zapyta� w �rodowisku wielow�tkowym
		//! Daje czas innym w�tkom na dzia�anie
		//! \param db Uchwyt bazy danych do zamkni�cia
		//! \return Czy uda�o si� zmkan�� baz�
		static const bool close(sqlite3 * db);
		//! \param db Uchwyt bazy danych do zamkni�cia
		//! \param maxStatements Maksymalna ilo�� zapyta� do bazy
		//! \param retriesCount Ilo�� dopuszczalnych pr�b zamykania oczekuj�cych zapyta�
		//! \param stepWaitTime Czas dla innych w�tk�w do dzia�ania [ms]
		//! \return Czy uda�o si� zamkn�� wczystkie zapytania na bazie danych
		static const bool finalizeStatements(sqlite3 * db, const unsigned int maxStatements,
			const unsigned int retriesCount = 0, const unsigned int stepWaitTime = 0);
		//! Metoda wywo�uj�ca zapytanie na bazie danych w �rodowisku wielow�tkowym
		//! \param db Uchwyt bazy danych
		//! \param query Zapytanie SQL
		//! \param retriesCount Ilo�� dopuszczalnych pr�b wykonannia zapyta�
		//! \param stepWaitTime Czas dla innych w�tk�w do dzia�ania [ms]
		//! \return Kod wykonania zapytania
		static const int exec(sqlite3 * db, const std::string & query,
			const unsigned int retriesCount = 0, const unsigned int stepWaitTime = 0);
	};
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEDB_H__