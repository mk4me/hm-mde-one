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

	virtual const IOStreamPtr get(const std::string & key)
	{
		return storage->rawGet(key);
	}

	virtual const IStreamPtr get(const std::string & key) const
	{
		return storage->rawGet(key);
	}

	virtual const bool set(const std::string & key, IStreamPtr input)
	{
		return storage->rawSet(key, input);
	}

	virtual void set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress, const float div)
	{
		storage->rawSet(key, input, progress, div);
	}

	virtual const bool remove(const std::string & key)
	{
		return storage->rawRemove(key);
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

	virtual const bool canStore(const unsigned long long size) const
	{
		return storage->canStore(size);
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

const IHMDBStorage::IOStreamPtr MemoryStorage::get(const std::string & key)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawGet(key);
}

const IHMDBStorage::IStreamPtr MemoryStorage::get(const std::string & key) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawGet(key);
}

const bool MemoryStorage::set(const std::string & key, IHMDBStorage::IStreamPtr input)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawSet(key, input);
}

void MemoryStorage::set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress, const float div)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	rawSet(key, input, progress, div);
}

const bool MemoryStorage::remove(const std::string & key)
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

const bool MemoryStorage::canStore(const unsigned long long size) const
{
	//TODO
	//crossplatform free memory
	return true;
}

const std::string MemoryStorage::name() const
{
	return "MemoryStorage";
}

const std::string MemoryStorage::protocol() const
{
	return "memory";
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

const IHMDBStorage::IOStreamPtr MemoryStorage::rawGet(const std::string & key)
{
	auto it = storage.find(key);
	if (it != storage.end()){
		return IHMDBStorage::IOStreamPtr(new std::stringstream(it->second));
	}

	return IHMDBStorage::IOStreamPtr();
}

const IHMDBStorage::IStreamPtr MemoryStorage::rawGet(const std::string & key) const
{
	auto it = storage.find(key);
	if (it != storage.end()){
		return IHMDBStorage::IStreamPtr(new std::stringstream(it->second));
	}

	return IHMDBStorage::IStreamPtr();
}

const bool MemoryStorage::rawSet(const std::string & key, IStreamPtr input)
{
	auto str = utils::readStream(input.get());
	auto it = storage.find(key);
	if (it != storage.end()){
		it->second = str;
	}
	else{
		storage.insert(Storage::value_type(key, str));
	}

	return true;
}

void MemoryStorage::rawSet(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress, const float div)
{
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

	const auto readSize = std::min((int)streamSize, (int)MaxBufferSize);

	boost::shared_array<char> memblock(new char[readSize]);
	input->seekg(0, std::ios::beg);
	int offset = 0;
	const unsigned int fullReads = (streamSize / readSize);

	const float progressPart = ((float)readSize / (float)streamSize) / div;

	std::string str;

	progress->setProgress(0.0);
	for (unsigned int i = 0; i < fullReads && progress->aborted() == false; ++i){
		input->read(memblock.get(), readSize);
		str.append(memblock.get(), readSize);
		offset += readSize;
		progress->setProgress(progressPart * (i + 1));
	}

	if (progress->aborted() == true){
		return;
	}

	const auto left = streamSize - offset;

	if (left > 0){
		input->read(memblock.get(), left);
		str.append(memblock.get(), left);
	}

	auto it = storage.find(key);
	if (it != storage.end()){
		it->second = str;
	}
	else{
		storage.insert(Storage::value_type(key, str));
	}
}

const bool MemoryStorage::rawRemove(const std::string & key)
{
	return storage.erase(key) > 0;
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