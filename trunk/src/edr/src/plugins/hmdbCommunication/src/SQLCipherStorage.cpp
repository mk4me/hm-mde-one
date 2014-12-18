#include "CommunicationPCH.h"
#include "SQLCipherStorage.h"
#include <streambuf>

#include <utils/SmartPtr.h>
#include <boost/format.hpp>
#include <boost/array.hpp>
#include <sqlite3.h>
#include <sqliteUtils/BufferPolicyT.h>
#include <sqliteUtils/SQLiteBLOB.h>
#include <sqliteUtils/SQLiteDB.h>
#include <sqliteUtils/SQLitePreparedStatement.h>
#include <sqliteUtils/SQLiteBLOBStreamBufferT.h>


static const unsigned int sqliteExecWaitMS = 50;
static const unsigned int maxSqliteExecTries = 2400;

template<class Base, class Buffer>
class StreamWrapper : public Base
{
public:

	explicit StreamWrapper(Buffer *_Strbuf,
		bool _Isstd) : Base(_Strbuf, _Isstd)		
	{	// construct from stream buffer pointer		
	}

	explicit StreamWrapper(Buffer *_Strbuf)
		: Base(_Strbuf)
	{	// construct from stream buffer pointer
	}

	StreamWrapper(const StreamWrapper&) = delete;
	StreamWrapper& operator=(const StreamWrapper&) = delete;

	virtual ~StreamWrapper()
	{	// destroy the object
		auto toDelete = Base::rdbuf();
		Base::rdbuf(nullptr);
		delete toDelete;
	}
};

const bool initialize(sqlite3 * db)
{
	auto ret = sqliteUtils::SQLiteDB::exec(db, "CREATE TABLE files_table (file_name TEXT PRIMARY KEY, file BLOB);", maxSqliteExecTries, sqliteExecWaitMS);
	return ret == SQLITE_DONE || ret == SQLITE_OK;
}

const bool checkStructure(sqlite3 * db)
{
	static const std::string sqlCheckTable = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='files_table' ORDER BY name;";

	bool ret = false;	
	sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, sqlCheckTable), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
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
		: storage(storage), db(sqliteUtils::SQLiteDB::open(storage->path_.string(), storage->key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE),
		sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS))
	{		
		if (db == nullptr){
			throw core::runtime_error("Failed to create transaction");
		}
	}

	virtual ~SQLCipherStorageTransaction()
	{
		
	}

	virtual const bool exists(const std::string & key) const
	{
		return SQLCipherStorage::rawExists(key, db);
	}
	
	virtual const IOStreamPtr get(const std::string & key)
	{
		return SQLCipherStorage::rawGet(key, db, storage->path_, storage->key_);
	}
	
	virtual const IStreamPtr get(const std::string & key) const
	{
		return SQLCipherStorage::rawGetReadOnly(key, db, storage->path_, storage->key_);
	}
	
	virtual const bool set(const std::string & key, IStreamPtr input)
	{
		return SQLCipherStorage::rawSet(key, input, db);
	}
		
	virtual void set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress)
	{
		return SQLCipherStorage::rawSet(key, input, progress, db);
	}
	
	virtual const bool remove(const std::string & key)
	{
		return SQLCipherStorage::rawRemove(key, db);
	}
	
	virtual const bool rename(const std::string & oldKey,
		const std::string & newKey, const bool overwrite = false)
	{
		return SQLCipherStorage::rawRename(oldKey, newKey, overwrite, db);
	}
	
	virtual const Keys keys() const
	{
		return SQLCipherStorage::rawKeys(db);
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
	sqliteUtils::SQLiteDB::Wrapper db;
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

	sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
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
	//pobieramy dane z bazy
	const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());
	sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));	

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(query, 0);
		sqliteUtils::SQLiteBLOB::Wrapper blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowID, 0), sqliteUtils::SQLiteBLOB::Close(maxSqliteExecTries, sqliteExecWaitMS));
		std::auto_ptr<sqliteUtils::SQLiteBLOBStreamBufferT<>> buf(new sqliteUtils::SQLiteBLOBStreamBufferT<>(path.string(), "files_table", "file", rowID, sqlite3_blob_bytes(blob), "main", dbKey));

		return IHMDBStorage::IOStreamPtr(new StreamWrapper<std::iostream, sqliteUtils::SQLiteBLOBStreamBufferT<>>(buf.release()));
	}

	return IHMDBStorage::IOStreamPtr();
}

