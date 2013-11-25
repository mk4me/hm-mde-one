#include "CoreLibPCH.h"
#include <corelib/ObjectWrapperCollection.h>

using namespace core;

ObjectWrapperCollection::ObjectWrapperCollection(const TypeInfo & typeInfo, bool exact)
	: typeInfo(typeInfo), exact(exact)
{

}

ObjectWrapperCollection::ObjectWrapperCollection(const ObjectWrapperCollection & owc)
	: data(owc.data), typeInfo(owc.typeInfo), exact(owc.exact)
{

}

ObjectWrapperCollection::~ObjectWrapperCollection()
{

}
const TypeInfo & ObjectWrapperCollection::getTypeInfo() const
{
	return typeInfo;
}

//! \return Czy dane musza by� dok��dnie tego samego typu dla kt�rego utworzono kolekcj� czy mog� te� by� pochodne od niego
const bool ObjectWrapperCollection::exactTypes() const
{
	return exact;
}

//! \param exact Czy dane maj� by� dok�adnie tego typu dla kt�rego utworzono kolekcj�
//! \param clear Czy czy�ci� dane niezgodne z typem kolekcji - pochodne mu
void ObjectWrapperCollection::setExactTypes(bool exact)
{
	this->exact = exact;

	if(exact == true){
		removeDerivedTypes();
	}
}

ObjectWrapperCollection::iterator ObjectWrapperCollection::begin()
{
	return data.begin();
}

ObjectWrapperCollection::iterator ObjectWrapperCollection::end()
{
	return data.end();
}

ObjectWrapperCollection::const_iterator ObjectWrapperCollection::begin() const
{
	return data.begin();
}

ObjectWrapperCollection::const_iterator ObjectWrapperCollection::end() const
{
	return data.end();
}

ObjectWrapperCollection::reverse_iterator ObjectWrapperCollection::rbegin()
{
	return data.rbegin();
}

ObjectWrapperCollection::reverse_iterator ObjectWrapperCollection::rend()
{
	return data.rend();
}

ObjectWrapperCollection::const_reverse_iterator ObjectWrapperCollection::crbegin() const
{
	return data.crbegin();
}

ObjectWrapperCollection::const_reverse_iterator ObjectWrapperCollection::crend() const
{
	return data.crend();
}

const bool ObjectWrapperCollection::empty() const
{
	return data.empty();
}

const ObjectWrapperCollection::size_type ObjectWrapperCollection::size() const
{
	return data.size();
}

void ObjectWrapperCollection::push_front(const ObjectWrapperConstPtr & obj)
{
	insert(data.begin(), obj);
}

void ObjectWrapperCollection::pop_front()
{
	data.pop_front();
}

void ObjectWrapperCollection::push_back(const ObjectWrapperConstPtr & obj)
{
	insert(data.end(), obj);
}

void ObjectWrapperCollection::pop_back()
{
	data.pop_back();
}

ObjectWrapperCollection::reference ObjectWrapperCollection::front()
{
	return data.front();
}

ObjectWrapperCollection::const_reference ObjectWrapperCollection::front() const
{
	return data.front();
}

ObjectWrapperCollection::reference ObjectWrapperCollection::back()
{
	return data.back();
}

ObjectWrapperCollection::const_reference ObjectWrapperCollection::back() const
{
	return data.back();
}

ObjectWrapperCollection::iterator ObjectWrapperCollection::insert(iterator position, const ObjectWrapperConstPtr& val)
{
	// sprawdzenie poprawno�ci typu
	if (exact == true){
		if(val->getTypeInfo() != getTypeInfo()) {
			// bad_cast nie pobiera parametr�w
			//throw std::bad_cast("Type of object not equal to type of collection");
			throw std::bad_cast();
		}
	}else if(val->isSupported(typeInfo) == false){
		//throw std::bad_cast("Type of object not supported by collection");
		throw std::bad_cast();
	}

	return data.insert(position, val);
}

ObjectWrapperCollection::iterator ObjectWrapperCollection::erase(iterator position)
{
	return data.erase(position);
}

ObjectWrapperCollection::iterator ObjectWrapperCollection::erase(iterator first, iterator last)
{
	return data.erase(first, last);
}

void ObjectWrapperCollection::swap(ObjectWrapperCollection & owc)
{
	data.swap(owc.data);
	std::swap(typeInfo, owc.typeInfo);
}

void ObjectWrapperCollection::clear()
{
	data.clear();
}

void ObjectWrapperCollection::remove (const ObjectWrapperConstPtr& val)
{
	data.remove(val);
}

void ObjectWrapperCollection::unique()
{
	data.unique();
}

void ObjectWrapperCollection::sort()
{
	data.sort();
}

void ObjectWrapperCollection::reverse()
{
	data.reverse();
}

void ObjectWrapperCollection::removeDerivedTypes()
{
	//iteruj po kolekcji i usun te typy kt�re nie s� dok�adnie typu kolekcji
	auto cIT = begin();
	while( cIT != end() ) {
		if((*cIT)->getTypeInfo() != typeInfo){
			cIT = data.erase(cIT);
		}else{
			++cIT;
		}
	}
}
