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
			//! \param force Czy wymusiæ zamkniêcie przez przerwanie zapyatañ
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
		//! \return Prawda jeœli uda³o siê zweryfikowaæ poprawnoœæ uchwytu bazy danych
		static const bool verify(sqlite3 * db);
		//! \param path Œcie¿ka do pliku z baz¹ danych
		//! \param key Klucz którym zaszyfrowana jest baza danych
		//! \param flags Flagi z jakimi otwieramy baze danych
		//! \return Uchwyt bazy danych
		static sqlite3 * open(const std::string & path, const std::string & key,
			const int flags = SQLITE_OPEN_READWRITE);
		//! \param path Œcie¿ka do pliku z baz¹ danych		
		//! \param flags Flagi z jakimi otwieramy baze danych
		//! \return Uchwyt bazy danych
		static sqlite3 * open(const std::string & path,
			const int flags = SQLITE_OPEN_READWRITE);
		//! Metoda próbuje wymusiæ zamkniêcie uchwytu poprzez zamykanie oczekuj¹cych zapytañ w œrodowisku wielow¹tkowym
		//! Daje czas innym w¹tkom na dzia³anie
		//! \param db Uchwyt bazy danych do zamkniêcia
		//! \return Czy uda³o siê zmkan¹æ bazê
		static const bool close(sqlite3 * db);
		//! \param db Uchwyt bazy danych do zamkniêcia
		//! \param maxStatements Maksymalna iloœæ zapytañ do bazy
		//! \param retriesCount Iloœæ dopuszczalnych prób zamykania oczekuj¹cych zapytañ
		//! \param stepWaitTime Czas dla innych w¹tków do dzia³ania [ms]
		//! \return Czy uda³o siê zamkn¹æ wczystkie zapytania na bazie danych
		static const bool finalizeStatements(sqlite3 * db, const unsigned int maxStatements,
			const unsigned int retriesCount = 0, const unsigned int stepWaitTime = 0);
		//! Metoda wywo³uj¹ca zapytanie na bazie danych w œrodowisku wielow¹tkowym
		//! \param db Uchwyt bazy danych
		//! \param query Zapytanie SQL
		//! \param retriesCount Iloœæ dopuszczalnych prób wykonannia zapytañ
		//! \param stepWaitTime Czas dla innych w¹tków do dzia³ania [ms]
		//! \return Kod wykonania zapytania
		static const int exec(sqlite3 * db, const std::string & query,
			const unsigned int retriesCount = 0, const unsigned int stepWaitTime = 0);
	};
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEDB_H__