const IHMDBStorage::IStreamPtr SQLCipherStorage::rawGetReadOnly(const std::string & key, sqlite3 * db,
	const core::Filesystem::Path & path, const std::string & dbKey)
{
	const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());
	sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(query, 0);
		sqliteUtils::SQLiteBLOB::Wrapper blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowID, 0), sqliteUtils::SQLiteBLOB::Close(maxSqliteExecTries, sqliteExecWaitMS));
		std::auto_ptr<sqliteUtils::SQLiteBLOBStreamBufferT<>> buf(new sqliteUtils::SQLiteBLOBStreamBufferT<>(path.string(), "files_table", "file", rowID, sqlite3_blob_bytes(blob), "main", dbKey, std::ios_base::in));

		return IHMDBStorage::IStreamPtr(new StreamWrapper<std::istream, sqliteUtils::SQLiteBLOBStreamBufferT<>>(buf.release()));
	}

	return IHMDBStorage::IStreamPtr();
}

const bool SQLCipherStorage::rawSet(const std::string & key, IHMDBStorage::IStreamPtr input, sqlite3 * db)
{
	if (input == nullptr){
		return false;
	}

	const auto streamSize = utils::streamSize(*input);

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
		sqliteUtils::SQLitePreparedStatement::Wrapper store(sqliteUtils::SQLitePreparedStatement::prepare(db, query), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

		if (store == nullptr){
			return false;
		}

		auto rc = sqlite3_bind_zeroblob(store, 1, streamSize);

		if (rc != SQLITE_OK || sqliteUtils::SQLitePreparedStatement::step(store) != SQLITE_DONE){
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
		sqliteUtils::SQLitePreparedStatement::Wrapper selectQuery(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

		if (selectQuery != nullptr && sqliteUtils::SQLitePreparedStatement::step(selectQuery) == SQLITE_ROW){
			rowid = sqlite3_column_int64(selectQuery, 0);
		}		
	}

	if (rowid == -1){
		rawRemove(key, db);
		return false;
	}

	// Getting here means we have a valid file handle, f, and a valid db handle, db
	//   Also, a blank row has been inserted with key rowid

	sqliteUtils::SQLiteBLOB::Wrapper blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowid, 1, "main"), sqliteUtils::SQLiteBLOB::Close(maxSqliteExecTries, sqliteExecWaitMS));

	if (blob == nullptr) {		
		rawRemove(key, db);
		return false;
	}

	const auto readSize = std::min((int)streamSize, (int)sqliteUtils::MaxBufferSize);

	utils::shared_array<char> memblock(new char[readSize] {0}, utils::array_deleter<char>());
	input->seekg(0, std::ios::beg);
	int offset = 0;
	const unsigned int fullReads = (streamSize / readSize);
	for (unsigned int i = 0; i < fullReads; ++i){	
		input->read(memblock.get(), readSize);		
		auto rc = sqlite3_blob_write(blob, memblock.get(), readSize, offset);
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
		auto rc = sqlite3_blob_write(blob, memblock.get(), left, offset);
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

	const auto streamSize = utils::streamSize(*input);

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
		sqliteUtils::SQLitePreparedStatement::Wrapper store(sqliteUtils::SQLitePreparedStatement::prepare(db, query), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

		if (store == nullptr){
			progress->setError("Creation of db query failed");
			return;
		}

		auto rc = sqlite3_bind_zeroblob(store, 1, streamSize);
	
		if (rc != SQLITE_OK || sqliteUtils::SQLitePreparedStatement::step(store) != SQLITE_DONE){
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
		sqliteUtils::SQLitePreparedStatement::Wrapper selectQuery(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

		if (selectQuery != nullptr && sqliteUtils::SQLitePreparedStatement::step(selectQuery) == SQLITE_ROW){
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
	sqliteUtils::SQLiteBLOB::Wrapper blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowid, 1, "main"), sqliteUtils::SQLiteBLOB::Close(maxSqliteExecTries, sqliteExecWaitMS));

	if (blob == nullptr) {		
		rawRemove(key, db);
		progress->setError("Failed to attach DB output for data");
		return;
	}

	const auto readSize = std::min((int)streamSize, (int)sqliteUtils::MaxBufferSize);

	utils::shared_array<char> memblock(new char[readSize] {0}, utils::array_deleter<char>());
	input->seekg(0, std::ios::beg);
	int offset = 0;
	const unsigned int fullReads = (streamSize / readSize);
	
	const float progressPart = (float)readSize / (float)streamSize;	
	for (unsigned int i = 0; i < fullReads && progress->aborted() == false; ++i){
		input->read(memblock.get(), readSize);
		auto rc = sqlite3_blob_write(blob, memblock.get(), readSize, offset);
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
		auto rc = sqlite3_blob_write(blob, memblock.get(), left, offset);
		if (rc != SQLITE_OK){
			blob.reset();
			rawRemove(key, db);
			progress->setError("Failed to write to DB output");
			return;
		}
	}

	progress->setProgress(1.0);

	//TODO
	//sprawdzac czy czasem nie bylo problemow ze zwolnieniem bloba!!
}

const bool SQLCipherStorage::rawRemove(const std::string & key, sqlite3 * db)
{
	const std::string fileDelete((boost::format("DELETE FROM files_table WHERE file_name = '%1%';") % key).str());	
	if (sqliteUtils::SQLiteDB::exec(db, fileDelete) != SQLITE_DONE) {		
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
	sqliteUtils::SQLitePreparedStatement::Wrapper update(sqliteUtils::SQLitePreparedStatement::prepare(db, query), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));	

	if (update != nullptr && rawExists(oldKey, db) == true){		
		if (rawExists(newKey, db) == false){			
			if (sqliteUtils::SQLitePreparedStatement::step(update) == SQLITE_DONE){
				return true;
			}
		}
		else if (overwrite == true){
			if (rawRemove(newKey, db) == false){
				return false;
			}

			if (sqliteUtils::SQLitePreparedStatement::step(update) == SQLITE_DONE){
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
	sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	if (query != nullptr){
		int r = SQLITE_DONE;
		while ((r = sqliteUtils::SQLitePreparedStatement::step(query)) == SQLITE_ROW){
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
	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path.string(), key, SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	if (db != nullptr){
		if (checkStructure(db) == false){
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
	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path.string(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	if (db != nullptr){
		auto ret = initialize(db);
		if (ret == true) {
			if (sqlite3_key(db, key.c_str(), key.size()) == SQLITE_OK) {
				return ret;
			} else {
				return false;
			}
		}
		return ret;
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

	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path.string(), oldKey, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));	

	if (db != nullptr){
		auto rc = sqlite3_rekey(db, newKey.c_str(), newKey.size());
		if (rc == SQLITE_OK){
			db.reset(sqliteUtils::SQLiteDB::open(path.string(), newKey, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE));
			ret = (db != nullptr);
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

	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path.string(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));	

	if (db != nullptr){
		auto rc = sqlite3_rekey(db, key.c_str(), key.size());
		if (rc == SQLITE_OK){
			ret = sqliteUtils::SQLiteDB::verify(db);
		}
	}	

	return ret;
}

const bool SQLCipherStorage::verify(const core::Filesystem::Path & path,
	const std::string & key)
{	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path.string(), key, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));

	if (db == nullptr){
		return checkStructure(db);
	}	

	return false;
}

const bool SQLCipherStorage::isOpened() const
{
	return path_.empty() == false;
}

const bool SQLCipherStorage::exists(const std::string & key) const
{
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));	
	return rawExists(key, db);
}

const IHMDBStorage::IOStreamPtr SQLCipherStorage::get(const std::string & key)
{	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	return rawGet(key, db, path_, key_);
}

const IHMDBStorage::IStreamPtr SQLCipherStorage::get(const std::string & key) const
{	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	return rawGetReadOnly(key, db, path_, key_);
}

const bool SQLCipherStorage::set(const std::string & key, IHMDBStorage::IStreamPtr input)
{
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	return rawSet(key, input, db);
}

void SQLCipherStorage::set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress)
{
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	rawSet(key, input, progress, db);
}

const bool SQLCipherStorage::remove(const std::string & key)
{	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	return rawRemove(key, db);
}

const bool SQLCipherStorage::rename(const std::string & oldKey,
	const std::string & newKey, const bool overwrite)
{	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	return rawRename(oldKey, newKey, overwrite, db);
}

const SQLCipherStorage::Keys SQLCipherStorage::keys() const
{	
	sqliteUtils::SQLiteDB::Wrapper db(sqliteUtils::SQLiteDB::open(path_.string(), key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	return rawKeys(db);
}

const std::string SQLCipherStorage::name() const
{
	return std::string("SQLCipher");
}

const std::string SQLCipherStorage::protocol() const
{
	return std::string("sqlcipher");
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
