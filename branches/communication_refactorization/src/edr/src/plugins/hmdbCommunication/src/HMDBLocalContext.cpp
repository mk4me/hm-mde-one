#include "CommunicationPCH.h"
#include "HMDBLocalContext.h"
#include "ShallowCopyUtils.h"
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <corelib/IParserManagerReader.h>

using namespace hmdbCommunication;

class StorageStreamGrabber : public core::IStreamDataManager::IStreamGrabber
{
public:

	//! \param storage Obiekt z danymi
	//! \param name Nazwa strumienia do wypakowania
	StorageStreamGrabber(const IHMDBStorageConstPtr storage, const std::string & name)
		: storage(storage), name_(name)
	{

	}

	//! Destruktor wirtualny
	virtual ~StorageStreamGrabber()
	{

	}

	//! \return Strumieñ
	virtual const core::IStreamDataManager::StreamPtr stream() const
	{
		return core::IStreamDataManager::StreamPtr(storage->get(name_));
	}

	//! \return Nazwa, identyfikator strumienia
	virtual const std::string name() const
	{
		return core::IStreamManagerReader::streamName(storage->protocol(), name_);
	}

	//! \return Nazwa, identyfikator strumienia
	virtual const std::string rawName() const
	{
		return name_;
	}

private:
	//! Obiekt z danymi
	const IHMDBStorageConstPtr storage;
	//! Nazwa strumienia do wypakowania
	const std::string name_;
};

/*

//próbuje pliki vsk wypakowaæ z zip

auto zipName = sessionName + ".zip";
auto vskName = sessionName + ".vsk";

if (file->fileName == zipName){
	//mam zipa - szukam vsk w zip
	auto vskDestPath = pathsManager->filePath(vskName, sessionName);
	QuaZip zip(filePath.string().c_str());

	if (zip.open(QuaZip::mdUnzip) == true){
		//uda³o sie otworzyæ plik do wypakowywania
		auto files = zip.getFileNameList();
		if (files.contains(QString::fromUtf8(vskName.c_str())) == true &&
			zip.setCurrentFile(QString::fromUtf8(vskName.c_str())) == true){
			//znalaz³em vsk - próbujemy go wypakowaæ
			QuaZipFile inFile(&zip);
			if (inFile.open(QIODevice::ReadOnly) && inFile.getZipError() == UNZ_OK) {
				//tworze plik docelowy
				QFile outFile;
				outFile.setFileName(vskDestPath.string().c_str());
				if (outFile.open(QIODevice::WriteOnly)) {
					//uda³o siê - mogê kopiowaæ
					auto ok = copyData(inFile, outFile);
					ok &= (inFile.getZipError() == UNZ_OK);

					//zamykam plik docelowy
					outFile.close();

					//zamykam plik w archiwum zip
					inFile.close();

					if (ok == false || inFile.getZipError() != UNZ_OK) {
						core::Filesystem::deleteFile(vskDestPath);
					}
				}
			}
		}
	}
}

*/

HMDBLocalContext::HMDBLocalContext(IHMDBDataContextPtr dataContext, core::IMemoryDataManager * mdm,
	core::IStreamDataManager * sdm) : dataContext_(dataContext), mdm(mdm), sdm(sdm)
{

}

HMDBLocalContext::~HMDBLocalContext()
{

}

const IHMDBDataContextPtr HMDBLocalContext::dataContext()
{
	return dataContext_;
}

const IHMDBDataContextConstPtr HMDBLocalContext::dataContext() const
{
	return dataContext_;
}

const bool HMDBLocalContext::isMyData(core::VariantConstPtr data) const
{
	//threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	auto mT = mdm->transaction();
	return rawIsMyData(data);
}

const core::ConstVariantsList HMDBLocalContext::data(const std::string & fileName) const
{
	//threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	auto sT = sdm->transaction();
	return rawData(fileName, sT);
}

const bool HMDBLocalContext::load(const std::string & fileName)
{
	//threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return rawLoad(fileName, sdm->transaction());
}

