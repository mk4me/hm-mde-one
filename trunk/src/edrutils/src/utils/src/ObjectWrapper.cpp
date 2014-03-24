#include <utils/ObjectWrapper.h>

using namespace utils;

const bool ObjectWrapper::getMetadata(const std::string & key, std::string & val) const
{
	auto it = metadata_.find(key);
	if(it == metadata_.end()){
		return false;
	}

	val = it->second;
	return true;
}

const bool ObjectWrapper::existMetadata(const std::string & key) const
{
	return metadata_.find(key) != metadata_.end();
}

void ObjectWrapper::setMetadata(const std::string & key, const std::string & val)
{
	metadata_[key] = val;
}

void ObjectWrapper::removeMetadata(const std::string & key)
{
	metadata_.erase(key);
}

void ObjectWrapper::clearMetadata()
{
	Metadata().swap(metadata_);
}

void ObjectWrapper::copyMetadata(const ObjectWrapper & srcWrapper)
{
	metadata_.insert(srcWrapper.metadata_.begin(), srcWrapper.metadata_.end());
}

ObjectWrapper::get_t ObjectWrapper::get(bool exact)
{
	get_t result = { this, nullptr, exact };
	return result;
}

//! Pobiera obiekt z wrappera. W razie b³êdu rzuca bad_castem.
//! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
//! \return Return Type Resolver
const ObjectWrapper::get_t ObjectWrapper::get(bool exact) const
{
	const get_t result = { nullptr, this, exact };
	return result;
}

void ObjectWrapper::setInitializer(const LazyInitializer & initializer)
{	
	initializer_ = initializer;
	
	if(initializer_.empty() == false){
		reset();
	}else{
		initialized = true;
	}
}

ObjectWrapper::ObjectWrapper() : initialized(true)
{

}

ObjectWrapper::ObjectWrapper(const ObjectWrapper & wrapper) : initialized(true)
{

}

const bool ObjectWrapper::__tryUnpackData(void * object, const TypeInfo & ptrType)
{
	initialize();
	return __tryGetData(object, ptrType);
}


const bool ObjectWrapper::__tryUnpackData(void * object, const TypeInfo & ptrType) const
{
	initialize();
	return __tryGetData(object, ptrType);
}

const bool ObjectWrapper::__tryUnpackBaseData(void * object, const TypeInfo & ptrType)
{
	initialize();
	return __tryGetBaseData(object, ptrType);
}


const bool ObjectWrapper::__tryUnpackBaseData(void * object, const TypeInfo & ptrType) const
{
	initialize();
	return __tryGetBaseData(object, ptrType);
}

const void* ObjectWrapper::getRawPtr() const
{
	initialize();
	return __getRawPtr();
}

void* ObjectWrapper::getRawPtr()
{
	initialize();
	return __getRawPtr();
}

ObjectWrapper::~ObjectWrapper()
{

}

const bool ObjectWrapper::isSupported(const TypeInfo& type) const
{
	return type == getTypeInfo();
}

void ObjectWrapper::getSupportedTypes(Types& supported) const
{
	supported.push_back(getTypeInfo());
}

const bool ObjectWrapper::isNull() const
{
	return __getRawPtr() == nullptr && (initializer_.empty() == true || (initializer_.empty() == false && initialized == true));
}

const ObjectWrapper::LazyInitializer & ObjectWrapper::initializer() const
{
	return initializer_;
}

void ObjectWrapper::initialize() const
{
	if(initialized == false && initializer_.empty() == false){
		try{
			initializer_(*(const_cast<ObjectWrapper*>(this)));
		}catch(...){

		}
	}
}

const ObjectWrapperPtr ObjectWrapper::clone() const
{
	initialize();
	auto ret = create();
	if(getRawPtr() != nullptr){
		__clone(ret);
	}

	ret->copyMetadata(*this);
	
	return ret;
}

void ObjectWrapper::reset()
{
	__reset();
	initialized = false;
}

void ObjectWrapper::swap(ObjectWrapper & ow)
{
	__swap(ow);
	std::swap(initialized, ow.initialized);
	std::swap(initializer_, ow.initializer_);
}