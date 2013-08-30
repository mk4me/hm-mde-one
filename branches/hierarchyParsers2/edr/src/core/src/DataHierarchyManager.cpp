#include "CorePCH.h"
#include "DataHierarchyManager.h"
#include "ApplicationCommon.h"
#include <utils/Debug.h>

namespace core {

ObjectWrapperPtr DataHierarchyManager::createWrapper(const TypeInfo & typeInfo) const
{
	auto it = typesHierarchy.find(typeInfo);
	if(it != typesHierarchy.end() && it->second.prototype != nullptr) {
		return it->second.prototype->create();
	}

	return ObjectWrapperPtr();
}

void DataHierarchyManager::getRegisteredTypes(TypeInfoSet & registeredTypes) const
{
	for(auto it = typesHierarchy.begin(); it != typesHierarchy.end(); ++it){
		if(it->second.prototype != nullptr){
			registeredTypes.insert(it->first);
		}
	}
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
	auto baseIT = typesHierarchy.find(base);
	auto derrivedIT = typesHierarchy.find(derrived);

	if(baseIT == typesHierarchy.end() || derrivedIT == typesHierarchy.end() || baseIT->second.prototype == nullptr || derrivedIT->second.prototype == nullptr){
		return false;
	}

	return derrivedIT->second.baseTypes.find(base) != derrivedIT->second.baseTypes.end();
}

void DataHierarchyManager::getTypeBaseTypes(const TypeInfo & type, TypeInfoSet & baseTypes) const
{
	auto it = typesHierarchy.find(type);
	if(it != typesHierarchy.end() && it->second.prototype != nullptr) {
		baseTypes.insert(it->second.baseTypes.begin(), it->second.baseTypes.end());
	}
}

void DataHierarchyManager::getTypeDerrivedTypes(const TypeInfo & type, TypeInfoSet & derrivedTypes) const
{
	auto it = typesHierarchy.find(type);
	if(it != typesHierarchy.end() && it->second.prototype != nullptr) {
		derrivedTypes.insert(it->second.derrivedTypes.begin(), it->second.derrivedTypes.end());
	}
}

void DataHierarchyManager::registerObjectWrapperPrototype(const ObjectWrapperPtr & owp)
{
	UTILS_ASSERT((owp->getRawPtr() == nullptr), "Obiekt powinien byc pusty jako prototyp");
	UTILS_ASSERT((owp->initializer().empty() == true), "Obiekt powinien nie zawiera� inicjalizatora - jest prototypem");
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
			//rejestrujemy typ pochodny przed typem bazowym - nieco niebezpieczne bo nie wiadomo czy typ bazowy b�dzie potem zarejestrowany
			TypeHierarchy thBase;
			thBase.derrivedTypes.insert(typeInfo);
			typesHierarchy.insert(TypesHierarchy::value_type(*it, thBase));
			CORE_LOG_WARNING("Registering derrived type before base type. Derrived type name: " << typeInfo.name() << ", base type name: " << (*it).name());
		}else{
			thIT->second.derrivedTypes.insert(typeInfo);
		}
	}
}

}
