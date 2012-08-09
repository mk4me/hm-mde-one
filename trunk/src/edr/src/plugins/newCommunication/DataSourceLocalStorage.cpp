#include "CommunicationPCH.h"
#include "DataSourceLocalStorage.h"
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <fstream>
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
// rev - hack, z jakiegos wzgledu nie bylo widac tej deklaracji
//#define SQLITE_HAS_CODEC
//#include <sqlite3.h>
extern int sqlite3_key(
  sqlite3 *db,                   /* Database to be rekeyed */
  const void *pKey, int nKey     /* The key */
);
#include <core/PluginCommon.h>

static const char magicKey[] = "P,j.W/s<T>k2:0\"1;2";

DataSourceLocalStorage * DataSourceLocalStorage::instance_ = nullptr;

DataSourceLocalStorage::DataSourceLocalStorage() : db(nullptr), dbOpen(false)
{

}

DataSourceLocalStorage::~DataSourceLocalStorage()
{
	if(dbOpen == true){
		sqlite3_close(db);
	}
}

DataSourceLocalStorage * DataSourceLocalStorage::create()
{
	if(instance_ != nullptr){
		throw std::runtime_error("Instance already created");
	}

	instance_ = new DataSourceLocalStorage();

	return instance_;
}

void DataSourceLocalStorage::destroy()
{
	if(instance_ != nullptr){
		delete instance_;
		instance_ = nullptr;
	}
}

DataSourceLocalStorage * DataSourceLocalStorage::instance()
{
	return instance_;
}

