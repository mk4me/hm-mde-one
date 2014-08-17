#include "CommunicationPCH.h"
#include "SQLCipherStorage.h"
#include <streambuf>

#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/array.hpp>
#include <sqlite3.h>

static const unsigned int sqliteExecWaitMS = 50;
static const unsigned int maxSqliteExecTries = 2400;

const int sqliteExec(sqlite3 * db, const std::string & query);

const bool finalizeSQLiteStatement(sqlite3_stmt * statement, sqlite3 * db = nullptr)
{
	// don't trap, can't handle it anyway
	auto rc = sqlite3_finalize(statement);
	int tries = 0;
	while (tries++ < maxSqliteExecTries && (rc == SQLITE_BUSY || rc == SQLITE_LOCKED)){
		boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		rc = sqlite3_finalize(statement);
	}

	if (rc != SQLITE_OK){
		if (db != nullptr){
			auto errorCode = sqlite3_errcode(db);
			std::string errorMessage(sqlite3_errmsg(db));
		}
		return false;
	}

	return true;
}

class SQLiteDB
{
public:

	SQLiteDB(const core::Filesystem::Path & path, const std::string & key = std::string(),
		const bool check = true, const int flags = SQLITE_OPEN_READWRITE) : db(nullptr)
	{
		open(path, key, check, flags);
	}

	explicit SQLiteDB(sqlite3 * db = nullptr) : db(db) {}

	void open(const core::Filesystem::Path & path, const std::string & key = std::string(),
		const bool check = true, const int flags = SQLITE_OPEN_READWRITE)
	{
		reset();
		auto rc = sqlite3_open_v2(path.string().c_str(), &db, flags, nullptr);
		if (rc != SQLITE_OK){
			reset();
		}
		else if(key.empty() == false){
			rc = sqlite3_key(db, key.c_str(), key.size());
			if (rc != SQLITE_OK){
				reset();
			}
			else if (check == true && verify() == false) {
				reset();				
			}
		}
	}

	const bool verify()
	{
		return sqliteExec(db, "SELECT count(*) FROM sqlite_master;") == SQLITE_ROW;
	}

	void reset(sqlite3 * db = nullptr)
	{
		clean();
		this->db = db;
	}

	sqlite3 * release()
	{
		auto ret = db;
		db = nullptr;
		return ret;
	}

	~SQLiteDB()
	{
		clean();
	}	

	sqlite3 * get() const
	{
		return db;
	}

	SQLiteDB & operator = (sqlite3 * db)
	{
		reset(db);
	}

private:

	void clean()
	{
		if (db != nullptr){
			int tries = 0;
			auto rc = sqlite3_close(db);
			while (tries++ < maxSqliteExecTries && (rc == SQLITE_BUSY || rc == SQLITE_LOCKED)) // maybe include _LOCKED
			{
				sqlite3_stmt *stmt = sqlite3_next_stmt(db, nullptr);
				if (stmt != nullptr){
					// don't trap, can't handle it anyway
					finalizeSQLiteStatement(stmt, db);					
				}
				// give other threads time
				boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
				rc = sqlite3_close(db);
			}
			//if (rc != SQLITE_OK) reportSQLiteError(rc, "while closing connection");

			if (rc != SQLITE_OK){
				auto errorCode = sqlite3_errcode(db);
				std::string errorMessage(sqlite3_errmsg(db));
			}			
		}
	}

private:
	sqlite3 * db;
};

const bool mtBlobClose(sqlite3_blob * blob, sqlite3 * db = nullptr)
{
	unsigned int n = 0;
	int rc = SQLITE_OK;
	do{

		rc = sqlite3_blob_close(blob);

		if ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)){
			boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		}

	} while ((n++ < maxSqliteExecTries) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));

	if (db != nullptr && rc != SQLITE_OK){
		auto errorCode = sqlite3_errcode(db);
		std::string errorMessage(sqlite3_errmsg(db));
	}

	return (rc == SQLITE_OK);
}

sqlite3_blob * mtBlobOpen(sqlite3 * db, const std::string & tableName,
	const std::string & columnName, const sqlite3_int64 rowID,
	const int flags, const std::string & dbName = "main")
{
	sqlite3_blob * ret = nullptr;
	unsigned int n = 0;
	int rc = SQLITE_OK;
	do{

		rc = sqlite3_blob_open(db, dbName.c_str(), tableName.c_str(),
			columnName.c_str(), rowID, flags, &ret);

		if ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)){
			boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		}

	} while ((n++ < maxSqliteExecTries) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));

	if (rc == SQLITE_OK){
		return ret;
	}
	
	auto errorCode = sqlite3_errcode(db);
	std::string errorMessage(sqlite3_errmsg(db));

	mtBlobClose(ret, db);

	return nullptr;
}

class SQLiteBLOB
{
public:

	SQLiteBLOB(sqlite3 * db, const std::string & tableName,
		const std::string & columnName, const sqlite3_int64 rowID,
		const int flags, const std::string & dbName = "main") : blob(nullptr)
	{
		open(db, tableName, columnName, rowID, flags, dbName);
	}

	explicit SQLiteBLOB(sqlite3_blob * blob = nullptr, sqlite3 * db = nullptr) : blob(blob), db(db) {}

	void open(sqlite3 * db, const std::string & tableName,
		const std::string & columnName, const sqlite3_int64 rowID,
		const int flags, const std::string & dbName = "main")
	{
		reset();
		blob = mtBlobOpen(db, tableName, columnName, rowID, flags, dbName);
		if (blob != nullptr){
			this->db = db;
		}
	}

	void reset(sqlite3_blob * blob = nullptr, sqlite3 * db = nullptr)
	{
		clean();
		this->blob = blob;
		if (blob != nullptr){
			this->db = db;
		}
	}

	sqlite3_blob * release()
	{
		auto ret = blob;
		blob = nullptr;
		db = nullptr;
		return ret;
	}

	~SQLiteBLOB()
	{
		clean();
	}

	sqlite3_blob * get() const
	{
		return blob;
	}

	SQLiteBLOB & operator = (sqlite3_blob * blob)
	{
		reset(blob);
	}

private:

	void clean()
	{
		if (blob != nullptr){
			mtBlobClose(blob, db);
		}
	}

private:
	//!
	sqlite3_blob * blob;
	//!
	sqlite3 * db;
};

void mtExecute(sqlite3 * db, const std::string & querry)
{

}

sqlite3_stmt * mtPrepare(sqlite3 * db, const std::string & querry)
{
	sqlite3_stmt * ret = nullptr;
	unsigned int n = 0;
	int rc = SQLITE_OK;
	do{

		rc = sqlite3_prepare_v2(db, querry.c_str(), querry.size(), &ret, 0);

		if ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)){
			boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		}

	} while ((n++ < maxSqliteExecTries) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));

	if (rc == SQLITE_OK){
		return ret;
	}

	auto errorCode = sqlite3_errcode(db);
	std::string errorMessage(sqlite3_errmsg(db));

	return nullptr;
}

const int mtStep(sqlite3_stmt * step)
{
	sqlite3_stmt * ret = nullptr;
	unsigned int n = 0;
	int rc = SQLITE_OK;
	do{
		rc = sqlite3_step(step);

		if (rc == SQLITE_LOCKED){
			rc = sqlite3_reset(step);
			boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		}
		else if (rc == SQLITE_BUSY){
			boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		}
	} while ((n++ < maxSqliteExecTries) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));

	return rc;
}

const int mtStep(sqlite3_stmt * step, sqlite3 * db)
{
	sqlite3_stmt * ret = nullptr;
	unsigned int n = 0;
	int rc = SQLITE_OK;
	do{
		rc = sqlite3_step(step);

		if (rc == SQLITE_LOCKED){
			rc = sqlite3_reset(step);
			boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		}
		else if (rc == SQLITE_BUSY){
			sqlite3_stmt * nextStatement = sqlite3_next_stmt(db, nullptr);
			boost::this_thread::sleep_for(boost::chrono::milliseconds(sqliteExecWaitMS));
		}
	} while ((n++ < maxSqliteExecTries) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)));

	return rc;
}

class SQLitePreparedStatement
{
public:
	SQLitePreparedStatement(sqlite3 * db, const std::string & sql)
		: statement(mtPrepare(db, sql))
	{
		if (statement != nullptr){
			this->db = db;
		}
	}

	~SQLitePreparedStatement()
	{
		if (statement != nullptr){
			const bool ret = finalizeSQLiteStatement(statement, db);			
		}
	}

	inline operator sqlite3_stmt*()
	{
		return statement;
	}

	inline sqlite3_stmt * get() const
	{
		return statement;
	}

	inline sqlite3_stmt * release()
	{
		sqlite3_stmt * q = nullptr;
		db = nullptr;
		std::swap(q, statement);
		return q;
	}

private:
	//! Skompilowane zapytanie
	sqlite3_stmt * statement;
	sqlite3 * db;
};

const int sqliteExec(sqlite3 * db, const std::string & query)
{
	SQLitePreparedStatement qStatement(db, query);
	return mtStep(qStatement, db);
}

enum BufferSizeTraits {
	MinBufferSize = (1 << 8),		//! 256B
	DefaultBufferSize = (1 << 11),	//! 2KB
	MaxBufferSize = (1 << 20) * 10	//! 10MB
};

//! \tparam T Typ danych przechowywanych w buforze
//! \tparam Rozmiar bufora
//! Klasa realizuj¹ca politykê bufora o sta³ym rozmiarze
template<typename T, int Size = DefaultBufferSize>
class FixedBufferPolicy
{

	UTILS_STATIC_ASSERT((Size > 0), "Fixed buffer sieze must be greater than 0");

private:
	//! Typ kolekcji przechowujacej dane
	typedef boost::array<T, Size> Data;
	//! Indeks ostatniego elementu
	static const int LastIdx = Size - 1;

public:
	//! Domyœlny konsturktor
	FixedBufferPolicy() {}
	//! Destruktor niewirtualny
	~FixedBufferPolicy() {}

	T * begin() { return &(data.data()[0]); }
	T * end() { return &(data.data()[LastIdx]); }

	const T * begin() const { return &(data.data()[0]); }
	const T * end() const { return &(data.data()[LastIdx]); }

	const int updateSize(const int size) { return Size; }

private:
	//! Faktyczne dane
	Data data;
};

//! \tparam T Typ danych przechowywanych w buforze
//! Klasa realizuj¹ca politykê bufora o dynamicznie zwiekszanym rozmiarze przy zapisie
template<typename T>
class DynamicWriteBufferPolicy
{
private:

	//! Rozmiar przechowywanego elementu
	static const int SizeT = sizeof(T);
	//! Typ kolekcji przechowujacej dane
	typedef std::vector<T> Data;
	//! Wspó³czynnik procentowy opisuj¹cy rozmiar bufora wzglêdem rozmiaru strumienia
	static const float BufferFactor() { return 0.05f; }

public:
	//! Domyœlny konsturktor
	DynamicWriteBufferPolicy() { data.resize(DefaultBufferSize); }
	//! Destruktor niewirtualny
	~DynamicWriteBufferPolicy() {}

	T * begin() { return &data.data()[0]; }
	T * end() { return &data.data()[0] + data.size() * sizeof(T); }

	const T * begin() const { return &(data.front()); }
	const T * end() const { return &(data.back()); }

	const int updateSize(const int size) {

		const auto newSize = std::min(std::max((int)(size * BufferFactor()), (int)MinBufferSize), (int)MaxBufferSize);

		if (data.size() < newSize){
			data.resize(newSize);
		}

		return newSize;
	}

private:
	//! Faktyczne dane
	Data data;
};

//! Wzorzec klasy obslugujacej sturmieniowe IO do blobow bazy sqlite
//! \tparam _Elem Elementy sk³adowane w blobach
//! \tparam _Traits Cechy obiektu typu _Elem
//! \tparam _BufferPolicy Polityka zarzadzania buforem danych
template<typename _Elem,
	typename _Traits,
	typename _BufferPolicy = FixedBufferPolicy<_Elem>>
class basic_sqliteblobbuf : public std::basic_streambuf<_Elem, _Traits>
{
private:

	enum
	{	// constants for bits in stream state		
		_Constant = 2,	// set if character array nonmutable
		_Noread = 4,	// set if character array cannot be read
		_Append = 8,	// set if all writes are appends
		_Atend = 16		// set if initial writes are appends
	};

	//! Internal state
	typedef int _Strstate;

	//! Typ inta
	typedef typename _Traits::int_type int_type;
	//! Typ pozycji
	typedef typename _Traits::pos_type pos_type;
	//! Typ offsetu
	typedef typename _Traits::off_type off_type;

public:
	//! Typ mojego bufora
	typedef basic_sqliteblobbuf<_Elem, _Traits> _Myt;
	//! Typ bufora strumienia
	typedef basic_streambuf<_Elem, _Traits> _Mysb;
	//! Typ polityki bufora
	typedef _BufferPolicy _MyBufferPolicy;
	//! Typ mojego stanu
	typedef typename _Traits::state_type _Myst;

private:

