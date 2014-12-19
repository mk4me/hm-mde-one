/********************************************************************
	created:  2014/09/24	09:04:05
	filename: SQLitePreparedStatement.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEPREPAREDSTATEMENT_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEPREPAREDSTATEMENT_H__

#include <sqliteUtils/Export.h>
#include <sqliteUtils/WrapperT.h>
#include <sqlite3.h>
#include <string>

namespace sqliteUtils
{
	class SQLITEUTILS_EXPORT SQLitePreparedStatement
	{
	public:

		class SQLITEUTILS_EXPORT Finalizer
		{
		public:
			Finalizer(const unsigned int retriesCount = 0,
				const unsigned int stepWaitTime = 150);

			const bool operator()(sqlite3_stmt * statment);

		private:
			const unsigned int retriesCount;
			const unsigned int stepWaitTime;
		};

		typedef WrapperT<sqlite3_stmt, Finalizer> Wrapper;

		//! \param db Uchwyt bazy danych
		//! \param sql Zapytanie
		//! \return Uchwyt zapytania lub nullptr
		static sqlite3_stmt * prepare(sqlite3 * db, const std::string & sql);		

		//! \param statement Uchwyt zapytania	
		//! \return Czy uda³o siê zmakn¹æ uchwyt
		static const bool finalize(sqlite3_stmt * statement);		

		//! \param statement Uchwyt zapytania
		//! \return Kod operacji wykonania
		static const int step(sqlite3_stmt * statement);
	};
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEPREPAREDSTATEMENT_H__