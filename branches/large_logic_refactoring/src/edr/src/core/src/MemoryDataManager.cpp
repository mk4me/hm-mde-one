#include "CorePCH.h"
#include "MemoryDataManager.h"
#include "DataHierarchyManager.h"

using namespace core;

class MemoryDataManager::MemoryTransaction : public plugin::IMemoryDataManagerOperations
{
private:
	MemoryDataManager * mdm;

public:
	MemoryTransaction(MemoryDataManager * mdm) : mdm(mdm), transactionFailed(false)
	{
		mdm->sync.lock();
	}

	~MemoryTransaction()
	{
		commit();
		mdm->sync.unlock();
	}

public:

	//! \data Dane wchodz¹ce pod kontrolê DM
	virtual void addData(const ObjectWrapperPtr & data)
	{
		if(transactionFailed == true){
			throw std::runtime_error("Memory transaction already failed and rolled-back");
		}

		if(mdm->rawIsManaged(data) == true){
			rollback();
			throw std::runtime_error("Memory transaction tried to add data already managed by manager");
		}

		try{
			rawAddData(data);
		}catch(...){
			rollback();
			throw;
		}
	}

	//! Dane usuwane z DM
	virtual void removeData(const ObjectWrapperConstPtr & data)
	{
		if(transactionFailed == true){
			throw std::runtime_error("Memory transaction already failed and rolled-back");
		}

		if(mdm->rawIsManaged(data) == false){
			rollback();
			throw std::runtime_error("Memory transaction tried to remove data not managed by manager");
		}

		try{
			rawRemoveData(data);
		}catch(...){
			rollback();
			throw;
		}
	}

	//! \param data Aktualizowane dane
	//! \param newData Nowa wartoœæ danych
	virtual void updateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
	{
		if(transactionFailed == true){
			throw std::runtime_error("Memory transaction already failed and rolled-back");
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
		if(transactionFailed == true || mdm->rawIsManaged(data) == true){
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
		if(transactionFailed == true || mdm->rawIsManaged(data) == false){
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
		if(transactionFailed == true || mdm->rawIsManaged(data) == false){
			return false;
		}

		try{
			rawUpdateData(data, newData);
		}catch(...){
			return false;
		}

		return true;
	}

private:

	void rawAddData(const ObjectWrapperPtr & data)
	{
		//dodajemy dane do dm
		mdm->rawAddData(data);
		//aktualizujemy liste zmian
		ObjectChange change;
		change.currentVal = data;
		change.modyfication = plugin::IDataManagerReader::ADD_OBJECT;
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
		change.modyfication = plugin::IDataManagerReader::REMOVE_OBJECT;
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
		change.currentVal = data;
		change.modyfication = plugin::IDataManagerReader::UPDATE_OBJECT;
		change.type = data->getTypeInfo();
		modyfications.push_back(change);
	}

	void rollback()
	{
		transactionFailed = true;

		std::set<ObjectWrapperConstPtr> firstChanges;
		
		//szybkie cofanie edycji - tylko pierwotna jest przywracana
		for(auto it = modyfications.begin(); it != modyfications.end(); ++it){
			if((*it).modyfication == plugin::IDataManagerReader::UPDATE_OBJECT && firstChanges.find((*it).currentVal) == firstChanges.end())
			{
				firstChanges.insert((*it).currentVal);
				mdm->rawUpdateData((*it).currentVal, (*it).previousValue);
			}
		}

		//cofanie pozosta³ych zmian od koñca
		for(auto it = modyfications.rbegin(); it != modyfications.rend(); ++it){
			switch((*it).modyfication){

			case plugin::IDataManagerReader::ADD_OBJECT:
				mdm->rawRemoveData((*it).currentVal);
				break;

			case plugin::IDataManagerReader::REMOVE_OBJECT:
				mdm->rawAddData(core::const_pointer_cast<ObjectWrapper>((*it).previousValue));
				break;
			}
		}
	}

	void commit()
	{
		if(transactionFailed == false){
			mdm->updateObservers(modyfications);
		}
	}

private:
	ChangeList modyfications;
	bool transactionFailed;
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

void MemoryDataManager::getObjects(ConstObjectsList & objects, const TypeInfo & type, bool exact)
{
	ScopedLock lock(sync);
	TypeInfoSet types;
	types.insert(type);

	if(exact == false){
		const auto & derrivedTypes = getDataHierarchyManager()->getTypeDerrivedTypes(type);
		types.insert(derrivedTypes.begin(), derrivedTypes.end());
	}

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end()){
			objects.insert(objects.end(), dataIT->second.begin(), dataIT->second.end());
		}
	}
}

void MemoryDataManager::getObjects(ObjectWrapperCollection& objects)
{
	ConstObjectsList locObjects;
	getObjects(locObjects, objects.getTypeInfo(), objects.exactTypes());
	objects.nonCheckInsert(objects.end(), locObjects.begin(), locObjects.end());
}

const bool MemoryDataManager::isManaged(const ObjectWrapperConstPtr & object) const
{
	ScopedLock lock(sync);
	return rawIsManaged(object);
}

void MemoryDataManager::addData(const ObjectWrapperPtr & data)
{
	ScopedLock lock(sync);
	rawAddData(data);
	ChangeList changes;
	ObjectChange change;
	change.currentVal = data;
	change.modyfication = plugin::IDataManagerReader::ADD_OBJECT;
	change.type = data->getTypeInfo();
	changes.push_back(change);
	updateObservers(changes);
}

void MemoryDataManager::removeData(const ObjectWrapperConstPtr & data)
{
	ScopedLock lock(sync);
	rawRemoveData(data);
}

void MemoryDataManager::updateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
{
	ScopedLock lock(sync);
	rawUpdateData(data, newData);
}

const MemoryDataManager::TransactionPtr MemoryDataManager::transaction()
{
	//TODO
	return MemoryDataManager::TransactionPtr();
}

void MemoryDataManager::rawAddData(const ObjectWrapperPtr & data)
{
	if(rawIsManaged(data) == true){
		throw std::runtime_error("Object already managed by manager");
	}

	objectsByTypes[data->getTypeInfo()].insert(data);
}

void MemoryDataManager::rawRemoveData(const ObjectWrapperConstPtr & data)
{
	if(rawIsManaged(data) == false){
		throw std::runtime_error("Object not managed by manager");
	}

	objectsByTypes[data->getTypeInfo()].erase(const_pointer_cast<ObjectWrapper>(data));
}

void MemoryDataManager::rawUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData)
{
	if(rawIsManaged(data) == false){
		throw std::runtime_error("Object not managed by manager");
	}

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