	//! \return Rozmiar bufora
	inline const int bufferSize() const
	{
		return buffer.end() - buffer.begin();
	}

	//! \param idx Globalny index
	//! \return Maksymalny index bufora uwzglêdniaj¹cy zakres strumienia i rozmiar bufora
	inline const int maxIDX(const int idx) const
	{
		return std::min(idx + bufferSize(), _Seekhigh);
	}

	//! \return Maksymalny index globalny bufora do odczytu
	inline const int readMaxIDX() const
	{
		return maxIDX(blobReadIDX);
	}

	//! \return Maksymalny index globalny bufora do zapisu
	inline const int writeMaxIDX() const
	{
		return maxIDX(blobWriteIDX);
	}

	//! \param globalBase Globalna baza
	//! \param localBase Baza lokalna bufora
	//! \param localIDX Lokalna pozycja bufora
	//! \return Globalna pozycja bufora
	inline static const int globalIDX(const int globalBase, const _Elem * localBase,
		const _Elem * localIDX)
	{
		return globalBase + (int)(localIDX - localBase);
	}

	//! \return Globalna pozycja dla zapisu
	inline const int globalWriteIDX() const
	{
		return globalIDX(blobWriteIDX, _Mysb::pbase(), _Mysb::pptr());
	}

	//! \return Globalna pozycja dla odczytu
	inline const int globalReadIDX() const
	{
		return globalIDX(blobReadIDX, _Mysb::eback(), _Mysb::gptr());		
	}

	inline void updateStreamEnd()
	{
		if (_Seekhigh < globalWriteIDX()){
			_Seekhigh = globalWriteIDX();
		}
	}

public:

	//! Destruktor wirtualny - zapisuje zmodyfikowane dane do bazy,
	//! zamyka i zwalnia wszystkie zasoby bazy
	virtual __CLR_OR_THIS_CALL ~basic_sqliteblobbuf()
	{
		
	}

	void synch()
	{

	}

	//! Konstruktor
	//! \param _Mode W jakim trybie otwarty jest strumieñ
	//! \param db Baza na której bêdziemy dzia³aæ
	//! \param dbName Nazwa bazy na której dzia³amy (https://sqlite.org/c3ref/blob_open.html)
	//! \param table Nazwa tabeli
	//! \param column Nazwa kolumny
	//! \param rowID Unikalny identyfikator wiersza w zadanej tabeli
	//! \param buffer Bufor uzywany wewnatrz
	basic_sqliteblobbuf(const core::Filesystem::Path & path,
		const std::string & key, const std::string & dbName, const std::string & table,
		const std::string & column, const sqlite3_int64 rowID, const int initialSize,
		std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out)
		: _Mysb(), path(path), key(key), mode_(_Mode), dbName(dbName), table(table), column(column),
		rowID(rowID), blobReadIDX(0), blobWriteIDX(0), blobSize(initialSize), _Seekhigh(initialSize)
	{
		_Mystate = _Getstate(_Mode);

		//czy pierwsze wywolanie i tryb append?
		if (_Mystate & _Append)
		{
			blobWriteIDX = blobSize;
		}
		else{
			blobWriteIDX = 0;
		}		
	}

protected:

	virtual int_type __CLR_OR_THIS_CALL overflow(int_type _Meta =
		_Traits::eof())
	{
		if (_Mystate & _Constant){
			return (_Traits::eof()); // read only mode or no open blob, fail
		} else if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
			return (_Traits::not_eof(_Meta));	// EOF, return success code
		}else if ((buffer.begin() == nullptr) && (buffer.updateSize(std::max(blobSize, (int)DefaultBufferSize)) < 1)){
			return (_Traits::eof());	// could not initialize internal buffer, fail
		}

		//czy cos by³o pisane do bufora?
		if (_Mysb::pptr() != nullptr){
			// musze zrzucic bufor do strumienia
			//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
			const auto oldBlobSize = blobSize;

			// je¿eli cos czyta³em wczeœniej to muszê zapamiêtaæ i wyzerowaæ je¿li to nie ten sam obszar
			if (_Mysb::gptr() != nullptr){
				blobReadIDX += (_Mysb::gptr() - _Mysb::eback());
				_Mysb::setg(buffer.begin(), nullptr, buffer.end());
			}

			//zapisuje bufor w stumieniu
			if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
				return (_Traits::eof());	// zapis sie nie powiodlo, fail
			}

