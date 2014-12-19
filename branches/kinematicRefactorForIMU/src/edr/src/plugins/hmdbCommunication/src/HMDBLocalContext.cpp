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

	//! \return Strumie�
	virtual const plugin::IStreamParser::IStreamPtr stream() const
	{
		return plugin::IStreamParser::IStreamPtr(storage->get(name_));
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

class HMDBLocalContext::HMDBLocalContextTransaction : public IHMDBLocalContextOperations
{
public:
	HMDBLocalContextTransaction(HMDBLocalContext * localContext)
		: localContext(localContext), mdm(localContext->mdm->transaction()),
		sdm(localContext->sdm->transaction())
	{

	}

	virtual ~HMDBLocalContextTransaction()
	{

	}

	//! \return Czy dane o kt�re pytamy pochodza z tego �r�d�a
	virtual const bool isMyData(const core::VariantConstPtr data) const
	{
		return localContext->rawIsMyData(data, mdm);
	}

	virtual const bool isMyData(const void * data) const
	{
		return localContext->isMyData(data);
	}

	//! \param fileName Nazwa pliku ze storage
	//! \return Dane skojarzone z tym plikiem
	virtual const core::ConstVariantsList data(const std::string & fileName) const
	{
		return localContext->rawData(fileName, sdm);
	}

	//! \param fileName Plik jaki b�d� �adowa� ze storage do StreamManagera
	//! \return Czy uda�o si� za�adowac plik
	virtual const bool load(const std::string & fileName)
	{
		return localContext->rawLoad(fileName, sdm);
	}
	//! \param fileName Plik jaki b�d� wy�adowywa� ze StreamManagera
	//! \return Czy uda�o si� wy�adowac plik
	virtual const bool unload(const std::string & fileName)
	{
		return localContext->rawUnload(fileName, sdm);
	}
	//! \param fileName Plik o kt�ry pytam czy jest za��dowany
	//! \return Czy plik jest za�adowany
	virtual const bool isLoaded(const std::string & fileName) const
	{
		return localContext->rawIsLoaded(fileName, sdm);
	}

	//! \param data Dane jakie b�d� �adowa� do MemoryManagera
	//! \return Czy uda�o si� za�adowac dane
	virtual const bool load(const core::VariantPtr data)
	{
		return localContext->rawLoad(data, mdm);
	}
	//! \param data Dane jakie b�d� wy�adowa� z MemoryManagera
	//! \return Czy uda�o si� wy�adowac dane
	virtual const bool unload(const core::VariantConstPtr data)
	{
		return localContext->rawUnload(data, mdm);
	}

private:
	//! Transakcja MDM
	core::IMemoryDataManager::TransactionPtr mdm;
	//! Transakcja SDM
	core::IStreamDataManager::TransactionPtr sdm;
	//! Lokalny kontekst
	HMDBLocalContext * localContext;
};

/*

//pr�buje pliki vsk wypakowa� z zip

auto zipName = sessionName + ".zip";
auto vskName = sessionName + ".vsk";

if (file->fileName == zipName){
	//mam zipa - szukam vsk w zip
	auto vskDestPath = pathsManager->filePath(vskName, sessionName);
	QuaZip zip(filePath.string().c_str());

	if (zip.open(QuaZip::mdUnzip) == true){
		//uda�o sie otworzy� plik do wypakowywania
		auto files = zip.getFileNameList();
		if (files.contains(QString::fromUtf8(vskName.c_str())) == true &&
			zip.setCurrentFile(QString::fromUtf8(vskName.c_str())) == true){
			//znalaz�em vsk - pr�bujemy go wypakowa�
			QuaZipFile inFile(&zip);
			if (inFile.open(QIODevice::ReadOnly) && inFile.getZipError() == UNZ_OK) {
				//tworze plik docelowy
				QFile outFile;
				outFile.setFileName(vskDestPath.string().c_str());
				if (outFile.open(QIODevice::WriteOnly)) {
					//uda�o si� - mog� kopiowa�
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

const bool HMDBLocalContext::isMyData(const core::VariantConstPtr data) const
{
	return rawIsMyData(data, mdm->transaction());
}

const bool HMDBLocalContext::isMyData(const void * data) const
{
	return rawIsMyData(data, mdm->transaction());
}

const core::ConstVariantsList HMDBLocalContext::data(const std::string & fileName) const
{
	return rawData(fileName, sdm->transaction());
}

const bool HMDBLocalContext::load(const std::string & fileName)
{
	return rawLoad(fileName, sdm->transaction());
}

const bool HMDBLocalContext::unload(const std::string & fileName)
{
	return rawUnload(fileName, sdm->transaction());
}

const bool HMDBLocalContext::isLoaded(const std::string & fileName) const
{
	return rawIsLoaded(fileName, sdm->transaction());
}

const bool HMDBLocalContext::load(const core::VariantPtr data)
{
	return rawLoad(data, mdm->transaction());
}

const bool HMDBLocalContext::unload(const core::VariantConstPtr data)
{
	return rawUnload(data, mdm->transaction());
}

const HMDBLocalContext::TransactionPtr HMDBLocalContext::transaction()
{
	return TransactionPtr(new HMDBLocalContextTransaction(this));
}

const HMDBLocalContext::TransactionConstPtr HMDBLocalContext::transaction() const
{
	return TransactionConstPtr(new HMDBLocalContextTransaction(const_cast<HMDBLocalContext*>(this)));
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

const bool HMDBLocalContext::rawIsMyData(const core::VariantConstPtr data, const core::IMemoryDataManager::TransactionPtr memoryTransaction) const
{	
	return std::find(myData_.begin(), myData_.end(), data) != myData_.end();
}

const bool HMDBLocalContext::rawIsMyData(const void * data, const core::IMemoryDataManager::TransactionPtr memoryTransaction) const
{
	auto it = std::find_if(myData_.begin(), myData_.end(), [=](const core::VariantConstPtr d) -> bool {
		if (d->getRawPtr() == data){
			return true;
		}

		return false;
	});


	return it != myData_.end();
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