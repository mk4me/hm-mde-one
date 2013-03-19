#include "CorePCH.h"
#include "MemoryDataManager.h"
#include "DataHierarchyManager.h"

using namespace core;

class MemoryDataManager::MemoryTransaction : public IMemoryDataManager::IMemoryDataTransaction
{
private:
	MemoryDataManager * mdm;
	MemoryDataManager::ChangeList modyfications;
	bool transactionRollbacked;

public:
	MemoryTransaction(MemoryDataManager * mdm) : mdm(mdm), transactionRollbacked(false)
	{
		mdm->sync.lock();
	}

	~MemoryTransaction()
	{
		if(transactionRollbacked == false){
			if(modyfications.empty() == false){
				mdm->updateObservers(modyfications);
			}
			mdm->sync.unlock();
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

		std::set<ObjectWrapperConstPtr> firstChanges;

		//szybkie cofanie edycji - tylko pierwotna jest przywracana
		for(auto it = modyfications.begin(); it != modyfications.end(); ++it){
			if((*it).modyfication == IDataManagerReader::UPDATE_OBJECT && firstChanges.find((*it).currentValue) == firstChanges.end())
			{
				firstChanges.insert((*it).currentValue);
				mdm->rawUpdateData((*it).currentValue, (*it).previousValue);
			}
		}

		//cofanie pozosta³ych zmian od koñca
		for(auto it = modyfications.rbegin(); it != modyfications.rend(); ++it){
			switch((*it).modyfication){

			case IDataManagerReader::ADD_OBJECT:
				mdm->rawRemoveData((*it).currentValue);
				break;

			case IDataManagerReader::REMOVE_OBJECT:
				mdm->rawAddData(core::const_pointer_cast<ObjectWrapper>((*it).previousValue));
				break;
			}
		}

		mdm->sync.unlock();
	}

	//! \data Dane wchodz¹ce pod kontrolê DM
	virtual void addData(const ObjectWrapperPtr & data)
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
	virtual void removeData(const ObjectWrapperConstPtr & data)
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

	//! \param data Aktualizowane dane
	//! \param newData Nowa wartoœæ danych
	virtual void updateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
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

	virtual const bool tryAddData(const ObjectWrapperPtr & data)
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

	virtual const bool tryRemoveData(const ObjectWrapperConstPtr & data)
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

	virtual const bool tryUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
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

	virtual void getObjects(core::ConstObjectsList & objects) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects);
	}

	virtual void getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects, type, exact);
	}

	virtual void getObjects(core::ObjectWrapperCollection& objects) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		mdm->rawGetObjects(objects);
	}

	virtual const bool isManaged(const core::ObjectWrapperConstPtr & object) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		return mdm->rawIsManaged(object);
	}

	virtual const bool hasObject(const TypeInfo & type, bool exact) const
	{
		if(transactionRollbacked == true){
			throw std::runtime_error("Memory transaction rolled-back");
		}

		return mdm->rawHasObject(type, exact);
	}

private:

	void rawAddData(const ObjectWrapperPtr & data)
	{
		//dodajemy dane do dm
		mdm->rawAddData(data);
		//aktualizujemy liste zmian
		ObjectChange change;
		change.currentValue = data;
		change.modyfication = IDataManagerReader::ADD_OBJECT;
		change.type = data->getTypeInfo();
		modyfications.push_back(change);
	}

	void rawRemoveData(const ObjectWrapperConstPtr & data)
	{
		//dodajemy dane do dm
		mdm->rawRemoveData(data);
		//aktualizujemy liste zmian
		ObjectChange change;
		change.previousValue = data;
		change.modyfication = IDataManagerReader::REMOVE_OBJECT;
		change.type = data->getTypeInfo();
		modyfications.push_back(change);
	}

	void rawUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
	{
		ObjectChange change;
		change.previousValue = data->clone();
		//dodajemy dane do dm
		mdm->rawUpdateData(data, newData);
		//aktualizujemy liste zmian
		change.currentValue = data;
		change.modyfication = IDataManagerReader::UPDATE_OBJECT;
		change.type = data->getTypeInfo();
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

	virtual void getObjects(core::ConstObjectsList & objects) const
	{
		mdm->rawGetObjects(objects);
	}

	virtual void getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact) const
	{
		mdm->rawGetObjects(objects, type, exact);
	}

	virtual void getObjects(core::ObjectWrapperCollection& objects) const
	{
		mdm->rawGetObjects(objects);
	}

	virtual const bool isManaged(const core::ObjectWrapperConstPtr & object) const
	{
		return mdm->rawIsManaged(object);
	}

	virtual const bool hasObject(const TypeInfo & type, bool exact) const
	{
		return mdm->rawHasObject(type, exact);
	}