			//czy zmieni³ siê rozmiar?
			if (oldBlobSize < blobSize){
				//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
				buffer.updateSize(blobSize);
			}
			//rozmiar sie nie zmieni³
			//sprawdzam gdzie teraz w strumieniu jestem
			else if (blobWriteIDX < blobSize){
				//ca³y czas piszê wewn¹trz bloba, moge odœwiezyæ zawartoœæ bufor ze strumienia
				auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), blobWriteIDX, blobSize);

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}
			}

			//ustawiam nowy bufor i jego stan
			_Mysb::setp(buffer.begin(), buffer.begin(), buffer.end());
		}
		//nie pisa³em do bufora
		else if (_Mysb::gptr() != nullptr){
			//czyta³em coœ z bufora a chcê bufor do zapisu
			//sprawdzam czy obszary odczytu/zapisu siê nie zazêbiaj¹
			if (blobWriteIDX < blobReadIDX || blobWriteIDX >= std::min(blobReadIDX + bufferSize(), _Seekhigh)){
				
				//niestety, obszary s¹ roz³¹czne, muszê zapamiêtac stan bufora do odczytu i zresetowaæ jego obszar
				blobReadIDX += (_Mysb::gptr() - _Mysb::eback());
				_Mysb::setg(buffer.begin(), nullptr, buffer.end());

				//prze³adowuje bufor nowymi danymi, bêdê po nich pisa³
				auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), blobWriteIDX, blobSize);

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}

				//ustawiam nowy bufor i jego stan
				_Mysb::setp(buffer.begin(), buffer.begin(), buffer.end());
			}
			else{
				//obszary pokrywaj¹ siê - wystarczy poprzestawiaæ wskaŸniki
				auto diff = blobWriteIDX - blobReadIDX;
				blobWriteIDX = blobReadIDX;
				//poprawiam obszar do zapisu
				_Mysb::setp(buffer.begin(), buffer.begin() + diff, buffer.end());
			}
		}
		else{
			//nie by³o zapisu i odczytu albo oba obszary nieaktyne po seek

			if (blobSize > 0 && blobWriteIDX != blobSize){
				//uzupelniam bufor
				auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), blobWriteIDX, blobSize);

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}
			}			

			//ustawiam nowy bufor i jego stan
			setp(buffer.begin(), buffer.begin(), buffer.end());
		}

		//zapisujemy
		*_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
		//aktualizuje koniec strumienia dla poprawnego odczytu ca³ego strumienia podczas zapisu
		updateStreamEnd();

		return (_Meta);
	}

	virtual int_type __CLR_OR_THIS_CALL underflow()
	{
		// get an element from stream, point on it
		if (blobSize < 0 || (_Mystate & _Noread) ||
			((buffer.begin() == nullptr) && (buffer.updateSize(std::max(blobSize, (int)DefaultBufferSize)) < 1))){
			return (_Traits::eof());	// no open blob or no data to read or failed to create buffer, fail
		}

		updateStreamEnd();

		//czy konieczne jest prze³adowanie bufora?
		bool refill = true;

		//sprawdzam czy byly zapisy
		if (_Mysb::pptr() != nullptr){
			//musze sprawdzic czy to gdzie sie chce znalezc wypada poza aktualny obszar - jezeli tak to musze zapisac dane, odswiezyc bufor
			if ((blobReadIDX < blobWriteIDX) || (globalWriteIDX() <= blobReadIDX)){
				//wypadam poza
				blobWriteIDX = globalWriteIDX();

				const auto oldBlobSize = blobSize;

				if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
					return (_Traits::eof());	// zapis sie nie powiodlo, fail
				}

				if (oldBlobSize < blobSize){
					//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
					buffer.updateSize(blobSize);
				}

				_Mysb::setp(buffer.begin(), nullptr, buffer.end());
			}
			else{
				//zazêbiaj¹ siê obszary, wystarczy wska¿niki poprawiæ
				refill = false;
				//maks rozmiar do odczytu
				const auto size = ((blobWriteIDX + bufferSize()) <= blobSize) ? bufferSize() : _Seekhigh - globalWriteIDX();
				//poprawiam obszar do odczytu
				_Mysb::setg(buffer.begin(), buffer.begin() + blobReadIDX - blobWriteIDX, buffer.begin() + size);
				blobReadIDX = blobWriteIDX;
			}
		}

		if (refill == true){

			//uzupelnam bufor
			auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), blobReadIDX, blobSize);

			//czy cos pobralem albo jakis blad?
			if (s < 1){
				return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
			}

			blobReadIDX += s;

			//inicjuje obszar do odczytu
			_Mysb::setg(buffer.begin(), buffer.begin(), buffer.begin() + s);
		}

		// no codecvt facet, just get it
		_Elem _Ch = *(_Mysb::gptr());
		return _Traits::to_int_type(_Ch);
	}

	virtual pos_type seekoff(off_type _Off,
		std::ios_base::seekdir _Way,
		std::ios_base::openmode _Which = std::ios_base::in | std::ios_base::out)
	{	// change position by _Off, according to _Way, _Mode

		updateStreamEnd();

		auto _OffPos = _Off;

		if (_Which & std::ios_base::in)
		{	// position within read buffer
			if (_Way == std::ios_base::end)
				_OffPos += (off_type)_Seekhigh;
			else if (_Way == std::ios_base::cur
				&& (_Which & std::ios_base::out) == 0)
				_OffPos += (off_type)((_Mysb::gptr() != nullptr) ? globalReadIDX() : blobReadIDX);
			else if (_Way != std::ios_base::beg)
				_OffPos = std::_BADOFF;

			if (0 <= _OffPos && _OffPos <= _Seekhigh)
			{	// change read position

				bool writeUpdated = false;

				if (_Mysb::gptr() == nullptr){
					blobReadIDX = _OffPos;
				}
				else if (blobReadIDX <= _OffPos && _OffPos < readMaxIDX()){
					_Mysb::gbump((int)(globalReadIDX() - _OffPos));
				}
				else{
					
					if (_Mysb::pptr() != nullptr){

						const auto oldBlobSize = blobSize;

						if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
							return (_Traits::eof());	// zapis sie nie powiodlo, fail
						}

						if (oldBlobSize < blobSize){
							//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
							buffer.updateSize(blobSize);
						}

						_Mysb::setp(buffer.begin(), nullptr, buffer.end());
						writeUpdated = true;
					}

					blobReadIDX = _OffPos;
					_Mysb::setg(buffer.begin(), nullptr, buffer.end());
				}

				if ((_Which & std::ios_base::out) && writeUpdated == false){
					if (blobWriteIDX <= _OffPos && _OffPos < readMaxIDX()){
						_Mysb::pbump((int)(globalWriteIDX() - _OffPos));
					}
					else{

						const auto oldBlobSize = blobSize;

						if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
							return (_Traits::eof());	// zapis sie nie powiodlo, fail
						}

						if (oldBlobSize < blobSize){
							//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
							buffer.updateSize(blobSize);
						}

						blobWriteIDX = _OffPos;
						_Mysb::setp(buffer.begin(), nullptr, buffer.end());
					}
				}
			}
			else
				_OffPos = std::_BADOFF;
		}
		else if (_Which & std::ios_base::out)
		{	// position within write buffer
			if (_Way == std::ios_base::end)
				_OffPos += (off_type)_Seekhigh;
			else if (_Way == std::ios_base::cur)
				_OffPos += (off_type)((_Mysb::pptr() != nullptr) ? globalWriteIDX() : blobWriteIDX);				
			else if (_Way != std::ios_base::beg)
				_OffPos = std::_BADOFF;

			if (0 <= _OffPos && _OffPos <= _Seekhigh){
				if (blobWriteIDX <= _OffPos && _OffPos < readMaxIDX()){
					_Mysb::pbump((int)(globalWriteIDX() - _OffPos));
				}
				else{

					if (_Mysb::gptr() != nullptr){
						blobReadIDX = globalReadIDX();
					}

					const auto oldBlobSize = blobSize;

					if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
						return (_Traits::eof());	// zapis sie nie powiodlo, fail
					}

					if (oldBlobSize < blobSize){
						//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
						buffer.updateSize(blobSize);
					}

					if (_Mysb::gptr() != nullptr){
						_Mysb::setg(buffer.begin(), nullptr, buffer.end());
					}

					blobWriteIDX = _OffPos;
					_Mysb::setp(buffer.begin(), nullptr, buffer.end());
				}
			}
			else
				_OffPos = std::_BADOFF;
		}

