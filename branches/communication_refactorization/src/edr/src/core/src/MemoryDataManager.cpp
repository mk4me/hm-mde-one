#include "CorePCH.h"
#include "MemoryDataManager.h"
#include "DataHierarchyManager.h"
#include "ApplicationCommon.h"

using namespace core;

class MemoryDataManager::MemoryTransaction : public IMemoryDataManager::IMemoryDataTransaction
{
private:
	MemoryDataManager * mdm;
	utils::shared_ptr<threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy>> lock;
	MemoryDataManager::ChangeList modyfications;
	bool transactionRollbacked;

public:
	MemoryTransaction(MemoryDataManager * mdm) : mdm(mdm),
		lock(new threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy>(mdm->sync)),
		transactionRollbacked(false)
	{		
	}

	~MemoryTransaction()
	{
		if(transactionRollbacked == false){
			if(modyfications.empty() == false){
				mdm->updateObservers(modyfications);
			}			
		}
	}

public:

	virtual const bool isRolledback() const
	{
		return transactionRollbacked;
	}

	virtual void rollback()
	{
		transactionRollbacked = true;

		ConstVariantsSet firstChanges;

		//szybkie cofanie edycji - tylko pierwotna jest przywracana
		for(auto it = modyfications.begin(); it != modyfications.end(); ++it){
			if((*it).modyfication == IDataManagerReader::UPDATE_OBJECT && firstChanges.find((*it).currentValue) == firstChanges.end())
			{
				firstChanges.insert((*it).currentValue);
				mdm->rawUpdateData((*it).currentValue, (*it).previousValue);
			}
		}

		//cofanie pozosta�ych zmian od ko�ca
		for(auto it = modyfications.rbegin(); it != modyfications.rend(); ++it){
			switch((*it).modyfication){

			case IDataManagerReader::ADD_OBJECT:
				mdm->rawRemoveData((*it).currentValue);
				break;

			case IDataManagerReader::REMOVE_OBJECT:
				mdm->rawAddData(utils::const_pointer_cast<Variant>((*it).previousValue));
				break;
			}
		}

		lock.reset();
	}

	//! \data Dane wchodz�ce pod kontrol� DM
	virtual void addData(const VariantPtr & data)
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		if(mdm->rawIsManaged(data) == true){
			rollback();
			throw std::runtime_error("Memory transaction tried to add data already managed by manager");
		}

		rawAddData(data);
	}

	//! Dane usuwane z DM
	virtual void removeData(const VariantConstPtr & data)
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		if(mdm->rawIsManaged(data) == false){
			rollback();
			throw std::runtime_error("Memory transaction tried to remove data not managed by manager");
		}

		rawRemoveData(data);
	}


    virtual const bool tryAddHierarchy( const IHierarchyItemConstPtr root, bool unpack )
    {
        throw std::exception("The method or operation is not implemented.");
    }

    virtual void addHierarchy( const IHierarchyItemConstPtr root, bool unpack )
    {
        throw std::exception("The method or operation is not implemented.");
    }


	//! \param data Aktualizowane dane
	//! \param newData Nowa warto�� danych
	virtual void updateData(const VariantConstPtr & data, const VariantConstPtr & newData)
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		if(mdm->rawIsManaged(data) == false){
			rollback();
			throw std::runtime_error("Memory transaction tried to edit data not managed by manager");
		}

		try{
			rawUpdateData(data, newData);
		}catch(...){
			rollback();
			throw;
		}
	}

	virtual const bool tryAddData(const VariantPtr & data)
	{
		if(transactionRollbacked == true || mdm->rawIsManaged(data) == true){
			return false;
		}

		try{
			rawAddData(data);
		}catch(...){
			return false;
		}

		return true;
	}

	virtual const bool tryRemoveData(const VariantConstPtr & data)
	{
		if(transactionRollbacked == true || mdm->rawIsManaged(data) == false){
			return false;
		}

		try{
			rawRemoveData(data);
		}catch(...){
			return false;
		}

		return true;
	}

	virtual const bool tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
	{
		if(transactionRollbacked == true || mdm->rawIsManaged(data) == false){
			return false;
		}

		try{
			rawUpdateData(data, newData);
		}catch(...){
			return false;
		}

		return true;
	}

	virtual void getObjects(ConstVariantsList & objects) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects);
	}

	virtual void getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects, type, exact);
	}

	virtual void getObjects(VariantsCollection& objects) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects);
	}

	virtual const bool isManaged(const VariantConstPtr & object) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		return mdm->rawIsManaged(object);
	}

	virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		return mdm->rawHasObject(type, exact);
	}