private:
	MemoryDataManager * mdm;
};

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

void MemoryDataManager::getObjects(core::ConstObjectsList & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects);
}

void MemoryDataManager::getObjects(ConstObjectsList & objects, const TypeInfo & type, bool exact) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects, type, exact);
}

void MemoryDataManager::getObjects(ObjectWrapperCollection& objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(objects);
}

const bool MemoryDataManager::isManaged(const ObjectWrapperConstPtr & object) const
{
	ScopedLock lock(sync);
	return rawIsManaged(object);
}

void MemoryDataManager::addData(const ObjectWrapperPtr & data)
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
	change.type = data->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

void MemoryDataManager::removeData(const ObjectWrapperConstPtr & data)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == false){
		throw std::runtime_error("Object not managed by manager");
	}

	rawRemoveData(data);
}

void MemoryDataManager::updateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
{
	ScopedLock lock(sync);

	if(rawIsManaged(data) == false){
		throw std::runtime_error("Object not managed by manager");
	}

	rawUpdateData(data, newData);
}

const bool MemoryDataManager::tryAddData(const ObjectWrapperPtr & data)
{
	bool ret = true;

	try{
		addData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool MemoryDataManager::tryRemoveData(const ObjectWrapperConstPtr & data)
{
	bool ret = true;

	try{
		removeData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool MemoryDataManager::tryUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
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

void MemoryDataManager::rawGetObjects(core::ConstObjectsList & objects) const
{
	for(auto it = objectsByTypes.begin(); it != objectsByTypes.end(); ++it){
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	}
}

void MemoryDataManager::rawGetObjects(ConstObjectsList & objects, const TypeInfo & type, bool exact) const
{
	TypeInfoSet types;
	requestedTypes(type, exact, types);

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end()){
			objects.insert(objects.end(), dataIT->second.begin(), dataIT->second.end());
		}
	}
}

void MemoryDataManager::rawGetObjects(ObjectWrapperCollection& objects) const
{
	ConstObjectsList locObjects;
	rawGetObjects(locObjects, objects.getTypeInfo(), objects.exactTypes());
	objects.nonCheckInsert(objects.end(), locObjects.begin(), locObjects.end());
}

void MemoryDataManager::rawAddData(const ObjectWrapperPtr & data)
{
	objectsByTypes[data->getTypeInfo()].insert(data);
}

void MemoryDataManager::rawRemoveData(const ObjectWrapperConstPtr & data)
{
	objectsByTypes[data->getTypeInfo()].erase(const_pointer_cast<ObjectWrapper>(data));
}

void MemoryDataManager::rawUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
{
	//TODO
	//assign value - clone?
}

const bool MemoryDataManager::rawIsManaged(const ObjectWrapperConstPtr & object) const
{
	if(getDataHierarchyManager()->isRegistered(object->getTypeInfo()) == false){
		throw std::runtime_error("Type not registered");
	}

	auto it = objectsByTypes.find(object->getTypeInfo());
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
			//rozwin¹æ obserwatorów aby siê jakoœ identyfikowali!! ewentualnie robiæ to przez w³asn¹ implementacjê dostarczan¹ konretnym obiektom
			//(osobne interfejsy readerów dla ka¿dego elemnentu ³adowanego do aplikacji - service, source, datasink, itp)
			CORE_LOG_WARNING("Error while updating memory data manager observer");
		}
	}
}

const bool MemoryDataManager::hasObject(const TypeInfo & type, bool exact) const
{
	ScopedLock lock(sync);
	return rawHasObject(type, exact);
};


const bool MemoryDataManager::rawHasObject(const TypeInfo & type, bool exact) const
{
	bool ret = false;

	TypeInfoSet types;
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

void MemoryDataManager::requestedTypes( const TypeInfo & type, bool exact, TypeInfoSet & types )
{
	types.insert(type);

	if(exact == false){		
		getDataHierarchyManager()->getTypeDerrivedTypes(type, types);
	}
}