const bool HMDBLocalContext::unload(const std::string & fileName)
{
	//threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return rawUnload(fileName, sdm->transaction());
}

const bool HMDBLocalContext::isLoaded(const std::string & fileName) const
{
	return rawIsLoaded(fileName, sdm->transaction());
}

const bool HMDBLocalContext::load(const core::VariantPtr data)
{
	//threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return rawLoad(data, mdm->transaction());
}

const bool HMDBLocalContext::unload(const core::VariantConstPtr data)
{
	//threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(synch);
	return rawUnload(data, mdm->transaction());
}

const HMDBLocalContext::TransactionPtr HMDBLocalContext::transaction()
{
	//TODO
	return TransactionPtr();
}

const HMDBLocalContext::TransactionConstPtr HMDBLocalContext::transaction() const
{
	//TODO
	return TransactionConstPtr();
}

const bool HMDBLocalContext::rawLoad(const std::string & fileName, const core::IStreamDataManager::TransactionPtr streamTransaction)
{
	bool ret = false;

	auto dsTransaction = dataContext()->storage()->transaction();		

	if (rawIsLoaded(fileName, streamTransaction) == false){
		core::IStreamDataManager::StreamGrabberPtr sg(new StorageStreamGrabber(dataContext_->storage(), fileName));
		streamTransaction->addStream(sg);
		core::ConstVariantsList oList;
		streamTransaction->getObjects(sg->name(), oList);
		myData_.insert(myData_.end(), oList.begin(), oList.end());
		ret = true;
	}

	return ret;
}

const bool HMDBLocalContext::rawUnload(const std::string & fileName,
	const core::IStreamDataManager::TransactionPtr streamTransaction)
{
	bool ret = false;

	const auto sName = core::IStreamManagerReader::streamName(dataContext_->storage()->protocol(), fileName);

	if (streamTransaction->isManaged(sName) == true){		
		streamTransaction->removeStream(sName);
		ret = true;
	}

	return ret;
}

const bool HMDBLocalContext::rawIsLoaded(const std::string & fileName, const core::IStreamDataManager::TransactionPtr streamTransaction) const
{
	const auto sName = core::IStreamManagerReader::streamName(dataContext_->storage()->protocol(), fileName);
	return streamTransaction->isManaged(sName);
}

const bool HMDBLocalContext::rawLoad(const core::VariantPtr data, const core::IMemoryDataManager::TransactionPtr memoryTransaction)
{
	bool ret = false;

	if (memoryTransaction->isManaged(data) == false){
		memoryTransaction->addData(data);
		myData_.push_back(data);
		ret = true;
	}

	return ret;
}

const bool HMDBLocalContext::rawUnload(const core::VariantConstPtr data, const core::IMemoryDataManager::TransactionPtr memoryTransaction)
{
	bool ret = false;	

	auto it = std::find(myData_.begin(), myData_.end(), data);

	if (it != myData_.end()){

		if (memoryTransaction->isManaged(data) == true){
			memoryTransaction->removeData(data);
		}
		else{
			myData_.erase(it);
		}

		ret = true;
	}

	return ret;
}

const bool HMDBLocalContext::rawIsMyData(core::VariantConstPtr data) const
{
	return std::find(myData_.begin(), myData_.end(), data) != myData_.end();
}

const core::ConstVariantsList HMDBLocalContext::rawData(const std::string & fileName, const core::IStreamDataManager::TransactionPtr streamTransaction) const
{
	const auto sName = core::IStreamManagerReader::streamName(dataContext_->storage()->protocol(), fileName);
	
	core::ConstVariantsList oList;
	
	if (streamTransaction->isManaged(sName) == true){
		streamTransaction->getObjects(sName, oList);
	}

	return oList;
}

void HMDBLocalContext::observe(const core::IDataManagerReader::ChangeList & changes)
{
	for (auto it = changes.begin(); it != changes.end(); ++it){
		
		auto oc = *it;
		
		if (oc.modyfication == core::IDataManagerReader::REMOVE_OBJECT){
			myData_.remove(oc.previousValue);
		}
	}
}