private:

	void rawAddData(const VariantPtr & data)
	{
		//dodajemy dane do dm
		mdm->rawAddData(data);
		//aktualizujemy liste zmian
		ObjectChange change;
		change.currentValue = data;
		change.modyfication = IDataManagerReader::ADD_OBJECT;
		change.type = data->data()->getTypeInfo();
		modyfications.push_back(change);
	}

	void rawRemoveData(const VariantConstPtr & data)
	{
		//dodajemy dane do dm
		mdm->rawRemoveData(data);
		//aktualizujemy liste zmian
		ObjectChange change;
		change.previousValue = data;
		change.modyfication = IDataManagerReader::REMOVE_OBJECT;
		change.type = data->data()->getTypeInfo();
		modyfications.push_back(change);
	}

	void rawUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
	{
		ObjectChange change;
		change.previousValue = data->clone();
		//dodajemy dane do dm
		mdm->rawUpdateData(data, newData);
		//aktualizujemy liste zmian
		change.currentValue = data;
		change.modyfication = IDataManagerReader::UPDATE_OBJECT;
		change.type = data->data()->getTypeInfo();
		modyfications.push_back(change);
	}
};

class MemoryDataManager::MemoryReaderTransaction : public IDataManagerReaderOperations
{
public:
	MemoryReaderTransaction(MemoryDataManager * mdm) : mdm(mdm)
	{
		mdm->sync.lock();
	}

	~MemoryReaderTransaction()
	{
		mdm->sync.unlock();
	}

public:

	virtual void getObjects(ConstVariantsList & objects) const
	{
		mdm->rawGetObjects(objects);
	}

	virtual void getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
	{
		mdm->rawGetObjects(objects, type, exact);
	}

	virtual void getObjects(VariantsCollection& objects) const
	{
		mdm->rawGetObjects(objects);
	}

	virtual const bool isManaged(const VariantConstPtr & object) const
	{
		return mdm->rawIsManaged(object);
	}

	virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const
	{
		return mdm->rawHasObject(type, exact);
	}


private:
	MemoryDataManager * mdm;
};

MemoryDataManager::MemoryDataManager()
{

}

MemoryDataManager::~MemoryDataManager()
{
	ObjectsByTypes().swap(objectsByTypes);
}

void MemoryDataManager::addObserver(const ObjectObserverPtr & objectWatcher)
{
	ScopedLock lock(sync);
	if(std::find(observers.begin(), observers.end(), objectWatcher) != observers.end()){
		throw std::runtime_error("Watcher already registered");
	}

	observers.push_back(objectWatcher);
}

void MemoryDataManager::removeObserver(const ObjectObserverPtr & objectWatcher)
{
	ScopedLock lock(sync);
	auto it = std::find(observers.begin(), observers.end(), objectWatcher);
	if(it == observers.end()){
		throw std::runtime_error("Watcher not registered");
	}

	observers.erase(it);
}

void MemoryDataManager::getObjects(ConstVariantsList & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects);
}

void MemoryDataManager::getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects, type, exact);
}

void MemoryDataManager::getObjects(VariantsCollection& objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects);
}

const bool MemoryDataManager::isManaged(const VariantConstPtr & object) const
{
	ScopedLock lock(sync);
	return rawIsManaged(object);
}

void MemoryDataManager::addData(const VariantPtr & data)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == true){
		throw std::runtime_error("Object already managed by manager");
	}

	rawAddData(data);
	ChangeList changes;
	ObjectChange change;
	change.currentValue = data;
	change.modyfication = IDataManagerReader::ADD_OBJECT;
	change.type = data->data()->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

void MemoryDataManager::removeData(const VariantConstPtr & data)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == false){
		throw std::runtime_error("Object not managed by manager");
	}

	rawRemoveData(data);

	ChangeList changes;
	ObjectChange change;
	change.previousValue = data;
	change.modyfication = IDataManagerReader::REMOVE_OBJECT;
	change.type = data->data()->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