#if _HAS_CPP0X
		else if (_OffPos != 0)

#else /* _HAS_CPP0X */
		else
#endif /* _HAS_CPP0X */

			_OffPos = std::_BADOFF;	// neither read nor write buffer selected, fail
		return (pos_type(_Off));
	}

	virtual pos_type seekpos(pos_type _Ptr,
		std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out)
	{	// change position to _Pos, according to _Mode
		std::streamoff _Off = (std::streamoff)_Ptr;

		updateStreamEnd();

		if (_Off == std::_BADOFF)
			;
		else if (_Mode & std::ios_base::in && _Mysb::gptr() != 0)
		{	// position within read buffer
			if (0 <= _Off && _Off <= _Seekhigh)
			{	// change read position
				_Mysb::gbump((int)(_Mysb::eback() - _Mysb::gptr() + _Off));
				if (_Mode & std::ios_base::out && _Mysb::pptr() != 0)
					_Mysb::setp(_Mysb::pbase(), _Mysb::gptr(),
					_Mysb::epptr());	// change write position to match
			}
			else
				_Off = std::_BADOFF;
		}
		else if (_Mode & std::ios_base::out && _Mysb::pptr() != 0)
		{	// position within write buffer
			if (0 <= _Off && _Off <= _Seekhigh)
				_Mysb::pbump((int)(_Mysb::eback()
				- _Mysb::pptr() + _Off));	// change write position
			else
				_Off = std::_BADOFF;
		}
		else
			_Off = std::_BADOFF;	// neither read nor write buffer selected, fail
		return (std::streampos(_Off));
	}

	virtual int __CLR_OR_THIS_CALL sync()
	{	// synchronize C stream with external file
		return flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == true ? 0 : -1;
	}

private:

	//! Metoda wypelnia bufor zawartoscia bloba
	//! \param buffer Wskaznik poczatku bufora gdzie moge czytac dane z bloba
	//! \param bufferSize Ile maksymalnie moge wczytac do bufora
	//! \param blob Blob z ktorego czytam
	//! \param blobIDX Wskaznik w obrebie bloba od ktorego zaczynam czytac
	//! \param blobSize Rozmiar bloba
	//! \return Ilosc znakow odczytanych z bloba
	const int refillBuffer(char * buffer, const int bufferSize,
		const int blobIDX, const int blobSize)
	{
		int toRead = std::min(bufferSize, blobSize - blobIDX);

		// jak mam jeszcze cos do czytania
		if (toRead > 0){
			SQLiteDB db(path, key);

			if (db.get() == nullptr){
				return -1;
			}

			SQLiteBLOB blob(db.get(), table, column, rowID,
				mode_ & std::ios_base::out, dbName);

			if (blob.get() == nullptr){
				return -1;
			}

			//teraz czytam do mojego bufora
			auto ret = sqlite3_blob_read(blob.get(), buffer, toRead, blobIDX);

			// jak sie nie udalo czytac z sqlite
			if (ret != SQLITE_OK){
				toRead = -1;
			}
		}

		return toRead;
	}

	//! Metoda wypycha lokalne zmiany do bloba - synchronizuje
	//! \return Czy udalo sie dane zapisac do bloba
	const bool flushBuffer(const char * base, const char * end,
		int & blobIDX)
	{
		const unsigned int bufferSize = end - base;
		const unsigned int newSize = blobIDX + bufferSize;

		SQLiteDB db(path, key);

		if (db.get() == nullptr){
			return false;
		}

		SQLiteBLOB blob(db.get(), table, column, rowID,
			mode_ & std::ios_base::out, dbName);

		if (blob.get() == nullptr){
			return false;
		}

		// zapis do bloba
		// czy dane jeszcze sie zmieszcza?
		if (newSize <= blobSize){
			
			//pisze bezposrednio do bloba
			auto ret = sqlite3_blob_write(blob.get(), base, bufferSize, blobIDX);

			if (ret != SQLITE_OK){
				return false;
			}
		}
		else {

			//przygotowuje update
			static const auto updateq = (boost::format("UPDATE %1% SET %2% = ? WHERE iRow = %3%;")
				% updateTableName(dbName, table) % column % rowID).str();

			SQLitePreparedStatement update(db.get(), updateq);

			if (update == nullptr){
				return false;
			}

			//bufor tymczasowy na nowy blob
			std::string tmpBuffer;

			try{

				//rezerwuje na nowy rozmiar
				tmpBuffer.reserve(newSize);

			}
			catch (...){
				return false; // failed to compose complete data, probably out of memory
			}

			//rozszerzam do aktualnego rozmiaru
			tmpBuffer.resize(blobIDX);
			//czytam calego bloba
			auto ret = sqlite3_blob_read(blob.get(), &tmpBuffer[0], blobIDX, 0);

			if (ret != SQLITE_OK){
				return false;
			}

			//zamykam bloba
			blob.reset();

			//dopisuje to co mam
			tmpBuffer.append(base, bufferSize);

			//aktualizuje bloba
			ret = sqlite3_bind_blob(update, 1, tmpBuffer.c_str(), newSize, SQLITE_STATIC);

			if (ret != SQLITE_OK){
				return false; // failed to bind sqlite query
			}

			//odpalam update			
			if (mtStep(update) != SQLITE_DONE){
				return false; // failed to write to sqlite db
			}

			blobSize = newSize;
		}

		blobIDX += bufferSize;

		return true;
	}

	//! Metoda generuje nazwe tabeli dla zapytaia aktualizujacego bloba
	//! \param dbName Nazwa bazy danych
	//! \param table Nazwa tabeli
	//! \return Nazwa tabeli dla zapytania update
	static const std::string updateTableName(const std::string & dbName,
		const std::string & table)
	{
		return (dbName.empty() == true) ? table : dbName + '.' + table;
	}

	//! \param _Mode tryb otwarcia strumienia
	//! \return Wewnêtrzny tryb otwarcia strumienia
	const _Strstate _Getstate(std::ios_base::openmode _Mode)
	{	// convert open mode to stream state bits
		_Strstate _State = (_Strstate)0;
		if (!(_Mode & std::ios_base::in))
			_State |= _Noread;
		if (!(_Mode & std::ios_base::out))
			_State |= _Constant;
		if (_Mode & std::ios_base::app)
			_State |= _Append;
		if (_Mode & std::ios_base::ate)
			_State |= _Atend;
		return (_State);
	}

