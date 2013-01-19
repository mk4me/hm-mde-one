#include "CorePCH.h"
#include "DataHierarchyManager.h"
#include <utils/Debug.h>

namespace core {

ObjectWrapperPtr DataHierarchyManager::createWrapper(const TypeInfo & typeInfo) const
{
	return findOrThrow(typeInfo).prototype->create();
}

const TypeInfoSet DataHierarchyManager::getRegisteredTypes() const
{
	TypeInfoSet ret;

	for(auto it = typesHierarchy.begin(); it != typesHierarchy.end(); ++it){
		if(it->second.prototype != nullptr){
			ret.insert(it->first);
		}
	}

	return ret;
}

const bool DataHierarchyManager::isRegistered(const core::TypeInfo & typeInfo) const
{
	auto it = typesHierarchy.find(typeInfo);
	if(it == typesHierarchy.end() || it->second.prototype == nullptr) {
		return false;
	}

	return true;
}

const bool DataHierarchyManager::isTypeCompatible(const TypeInfo & base, const TypeInfo & derrived) const
{
	const auto & baseSet = findOrThrow(base);
	const auto & derrivedSet = findOrThrow(derrived);
	return derrivedSet.baseTypes.find(base) != derrivedSet.baseTypes.end();
}

const TypeInfoSet DataHierarchyManager::getTypeBaseTypes(const TypeInfo & type) const
{
	return findOrThrow(type).baseTypes;
}

const TypeInfoSet DataHierarchyManager::getTypeDerrivedTypes(const TypeInfo & type) const
{
	return findOrThrow(type).derrivedTypes;
}

void DataHierarchyManager::registerObjectWrapperPrototype(const ObjectWrapperPtr & owp)
{
	UTILS_ASSERT((owp->getRawPtr() == nullptr), "Obiekt powinien byc pusty jako prototyp");
	UTILS_ASSERT((owp->initializer().empty() == true), "Obiekt powinien nie zawieraæ inicjalizatora - jest prototypem");
	const TypeInfo & typeInfo = owp->getTypeInfo();
	auto it = typesHierarchy.find(typeInfo);
	if(it == typesHierarchy.end()){
		TypeHierarchy th;
		th.prototype = owp->create();
		it = typesHierarchy.insert(TypesHierarchy::value_type(typeInfo, th)).first;
	}else if(it->second.prototype != nullptr) {
		throw std::runtime_error("Type already registered");
	}else{
		it->second.prototype = owp->create();
	}

	//pobieramy typy bazowe
	TypeInfoList baseTypes;
	it->second.prototype->getSupportedTypes(baseTypes);
	baseTypes.remove(typeInfo);
	//inicjujemy typy bazowe
	it->second.baseTypes.insert(baseTypes.begin(), baseTypes.end());
	//aktualizujemy typy bazowe
	for(auto it = baseTypes.begin(); it != baseTypes.end(); ++it){
		auto thIT = typesHierarchy.find(*it);
		if(thIT == typesHierarchy.end()){
			//rejestrujemy typ pochodny przed typem bazowym - nieco niebezpieczne bo nie wiadomo czy typ bazowy bêdzie potem zarejestrowany
			TypeHierarchy thBase;
			thBase.derrivedTypes.insert(typeInfo);
			typesHierarchy.insert(TypesHierarchy::value_type(*it, thBase));
			CORE_LOG_WARNING("Registering derrived type before base type. Derrived type name: " << typeInfo.name() << ", base type name: " << (*it).name());
		}else{
			thIT->second.derrivedTypes.insert(typeInfo);
		}
	}
}

const DataHierarchyManager::TypeHierarchy & DataHierarchyManager::findOrThrow(const TypeInfo & typeInfo) const
{
	auto it = typesHierarchy.find(typeInfo);
	if(it == typesHierarchy.end() || it->second.prototype == nullptr) {
		throw std::runtime_error("Type not registered");
	}

	return it->second;
}

}