void DataSourceLocalStorage::setLocalStorageDataPath(const core::Filesystem::Path & localStorageDataPath)
{
	core::Filesystem::createDirectory(localStorageDataPath.parent_path());

	int rc = sqlite3_open_v2(localStorageDataPath.string().c_str(), &(this->db), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

	if (rc != SQLITE_OK){
		//TODO
		//obs�uga kod�w b��du sqlite

		sqlite3_close(db);
		throw std::runtime_error("Could not initialize local storage in specified path");
	}else {

		//uda�o si� otworzy�/stworzy� baz�

		//sprawdzam czy baza zaszyfrowana
		if(checkIfEncrypted() == false){
			sqlite3_close(db);
			db = nullptr;
			encrypt(localStorageDataPath);
		}
		localStorageDataPath_ = localStorageDataPath;
		dbOpen = true;
	}
}

bool DataSourceLocalStorage::checkIfEncrypted()
{
	bool ret = false;

	int rc = sqlite3_key(this->db, magicKey, 18);

	if (rc != SQLITE_OK){
		//TODO
		//obs�uga kod�w b��du sqlite

		sqlite3_close(db);
		throw std::runtime_error("Could not decrypt local storage");
	}

	try{
		ret = checkIfInitialized();

		if(ret == false){
			initialize();
			ret = true;
		}

	}catch(...){

	}

	return ret;
}

bool DataSourceLocalStorage::checkIfInitialized()
{
	bool ret = false;

	const char* tail;
	sqlite3_stmt* res;
	static const char sqlCheckTable[] = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='files_table' ORDER BY name;";
	int rc = sqlite3_prepare_v2(db, sqlCheckTable, -1, &res, &tail);

	if (rc != SQLITE_OK || res == nullptr){
		sqlite3_finalize(res);
		throw std::runtime_error("Could not interact with DB");
	}

	if(sqlite3_step(res) == SQLITE_ROW){
		int count = sqlite3_column_int(res, 0);
		sqlite3_finalize(res);
		if(count >= 1){
			ret = true;
		}
	}

	return ret;
}

void DataSourceLocalStorage::initialize()
{
	char * error;
	static const char sqlCreateTable[] = "CREATE TABLE files_table (file_name TEXT PRIMARY KEY, file BLOB, size UNSIGNED BIG INT);";
	int rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
	if (rc != SQLITE_OK){
		sqlite3_free(error);
		throw std::runtime_error("Could not initialize DB structure");
	}
}

void DataSourceLocalStorage::encrypt(const core::Filesystem::Path & localStorageDataPath)
{
	core::Filesystem::Path tmpDB = core::getPathInterface()->getTmpPath() / "tmpDB.db";
	int rc = sqlite3_open_v2(localStorageDataPath.string().c_str(), &(this->db), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

	if (rc != SQLITE_OK){
		sqlite3_close(db);
		throw std::runtime_error("Could not open DB");
	}
	char * error = nullptr;
	std::string encryptSql((boost::format("ATTACH DATABASE '%1%' AS encrypted KEY '%2%'; SELECT sqlcipher_export('encrypted'); DETACH DATABASE encrypted") % tmpDB.string().c_str() % magicKey).str());
	rc = sqlite3_exec(db, encryptSql.c_str(), NULL, NULL, &error);
	if (rc != SQLITE_OK){
		sqlite3_free(error);
		throw std::runtime_error("Could not encrypt DB");
	}

	//zamykam aktualne po��czenie - zwalniam uchwyt do pliku - b�d� go kasowa�
	sqlite3_close(db);

	//uda�o si� zaszyfrowa� - usuwam star� baz�
	core::Filesystem::deleteFile(localStorageDataPath);
	// na jej miejsce kopiuj� now�
	core::Filesystem::copy(tmpDB, localStorageDataPath);
	// usuwam tymczasow�
	core::Filesystem::deleteFile(tmpDB);
	//otwieram nowe po��czenie z now�, zaszyfrowan� ju� baz�
	rc = sqlite3_open_v2(localStorageDataPath.string().c_str(), &(this->db), SQLITE_OPEN_READWRITE, nullptr);

	if (rc != SQLITE_OK){
		//TODO
		//obs�uga kod�w b��du sqlite

		sqlite3_close(db);
		throw std::runtime_error("Could not initialize local storage in specified path");
	}

	if(checkIfInitialized() == false){
		initialize();
	}
}

bool DataSourceLocalStorage::fileIsLocal(const std::string & fileName) const
{
	bool ret = false;

	const char* tail;
	sqlite3_stmt* res;

	std::string fileQuery((boost::format("SELECT count(*) FROM files_table WHERE file_name='%1%';") % fileName).str());

	int rc = sqlite3_prepare_v2(db, fileQuery.c_str(), -1, &res, &tail);

	if (rc != SQLITE_OK || res == nullptr){
		sqlite3_finalize(res);
		throw std::runtime_error("Could not query DB");
	}

	if(sqlite3_step(res) == SQLITE_ROW){
		int count = sqlite3_column_int(res, 0);
		sqlite3_finalize(res);
		if(count == 1){
			ret = true;
		}
	}

	return ret;
}

void DataSourceLocalStorage::loadFile(const core::Filesystem::Path & path, const std::string & fileUniqueName)
{
	std::string uniqueName(fileUniqueName);

	//je�li nie podano unikalnej nazwy pliku do zapisu przyjmujemy �e to nazwa pliku + jego rozszerzenie
	if(fileUniqueName.empty() == true){
		uniqueName = path.filename().string();
	}

	//czytamy zawarto�c pliku
	std::ifstream file (path.string().c_str(), std::ios::in|std::ios::binary);
	if (file.is_open())
	{
		auto begin = file.tellg();
		file.seekg (0, std::ios::end);
		auto end = file.tellg();
		auto size = end - begin;
		boost::shared_array<char> memblock(new char [size]);
		file.seekg (0, std::ios::beg);
		file.read(memblock.get(), size);
		auto readSize = file.gcount();
		file.close();

		//wrzucam to do bazy danych

		const char* tail;
		sqlite3_stmt* res;
		std::string querry;

		//najpierw sprawdzam czy plik istnieje
		if(fileIsLocal(uniqueName) == false){
			//insert
			querry = (boost::format("INSERT INTO files_table VALUES('%1%', ?, %2%);") % uniqueName % (size * sizeof(char))).str();
		}else{
			//update
			querry = (boost::format("UPDATE files_table SET file = ?, size = %1% WHERE file_name = '%2%';") % (size * sizeof(char)) % uniqueName).str();
		}

		int rc = sqlite3_prepare_v2(db, querry.c_str(), -1, &res, &tail);

		if (rc != SQLITE_OK || res == nullptr){
			sqlite3_finalize(res);
			throw std::runtime_error("Could not prepare querry");
		}

		sqlite3_bind_blob(res, 1, memblock.get(), size * sizeof(char), SQLITE_STATIC);

		auto ret = sqlite3_step(res);
		sqlite3_finalize(res);
		if(ret != SQLITE_DONE){
			throw std::runtime_error("DB execution error");
		}
	}else{
		throw std::runtime_error("Could not open source file to load");
	}
}

void DataSourceLocalStorage::extractFile(const std::string & fileName, const core::Filesystem::Path & destPath)
{
	//pr�bujemy strworzy� plik do zapisu

	std::ofstream file (destPath.string().c_str(), std::ios::out|std::ios::binary);
	if (file.is_open())
	{
		//pobieramy dane z bazy
		const char* tail;
		sqlite3_stmt* res;

		std::string fileQuery((boost::format("SELECT file, size FROM files_table WHERE file_name = '%1%';") % fileName).str());

		int rc = sqlite3_prepare_v2(db, fileQuery.c_str(), -1, &res, &tail);

		if (rc != SQLITE_OK){
			throw std::runtime_error("Could not query DB");
		}

		if(sqlite3_step(res) == SQLITE_ROW){
			auto fContent = sqlite3_column_blob(res, 0);
			//mamy to w sumie ju� w jednej kolumnie zapisane!!
			//nie moge u�yc column_bytes bo to tylko int!!
			unsigned long long size = sqlite3_column_int64(res, 1);

			//zapis do pliku
			try{
				file.write((const char*)fContent, size);
				file.close();
			}catch(...){

				//pr�buje zamkn�� plik
				try{
					if(file.is_open()){
						file.close();
					}
				}catch(...){

				}
				//pr�buje usun�� plik
				try{
					core::Filesystem::deleteFile(destPath);
				}catch(...){

				}

				//zwalniam zasoby bazy danych
				sqlite3_finalize(res);

				//rzucam dalej
				throw;
			}
			//zwalniamy zasoby bazy danych
			sqlite3_finalize(res);
		}else{
			throw std::runtime_error("File not stored in DB");
		}
	}else{
		throw std::runtime_error("Could not open file for writing");
	}
}

void DataSourceLocalStorage::removeFile(const std::string & fileName)
{
	std::string fileDelete((boost::format("DELETE FROM files_table WHERE file_name = '%1%';") % fileName).str());
	char * error;
	int rc = sqlite3_exec(db, fileDelete.c_str(), nullptr, nullptr, &error);
	if (rc) {
		sqlite3_free(error);
		throw std::runtime_error("Could not delete file from DB");
	}
}
