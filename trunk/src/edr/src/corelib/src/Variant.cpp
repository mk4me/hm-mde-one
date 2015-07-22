#include "CoreLibPCH.h"
#include <corelib/Variant.h>
#include <utils/Push.h>
#include <corelib/Exceptions.h>

using namespace core;

Variant::data_t::data_t(const Variant* variant) : variant(variant)
{
	variant->sync_.lock();
}

Variant::data_t::~data_t()
{
	variant->sync_.unlock();
}

const utils::ObjectWrapperConstPtr Variant::data_t::operator->() const
{
	return variant->wrapper_;
}

VariantPtr Variant::create(const utils::ObjectWrapperPtr & wrapper)
{
	if (wrapper == nullptr){
		throw std::runtime_error("Empty data placeholder");
	}

	return VariantPtr(new Variant(wrapper));
}

VariantPtr Variant::create() const
{
	return VariantPtr(new Variant(wrapper_->create()));
}

Variant::Variant(const Variant & ow, const CloneOp co)
: initialized_(ow.initialized_), initializing_(false)
{
	auto data = ow.data();
	if (co == utils::ObjectWrapper::ShallowClone){
		wrapper_ = ow.wrapper_;
		initializer_ = ow.initializer_;
		metadata_ = ow.metadata_;
	}
	else{
		ow.wrapper_->clone(*wrapper_, co);
		if (ow.initializer_ != nullptr){
			initializer_.reset(ow.initializer_->clone());
		}

		if (ow.metadata_ != nullptr){
			metadata_.reset(new Metadata(*(ow.metadata_)));
		}
	}
}

Variant::Variant(const utils::ObjectWrapperPtr & wrapper)
: wrapper_(wrapper), initialized_(false), initializing_(false)
{
	UTILS_ASSERT((wrapper_ != nullptr), "Wrapper dla danych nie mo�e by� pusty");
}

void Variant::set(const utils::ObjectWrapperPtr & wrapper)
{
	if (wrapper == nullptr){
		throw std::invalid_argument("Empty data object");
	}

	ScopedLock lock(sync_);
	wrapper->clone(*wrapper_, utils::ObjectWrapper::ShallowClone);
	tryResetInitializer();
}

utils::ObjectWrapper::get_t Variant::get(const bool exact)
{
	innerInitialize();
	return wrapper_->get(exact);
}

const utils::ObjectWrapper::get_t Variant::get(const bool exact) const
{
	innerInitialize();
	return utils::ObjectWrapperConstPtr(wrapper_)->get(exact);
}

void * Variant::getRawPtr()
{
	innerInitialize();
	return wrapper_->getRawPtr();
}

const void * Variant::getRawPtr() const
{
	innerInitialize();
	return wrapper_->getRawPtr();
}

Variant::~Variant()
{
}

const bool Variant::getMetadata(const std::string & key, std::string & val) const
{
	ScopedLock lock(sync_);

	if (metadata_ == nullptr){
		return false;
	}

	auto it = metadata_->find(key);
	if (it == metadata_->end()){
		return false;
	}

	val = it->second;
	return true;
}

const bool Variant::existMetadata(const std::string & key) const
{
	ScopedLock lock(sync_);
	return metadata_ != nullptr && metadata_->find(key) != metadata_->end();
}

void Variant::setMetadata(const std::string & key, const std::string & val)
{
	ScopedLock lock(sync_);

	if (metadata_ == nullptr){
		metadata_.reset(new Metadata);
	}

	(*metadata_)[key] = val;
}

void Variant::removeMetadata(const std::string & key)
{
	ScopedLock lock(sync_);

	if (metadata_ == nullptr){
		return;
	}

	(*metadata_).erase(key);
}

void Variant::clearMetadata()
{
	ScopedLock lock(sync_);

	if (metadata_ == nullptr){
		return;
	}

	Metadata().swap(*metadata_);
}

