/********************************************************************
	created:  2014/09/24	08:43:12
	filename: SQLiteBLOB.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEBLOB_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEBLOB_H__

#include <sqliteUtils/Export.h>
#include <sqliteUtils/WrapperT.h>
#include <sqlite3.h>
#include <string>
#include <memory>

namespace sqliteUtils
{
	class SQLITEUTILS_EXPORT SQLiteBLOB
	{
	public:

		class SQLITEUTILS_EXPORT Close
		{
		public:
			Close(const unsigned int retriesCount = 0,
				const unsigned int stepWaitTime = 150);

			const bool operator()(sqlite3_blob * blob);

		private:
			const unsigned int retriesCount;
			const unsigned int stepWaitTime;
		};		
		
		//! \param db Uchwyt bazy danych na kt�rej pr�buj� otworzy� bloba
		//! \param tableName Nazwa tabeli
		//! \param columnName Nazwa kolumny
		//! \param rowID Identyfikator wiersza
		//! \param flags Flagi otwarcia
		//! \param dbName Nazwa bazy danych
		//! \return Uchwyt bloba je�li uda�o si� otworzy� lub nullptr
		static sqlite3_blob * open(sqlite3 * db, const std::string & tableName,
			const std::string & columnName, const sqlite3_int64 rowID,
			const int flags, const std::string & dbName = "main");

		//! \param blob Uchyt do zamkni�cia
		//! \param retriesCount Ilo�� pr� zamkni�cia
		//! \param stepWaitTime Czas dla innych watk�w na dzia�anie [ms]
		//! \return Czy uda�o si� zamkn�� bloba
		static const bool close(sqlite3_blob * blob);
	};
}

namespace std
{
	template<>
	void default_delete<sqlite3_blob>::operator()(sqlite3_blob *blob) const
	{	// delete a pointer
		sqliteUtils::SQLiteBLOB::close(blob);
	}
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEBLOB_H__