private:

	//! ------------ OPIS DANYCH W SQLITE ------------------
	const std::ios_base::openmode mode_;
	//! Œcie¿ka do bazy danych
	const core::Filesystem::Path path;
	//! Has³o bazy
	const std::string key;
	//! Nazwa bazy
	std::string dbName;
	//! Nazwa tabeli
	std::string table;
	//! Nazwa kolumny
	std::string column;
	//! Identyfikator wiersza
	sqlite3_int64 rowID;
	//! Aktualny rozmiar bloba
	int blobSize;

	//! ----------------- STAN BUFORÓW --------------------
	//! Bufor
	_MyBufferPolicy buffer;
	//! Aktualny indeks wewn¹trz bloba
	int blobIDX;
	int blobReadIDX;
	int blobWriteIDX;

	//! -------------- STAN STRUMIENIA ---------------------

	//const _Cvt *_Pcvt;	// pointer to codecvt facet (may be null)
	_Strstate _Mystate;	// the stream testa
	int _Seekhigh;	// the high-water pointer in character array
};

const bool initialize(sqlite3 * db)
{
	return sqliteExec(db, "CREATE TABLE files_table (file_name TEXT PRIMARY KEY, file BLOB);") == SQLITE_DONE;
}

const bool checkStructure(sqlite3 * db)
{
	static const std::string sqlCheckTable = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='files_table' ORDER BY name;";

	bool ret = false;	
	SQLitePreparedStatement query(db, sqlCheckTable);	

	if (query != nullptr && mtStep(query) == SQLITE_ROW){
		const auto count = sqlite3_column_int(query, 0);		
		if (count >= 1){
			ret = true;
		}
	}

	return ret;
}

class hmdbCommunication::SQLCipherStorage::SQLCipherStorageTransaction : public hmdbCommunication::IHMDBStorageOperations
{
public:
	SQLCipherStorageTransaction(const SQLCipherStorage * storage)
		: storage(storage), db(storage->path_, storage->key_)
	{		
		if (db.get() == nullptr){
			throw std::runtime_error("Failed to create transaction");
		}
	}

	virtual ~SQLCipherStorageTransaction()
	{
		
	}

	virtual const bool exists(const std::string & key) const
	{
		return SQLCipherStorage::rawExists(key, db.get());
	}
	
	virtual const IOStreamPtr get(const std::string & key)
	{
		return SQLCipherStorage::rawGet(key, db.get(), storage->path_, storage->key_);
	}
	
	virtual const IStreamPtr get(const std::string & key) const
	{
		return SQLCipherStorage::rawGetReadOnly(key, db.get(), storage->path_, storage->key_);
	}
	
	virtual const bool set(const std::string & key, IStreamPtr input)
	{
		return SQLCipherStorage::rawSet(key, input, db.get());
	}
		
	virtual void set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress)
	{
		return SQLCipherStorage::rawSet(key, input, progress, db.get());
	}
	
	virtual const bool remove(const std::string & key)
	{
		return SQLCipherStorage::rawRemove(key, db.get());
	}
	
	virtual const bool rename(const std::string & oldKey,
		const std::string & newKey, const bool overwrite = false)
	{
		return SQLCipherStorage::rawRename(oldKey, newKey, overwrite, db.get());
	}
	
	virtual const Keys keys() const
	{
		return SQLCipherStorage::rawKeys(db.get());
	}

	const bool canStore(const unsigned long long size) const
	{
		return storage->canStore(size);
	}

	const bool shareDiskSpace(const core::Filesystem::Path & path) const
	{
		return storage->shareDiskSpace(path);
	}

private:
	const SQLCipherStorage * storage;
	SQLiteDB db;
};


using namespace hmdbCommunication;

SQLCipherStorage::SQLCipherStorage()
{

}

SQLCipherStorage::~SQLCipherStorage()
{

}


const bool SQLCipherStorage::rawExists(const std::string & key, sqlite3 * db)
{
	bool ret = false;
	const std::string fileQuery((boost::format("SELECT count(*) FROM files_table WHERE file_name='%1%';") % key).str());

	SQLitePreparedStatement query(db, fileQuery);	

	if (query != nullptr && mtStep(query) == SQLITE_ROW){
		const auto count = sqlite3_column_int(query, 0);
		if (count == 1){
			ret = true;
		}
	}	

	return ret;
}

const IHMDBStorage::IOStreamPtr SQLCipherStorage::rawGet(const std::string & key, sqlite3 * db,
	const core::Filesystem::Path & path, const std::string & dbKey)
{
	//typ bufora strumienia
	typedef basic_sqliteblobbuf<char, std::char_traits<char>, DynamicWriteBufferPolicy<char>> SQLiteBuffer;

	//pobieramy dane z bazy
	const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());
	SQLitePreparedStatement query(db, fileQuery);

	if (query != nullptr && mtStep(query) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(query, 0);
		SQLiteBLOB blob(db, "files_table", "file", rowID, std::ios_base::in, "main");
		std::auto_ptr<SQLiteBuffer> buf(new SQLiteBuffer(path, dbKey, "main", "files_table", "file", rowID, sqlite3_blob_bytes(blob.get())));

		return IHMDBStorage::IOStreamPtr(new std::iostream(buf.release()));
	}

	return IHMDBStorage::IOStreamPtr();
}

const IHMDBStorage::IStreamPtr SQLCipherStorage::rawGetReadOnly(const std::string & key, sqlite3 * db,
	const core::Filesystem::Path & path, const std::string & dbKey)
{
	typedef basic_sqliteblobbuf<char, std::char_traits<char>, FixedBufferPolicy<char>> SQLiteBuffer;

	//pobieramy dane z bazy
	const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());
	SQLitePreparedStatement query(db, fileQuery);

	if (query != nullptr && mtStep(query) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(query, 0);
		SQLiteBLOB blob(db, "files_table", "file", rowID, std::ios_base::in, "main");
		//http://www.sqlite.org/c3ref/blob_open.html
		std::auto_ptr<SQLiteBuffer> buf(new SQLiteBuffer(path, dbKey, "main", "files_table", "file", rowID, sqlite3_blob_bytes(blob.get()), std::ios_base::in));

		return IHMDBStorage::IStreamPtr(new std::iostream(buf.release()));
	}

	return IHMDBStorage::IStreamPtr();
}