void Variant::copyMetadata(const Variant & srcWrapper)
{
	ScopedLock lock(sync_);

	if (srcWrapper.metadata_ == nullptr || srcWrapper.metadata_->empty() == true){
		return;
	}

	if (metadata_ == nullptr){
		metadata_.reset(new Metadata);
	}

	metadata_->insert(srcWrapper.metadata_->begin(), srcWrapper.metadata_->end());
}

void Variant::setInitializer(const VariantInitializerPtr & initializer)
{
	ScopedLock lock(sync_);
	initializer_ = initializer;

	if (initializer_ != nullptr){
		resetData();
	}
	else{
		initialized_ = true;
	}
}

const VariantInitializerPtr Variant::initializer()
{
	ScopedLock lock(sync_);
	return initializer_;
}

const VariantInitializerConstPtr Variant::initializer() const
{
	ScopedLock lock(sync_);
	return initializer_;
}

void Variant::innerInitialize() const
{
	ScopedLock lock(sync_);
	if (initialized() == false && initializer_ != nullptr){
		utils::Push init(initializing_, true);
		initializer_->initialize(const_cast<Variant*>(this));
		initialized_ = true;
	}
}

const bool Variant::initialized() const
{
	ScopedLock lock(sync_);
	return initialized_ || (wrapper_->getRawPtr() != nullptr);
}

const VariantPtr Variant::clone(const CloneOp co) const
{
	auto ret = create();
	clone(*ret, co);
	return ret;
}

void Variant::clone(Variant & dest, const CloneOp co) const
{
	if (co == utils::ObjectWrapper::ShallowClone){
		dest.wrapper_ = wrapper_;
		dest.initializer_ = initializer_;
		dest.metadata_ = metadata_;
	}
	else{
		if (wrapper_->getRawPtr() != nullptr){
			wrapper_->clone(*(dest.wrapper_), co);
		}

		if (initializer_ != nullptr){
			dest.initializer_.reset(initializer_->clone());
		}

		if (metadata_ != nullptr){
			dest.metadata_.reset(new Metadata(*(metadata_)));
		}
	}

	dest.initialized_ = initialized_;
}

void Variant::resetData()
{
	wrapper_->reset();
	initialized_ = false;
}

void Variant::swap(Variant & ow)
{
	std::swap(initialized_, ow.initialized_);
	std::swap(initializer_, ow.initializer_);
	std::swap(metadata_, ow.metadata_);
}

const bool Variant::isEqual(const Variant & obj) const
{
	//czy zgadza si� meta - wska�niki
	return ((metadata_ == obj.metadata_) ||
		//czy zgadza si� meta - zawarto��
		((metadata_ != nullptr) && (obj.metadata_ != nullptr) && (*metadata_ == *(obj.metadata_)))) &&
		//czy zgadzaj� si� dane - wska�niki
		(((wrapper_ != nullptr) && (wrapper_ == obj.wrapper_)) ||
		// czy zgadzaj� si� dane - zawarto��
		(wrapper_->isEqual(*(obj.wrapper_)) == true) ||
		//czy maj� ten sam inicjalizator je�li brak danych - po wska�niki
		(((wrapper_->getRawPtr() == nullptr) || (obj.wrapper_->getRawPtr() == nullptr)) && (((initializer_ != nullptr) && (initializer_ == obj.initializer_)) ||
		//czy ten sam inicjalizator je�li chodzi o efekt dzia�ania
		((initializer_ != nullptr) && (obj.initializer_ != nullptr) && initializer_->isEqual(*(obj.initializer_))))));
}


void Variant::tryInitialize()
{
	innerInitialize();
}

void Variant::forceInitialize()
{
	ScopedLock lock(sync_);
	utils::Push init(initializing_, true);
	initializer_->initialize(const_cast<Variant*>(this));
	initialized_ = true;
}

const Variant::data_t Variant::data() const
{
	return data_t(this);
}

const Variant::data_t Variant::initializedData() const
{
	innerInitialize();
	return data_t(this);
}

void Variant::tryResetInitializer()
{
	if (initializing_ == false){
		initializer_.reset();
		initialized_ = true;
	}
}

void Variant::copyData(const Variant & srcWrapper)
{
	wrapper_->clone(*(srcWrapper.wrapper_));
}
