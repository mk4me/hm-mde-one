#include "CommunicationPCH.h"
#include "MemoryStorage.h"

using namespace hmdbCommunication;


class MemoryStorage::MemoryStorageTransaction : public hmdbCommunication::IHMDBStorageOperations
{
public:
	MemoryStorageTransaction(MemoryStorage * storage)
		: storage(storage), lock(storage->sync_)
	{

	}

	virtual ~MemoryStorageTransaction()
	{

	}

	virtual const bool exists(const std::string & key) const
	{
		return storage->rawExists(key);
	}

	virtual std::iostream * get(const std::string & key)
	{
		return storage->rawGet(key);
	}

	virtual std::istream * get(const std::string & key) const
	{
		return storage->rawGet(key);
	}

	virtual const bool set(const std::string & key, std::istream * input)
	{
		return storage->rawSet(key, input);
	}

	virtual void remove(const std::string & key)
	{
		storage->rawRemove(key);
	}

	virtual const bool rename(const std::string & oldKey,
		const std::string & newKey, const bool overwrite = false)
	{
		return storage->rawRename(oldKey, newKey, overwrite);
	}

	virtual const Keys keys() const
	{
		return storage->rawKeys();
	}

private:
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock;
	MemoryStorage * const storage;

};


MemoryStorage::MemoryStorage()
{

}


MemoryStorage::~MemoryStorage()
{

}

const bool MemoryStorage::isOpened() const
{
	return true;
}

const bool MemoryStorage::exists(const std::string & key) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawExists(key);
}

std::iostream * MemoryStorage::get(const std::string & key)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawGet(key);
}

std::istream * MemoryStorage::get(const std::string & key) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawGet(key);
}

const bool MemoryStorage::set(const std::string & key, std::istream * input)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawSet(key, input);
}

void MemoryStorage::remove(const std::string & key)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawRemove(key);
}

void MemoryStorage::close()
{

}

const bool MemoryStorage::rename(const std::string & oldKey,
	const std::string & newKey, const bool overwrite)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawRename(oldKey, newKey, overwrite);
}

const MemoryStorage::Keys MemoryStorage::keys() const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawKeys();
}

const std::string MemoryStorage::name() const
{
	return "MemoryStorage";
}

const std::string MemoryStorage::protocol() const
{
	return "mem";
}

const MemoryStorage::TransactionPtr MemoryStorage::transaction()
{
	return TransactionPtr(new MemoryStorageTransaction(this));
}

const MemoryStorage::TransactionConstPtr MemoryStorage::transaction() const
{
	return TransactionConstPtr(new MemoryStorageTransaction(const_cast<MemoryStorage*>(this)));
}

const bool MemoryStorage::rawExists(const std::string & key) const
{
	return storage.find(key) != storage.end();
}

std::iostream * MemoryStorage::rawGet(const std::string & key)
{
	auto it = storage.find(key);
	if (it != storage.end()){
		return new std::stringstream(it->second);
	}

	return nullptr;
}

std::istream * MemoryStorage::rawGet(const std::string & key) const
{
	auto it = storage.find(key);
	if (it != storage.end()){
		return new std::stringstream(it->second);
	}

	return nullptr;
}

const bool MemoryStorage::rawSet(const std::string & key, std::istream * input)
{
	auto str = utils::readStream(input);
	auto it = storage.find(key);
	if (it != storage.end()){
		it->second = str;
	}
	else{
		storage.insert(Storage::value_type(key, str));
	}

	return true;
}

void MemoryStorage::rawRemove(const std::string & key)
{
	storage.erase(key);
}

const bool MemoryStorage::rawRename(const std::string & oldKey,
	const std::string & newKey, const bool overwrite)
{
	auto oit = storage.find(oldKey);

	if (oit == storage.end()){
		return false;
	}

	auto it = storage.find(newKey);
	if (it != storage.end()){
		if (overwrite == false){
			return false;
		}

		it->second = oit->second;
	}
	else{
		storage.insert(Storage::value_type(newKey, oit->second));
	}

	storage.erase(oit);

	return true;
}

const MemoryStorage::Keys MemoryStorage::rawKeys() const
{
	Keys ret;

	for (auto it = storage.begin(); it != storage.end(); ++it){
		ret.insert(it->first);
	}

	return ret;
}