const bool SQLCipherStorage::rawSet(const std::string & key, IHMDBStorage::IStreamPtr input, sqlite3 * db)
{
	if (input == nullptr){
		return false;
	}

	const auto currentPos = input->tellg();
	input->seekg(0, std::ios::beg);
	auto begin = input->tellg();
	input->seekg(0, std::ios::end);
	auto end = input->tellg();
	input->seekg(currentPos, std::ios::beg);
	const auto streamSize = end - begin;

	if (streamSize == 0){
		return false;
	}

	//wrzucam to do bazy danych
	std::string query;

	const bool existed = rawExists(key, db);

	//najpierw sprawdzam czy plik istnieje
	if (existed == false){
		//insert
		query = (boost::format("INSERT INTO files_table(file_name, file) VALUES('%1%', ?);") % key).str();
	}
	else{
		//update
		query = (boost::format("UPDATE files_table SET file = ? WHERE file_name = '%1%';") % key).str();
	}

	{
		SQLitePreparedStatement store(db, query);

		if (store == nullptr){
			return false;
		}

		auto rc = sqlite3_bind_zeroblob(store, 1, streamSize);

		if (rc != SQLITE_OK || mtStep(store, db) != SQLITE_DONE){
			rawRemove(key, db);
			return false;
		}		
	}

	sqlite3_int64 rowid = -1;

	if (existed == false){
		rowid = sqlite3_last_insert_rowid(db);
	}
	else{
		const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name='%1%';") % key).str());
		SQLitePreparedStatement selectQuery(db, fileQuery);		

		if (selectQuery != nullptr && mtStep(selectQuery) == SQLITE_ROW){
			rowid = sqlite3_column_int64(selectQuery, 0);
		}		
	}

	if (rowid == -1){
		rawRemove(key, db);
		return false;
	}

	// Getting here means we have a valid file handle, f, and a valid db handle, db
	//   Also, a blank row has been inserted with key rowid

	SQLiteBLOB blob(db, "files_table", "file", rowid, 1, "main");	

	if (blob.get() == nullptr) {		
		rawRemove(key, db);
		return false;
	}

	const auto readSize = std::min((int)streamSize, (int)MaxBufferSize);

	boost::shared_array<char> memblock(new char[readSize]);
	input->seekg(0, std::ios::beg);
	int offset = 0;
	const unsigned int fullReads = (streamSize / readSize);
	for (unsigned int i = 0; i < fullReads; ++i){	
		input->read(memblock.get(), readSize);		
		auto rc = sqlite3_blob_write(blob.get(), memblock.get(), readSize, offset);
		if (rc != SQLITE_OK){
			blob.reset();
			rawRemove(key, db);
			return false;
		}

		offset += readSize;
	}

	const auto left = streamSize - offset;

	if (left > 0){
		input->read(memblock.get(), left);
		auto rc = sqlite3_blob_write(blob.get(), memblock.get(), left, offset);
		if (rc != SQLITE_OK){
			blob.reset();
			rawRemove(key, db);
			return false;
		}
	}

	blob.reset();

	//TODO co jak zwalnianie bloba nie da rady?
	/*
	if (rc != SQLITE_OK){
		rawRemove(key, db);
		return false;
	}*/

	return true;
}

void SQLCipherStorage::rawSet(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress, sqlite3 * db)
{
	if (input == nullptr){
		progress->setError("Uninitialized input");
		return;
	}

	const auto currentPos = input->tellg();
	input->seekg(0, std::ios::beg);
	auto begin = input->tellg();
	input->seekg(0, std::ios::end);
	auto end = input->tellg();
	input->seekg(currentPos, std::ios::beg);
	const auto streamSize = end - begin;

	if (streamSize == 0){
		progress->setError("Empty input stream");
		return;
	}

	//wrzucam to do bazy danych
	std::string query;

	const bool existed = rawExists(key, db);

	//najpierw sprawdzam czy plik istnieje
	if (existed == false){
		//insert
		query = (boost::format("INSERT INTO files_table(file_name, file) VALUES('%1%', ?);") % key).str();
	}
	else{
		//update
		query = (boost::format("UPDATE files_table SET file = ? WHERE file_name = '%1%';") % key).str();
	}

	{
		SQLitePreparedStatement store(db, query);

		if (store == nullptr){
			progress->setError("Creation of db query failed");
			return;
		}

		auto rc = sqlite3_bind_zeroblob(store, 1, streamSize);
	
		if (rc != SQLITE_OK || mtStep(store, db) != SQLITE_DONE){
			rawRemove(key, db);
			progress->setError("DB update failed");
			return;
		}
	}

	sqlite3_int64 rowid = -1;

	if (existed == false){
		rowid = sqlite3_last_insert_rowid(db);
	}
	else{
		const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name='%1%';") % key).str());
		SQLitePreparedStatement selectQuery(db, fileQuery);

		if (selectQuery != nullptr && mtStep(selectQuery) == SQLITE_ROW){
			rowid = sqlite3_column_int64(selectQuery, 0);
		}
	}

	if (rowid == -1){
		rawRemove(key, db);
		progress->setError("Failed to grab DB record for modification");
		return;
	}

	// Getting here means we have a valid file handle, f, and a valid db handle, db
	//   Also, a blank row has been inserted with key rowid
	SQLiteBLOB blob(db, "files_table", "file", rowid, 1, "main");

	if (blob.get() == nullptr) {		
		rawRemove(key, db);
		progress->setError("Failed to attach DB output for data");
		return;
	}

	const auto readSize = std::min((int)streamSize, (int)MaxBufferSize);

	boost::shared_array<char> memblock(new char[readSize]);
	input->seekg(0, std::ios::beg);
	int offset = 0;
	const unsigned int fullReads = (streamSize / readSize);

	const float progressPart = (float)readSize / (float)streamSize;
	progress->setProgress(0.0);
	for (unsigned int i = 0; i < fullReads && progress->aborted() == false; ++i){
		input->read(memblock.get(), readSize);
		auto rc = sqlite3_blob_write(blob.get(), memblock.get(), readSize, offset);
		if (rc != SQLITE_OK){
			blob.reset();
			rawRemove(key, db);
			progress->setError("Failed to write to DB output");
			return;
		}

		offset += readSize;
		progress->setProgress(progressPart * (i + 1));
	}

	if (progress->aborted() == true){
		blob.reset();
		rawRemove(key, db);
		return;
	}

	const auto left = streamSize - offset;

	if (left > 0){
		input->read(memblock.get(), left);
		auto rc = sqlite3_blob_write(blob.get(), memblock.get(), left, offset);
		if (rc != SQLITE_OK){
			blob.reset();
			rawRemove(key, db);
			progress->setError("Failed to write to DB output");
			return;
		}
	}

	//TODO
	//sprawdzac czy czasem nie bylo problemow ze zwolnieniem bloba!!
}

