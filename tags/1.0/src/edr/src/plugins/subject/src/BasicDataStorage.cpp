#include "SubjectPCH.h"
#include "BasicDataStorage.h"
#include <corelib/PluginCommon.h>
#include <corelib/IDataHierarchyManagerReader.h>

BasicDataStorage::BasicDataStorage()
{

}

BasicDataStorage::~BasicDataStorage()
{

}

void BasicDataStorage::getObjects(core::ConstObjectsList & objects) const
{
	ScopedLock lock(sync);
	for(auto it = objectsByTypes.begin(); it != objectsByTypes.end(); ++it){
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	}
}

void BasicDataStorage::getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact) const
{
	core::TypeInfoSet types;
	requestedTypes(type, exact, types);

	ScopedLock lock(sync);

	for(auto it = types.begin(); it != types.end(); ++it)
	{
		auto dataIT = objectsByTypes.find(*it);
		if(dataIT != objectsByTypes.end()){
			objects.insert(objects.end(), dataIT->second.begin(), dataIT->second.end());
		}
	}
}

void BasicDataStorage::getObjects(core::ObjectWrapperCollection& objects) const
{
	core::ConstObjectsList locObjects;
	getObjects(locObjects, objects.getTypeInfo(), objects.exactTypes());	
	objects.insert(objects.end(), locObjects.begin(), locObjects.end());
}

const bool BasicDataStorage::isManaged(const core::ObjectWrapperConstPtr & object) const
{
	if(plugin::getDataHierachyManagerReader()->isRegistered(object->getTypeInfo()) == false){
		throw std::runtime_error("Type not registered");
	}

	ScopedLock lock(sync);

	auto it = objectsByTypes.find(object->getTypeInfo());
	if(it != objectsByTypes.end() && std::find(it->second.begin(), it->second.end(), object) != it->second.end()){
		return true;
	}

	return false;
}

const bool BasicDataStorage::hasObject(const core::TypeInfo & type, bool exact) const
{
	bool ret = false;

	core::TypeInfoSet types;
	requestedTypes(type, exact, types);

	ScopedLock lock(sync);

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

void BasicDataStorage::addData(const core::ObjectWrapperConstPtr & data)
{
	if(plugin::getDataHierachyManagerReader()->isRegistered(data->getTypeInfo()) == false){
		throw std::runtime_error("Type not registered");
	}

	ScopedLock lock(sync);

	auto found = find(data);
	if(found.first != objectsByTypes.end() && found.second != found.first->second.end()){
		throw std::runtime_error("Object already managed");
	}

	if(found.first != objectsByTypes.end()){
		found.first->second.insert(data);
	}else{
		objectsByTypes[data->getTypeInfo()].insert(data);
	}
}

void BasicDataStorage::removeData(const core::ObjectWrapperConstPtr & data)
{
	if(plugin::getDataHierachyManagerReader()->isRegistered(data->getTypeInfo()) == false){
		throw std::runtime_error("Type not registered");
	}

	ScopedLock lock(sync);

	auto found = find(data);
	if(found.first == objectsByTypes.end() || found.second == found.first->second.end()){
		throw std::runtime_error("Object not managed");
	}

	found.first->second.erase(found.second);

	if(found.first->second.empty() == true){
		objectsByTypes.erase(found.first);
	}
}

const bool BasicDataStorage::tryAddData(const core::ObjectWrapperConstPtr & data)
{
	bool ret = true;
	try{
		addData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

const bool BasicDataStorage::tryRemoveData(const core::ObjectWrapperConstPtr & data)
{
	bool ret = true;
	try{
		removeData(data);
	}catch(...){
		ret = false;
	}

	return ret;
}

void BasicDataStorage::requestedTypes( const core::TypeInfo & type, bool exact, core::TypeInfoSet & types )
{
	types.insert(type);

	if(exact == false){		
		plugin::getDataHierachyManagerReader()->getTypeDerrivedTypes(type, types);
	}
}

std::pair<BasicDataStorage::ObjectsByTypes::iterator, core::ConstObjects::iterator> BasicDataStorage::find(const core::ObjectWrapperConstPtr & data)
{
	std::pair<BasicDataStorage::ObjectsByTypes::iterator, core::ConstObjects::iterator> ret;

	ret.first = objectsByTypes.find(data->getTypeInfo());
	if(ret.first != objectsByTypes.end()){
		ret.second = ret.first->second.find(data);
	}

	return ret;
}