void MemoryDataManager::updateData(const VariantConstPtr & data, const VariantConstPtr & newData)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == false){
		throw std::runtime_error("Object not managed by manager");
	}

	ChangeList changes;
	ObjectChange change;
	change.previousValue = data->clone();

	rawUpdateData(data, newData);

	change.currentValue = data;

	change.modyfication = IDataManagerReader::UPDATE_OBJECT;
	change.type = data->data()->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

const bool MemoryDataManager::tryAddData(const VariantPtr & data)
{
	bool ret = true;

	try{
		addData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool MemoryDataManager::tryRemoveData(const VariantConstPtr & data)
{
	bool ret = true;

	try{
		removeData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool MemoryDataManager::tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
{
	bool ret = true;

	try{
		updateData(data, newData);
	}catch(...){
		ret = false;
	}

	return ret;
}

IMemoryDataManager::TransactionPtr MemoryDataManager::transaction()
{	
	return IMemoryDataManager::TransactionPtr(new MemoryTransaction(this));
}

IDataManagerReader::TransactionPtr MemoryDataManager::transaction() const
{	
	return IDataManagerReader::TransactionPtr(new MemoryReaderTransaction(const_cast<MemoryDataManager*>(this)));
}



void MemoryDataManager::rawGetObjects(ConstVariantsList & objects) const
{
	for(auto it = objectsByTypes.begin(); it != objectsByTypes.end(); ++it){
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	}
}

void MemoryDataManager::rawGetObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
{
	utils::TypeInfoSet types;
	requestedTypes(type, exact, types);

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end()){
			objects.insert(objects.end(), dataIT->second.begin(), dataIT->second.end());
		}
	}
}

void MemoryDataManager::rawGetObjects(VariantsCollection& objects) const
{
	ConstVariantsList locObjects;
	rawGetObjects(locObjects, objects.getTypeInfo(), objects.exactTypes());
	objects.nonCheckInsert(objects.end(), locObjects.begin(), locObjects.end());
}

void MemoryDataManager::rawAddData(const VariantPtr & data)
{
	objectsByTypes[data->data()->getTypeInfo()].insert(data);
}

void MemoryDataManager::rawRemoveData(const VariantConstPtr & data)
{
	auto it = objectsByTypes.find(data->data()->getTypeInfo());
	if (it != objectsByTypes.end()){
		it->second.erase(utils::const_pointer_cast<Variant>(data));
		if (it->second.empty() == true){
			objectsByTypes.erase(it);
		}
	}
}

void MemoryDataManager::rawUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData)
{
	//TODO
	//assign value - clone?

	auto d = utils::const_pointer_cast<Variant>(data);

	d->copyData(*newData);
	d->copyMetadata(*newData);
}

const bool MemoryDataManager::rawIsManaged(const VariantConstPtr & object) const
{
	auto type = object->data()->getTypeInfo();
	if(getDataHierarchyManager()->isRegistered(type) == false){
		throw std::runtime_error("Type not registered");
	}

	auto it = objectsByTypes.find(type);
	if(it != objectsByTypes.end() && std::find(it->second.begin(), it->second.end(), object) != it->second.end()){
		return true;
	}

	return false;
}

void MemoryDataManager::updateObservers(const ChangeList & changes )
{
	for(auto it = observers.begin(); it != observers.end(); ++it){
		try{
			(*it)->observe(changes);
		}catch(...){
			//TODO
			//rozwin�� obserwator�w aby si� jako� identyfikowali!! ewentualnie robi� to przez w�asn� implementacj� dostarczan� konretnym obiektom
			//(osobne interfejsy reader�w dla ka�dego elemnentu �adowanego do aplikacji - service, source, datasink, itp)
			CORE_LOG_WARNING("Error while updating memory data manager observer");
		}
	}
}

const bool MemoryDataManager::hasObject(const utils::TypeInfo & type, bool exact) const
{
	ScopedLock lock(sync);
	return rawHasObject(type, exact);
};


const bool MemoryDataManager::rawHasObject(const utils::TypeInfo & type, bool exact) const
{
	bool ret = false;

	utils::TypeInfoSet types;
	requestedTypes(type, exact, types);

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end() && dataIT->second.empty() == false){
			ret = true;
			break;
		}
	}

	return ret;
}

void MemoryDataManager::requestedTypes(const utils::TypeInfo & type, bool exact, utils::TypeInfoSet & types)
{
	types.insert(type);

	if(exact == false){		
		getDataHierarchyManager()->getTypeDerrivedTypes(type, types);
	}
}