const bool SQLCipherStorage::rawRemove(const std::string & key, sqlite3 * db)
{
	const std::string fileDelete((boost::format("DELETE FROM files_table WHERE file_name = '%1%';") % key).str());	
	if (sqliteExec(db, fileDelete) != SQLITE_DONE) {		
		return false;
	}

	if (sqlite3_changes(db) == 0){
		return false;
	}

	return true;
}

const bool SQLCipherStorage::rawRename(const std::string & oldKey,
	const std::string & newKey, const bool overwrite, sqlite3 * db)
{
	const std::string query((boost::format("UPDATE files_table SET file_name = %1% WHERE file_name = '%2%';") % newKey % oldKey).str());
	SQLitePreparedStatement update(db, query);

	if (update != nullptr && rawExists(oldKey, db) == true){		
		if (rawExists(newKey, db) == false){			
			if (mtStep(update) == SQLITE_DONE){
				return true;
			}
		}
		else if (overwrite == true){
			if (rawRemove(newKey, db) == false){
				return false;
			}

			if (mtStep(update) == SQLITE_DONE){
				return true;
			}
		}		
	}
	
	return false;	
}

const SQLCipherStorage::Keys SQLCipherStorage::rawKeys(sqlite3 * db)
{
	SQLCipherStorage::Keys ret;

	const std::string fileQuery("SELECT file_name FROM files_table;");
	SQLitePreparedStatement query(db, fileQuery);	

	if (query != nullptr){
		int r = SQLITE_DONE;
		while ((r = mtStep(query)) == SQLITE_ROW){
			const auto key = sqlite3_column_text(query, 0);
			
			ret.insert(std::string((const char *)key));
		}

		if (r == false){
			SQLCipherStorage::Keys().swap(ret);
		}
	}	

	return ret;
}

void SQLCipherStorage::open(const core::Filesystem::Path & path,
	const std::string & key)
{	
	if (path_.empty() == false || core::Filesystem::pathExists(path) == false){
		return;
	}
	
	SQLiteDB db(path, key, false);
	if (db.get() != nullptr){
		if (checkStructure(db.get()) == false){
			db.reset();
		}
		else{
			path_ = path;
			key_ = key;
		}
	}	
}

const bool SQLCipherStorage::create(const core::Filesystem::Path & path,
	const std::string & key)
{
	if (core::Filesystem::pathExists(path) == true){
		return false;
	}	
	
	SQLiteDB db(path, key, false, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
	if (db.get() != nullptr){
		return initialize(db.get());
	}

	return false;
}

const bool SQLCipherStorage::changeKey(const core::Filesystem::Path & path,
	const std::string & oldKey, const std::string & newKey)
{
	if (core::Filesystem::pathExists(path) == false || oldKey == newKey){
		return false;
	}	

	bool ret = false;

	SQLiteDB db(path, oldKey);

	if (db.get() != nullptr){
		auto rc = sqlite3_rekey(db.get(), newKey.c_str(), newKey.size());
		if (rc == SQLITE_OK){
			db.open(path, newKey);
			ret = (db.get() != nullptr);
		}		
	}	

	return ret;
}

const bool SQLCipherStorage::setKey(const core::Filesystem::Path & path,
	const std::string & key)
{
	if (core::Filesystem::pathExists(path) == false){
		return false;
	}

	bool ret = false;
	SQLiteDB db(path);	

	if (db.get() != nullptr){
		auto rc = sqlite3_rekey(db.get(), key.c_str(), key.size());
		if (rc == SQLITE_OK){
			ret = db.verify();
		}
	}	

	return ret;
}

const bool SQLCipherStorage::verify(const core::Filesystem::Path & path,
	const std::string & key)
{
	SQLiteDB db(path, key, SQLITE_OPEN_READWRITE, false);

	if (db.get() == nullptr){
		return checkStructure(db.get());
	}	

	return false;
}

const bool SQLCipherStorage::isOpened() const
{
	return path_.empty() == false;
}

const bool SQLCipherStorage::exists(const std::string & key) const
{
	SQLiteDB db(path_, key_, false);
	return rawExists(key, db.get());
}

const IHMDBStorage::IOStreamPtr SQLCipherStorage::get(const std::string & key)
{	
	SQLiteDB db(path_, key_, false);
	return rawGet(key, db.get(), path_, key_);
}

const IHMDBStorage::IStreamPtr SQLCipherStorage::get(const std::string & key) const
{	
	SQLiteDB db(path_, key_, false);
	return rawGetReadOnly(key, db.get(), path_, key_);
}

const bool SQLCipherStorage::set(const std::string & key, IHMDBStorage::IStreamPtr input)
{
	SQLiteDB db(path_, key_, false);
	return rawSet(key, input, db.get());
}

void SQLCipherStorage::set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress)
{
	SQLiteDB db(path_, key_, false);
	rawSet(key, input, progress, db.get());
}

const bool SQLCipherStorage::remove(const std::string & key)
{	
	SQLiteDB db(path_, key_, false);
	return rawRemove(key, db.get());
}

const bool SQLCipherStorage::rename(const std::string & oldKey,
	const std::string & newKey, const bool overwrite)
{	
	SQLiteDB db(path_, key_, false);
	return rawRename(oldKey, newKey, overwrite, db.get());
}

const SQLCipherStorage::Keys SQLCipherStorage::keys() const
{	
	SQLiteDB db(path_, key_, false);
	return rawKeys(db.get());
}

const std::string SQLCipherStorage::name() const
{
	return std::string("SQLCipher");
}

const std::string SQLCipherStorage::protocol() const
{
	return std::string("sqlcipherstream://");
}

void SQLCipherStorage::close()
{
	core::Filesystem::Path().swap(path_);
	std::string().swap(key_);	
}

const SQLCipherStorage::TransactionPtr SQLCipherStorage::transaction()
{
	return TransactionPtr(new SQLCipherStorageTransaction(this));
}

const SQLCipherStorage::TransactionConstPtr SQLCipherStorage::transaction() const
{
	return TransactionPtr(new SQLCipherStorageTransaction(this));
}

const bool SQLCipherStorage::canStore(const unsigned long long size) const
{
	return core::Filesystem::availableSpace(path_) >= size;
}

const bool SQLCipherStorage::shareDiskSpace(const core::Filesystem::Path & path) const
{
	return path.root_path() == path_.root_path();
}