#include "CoreLibPCH.h"
#include <corelib/Job.h>

using namespace core;

JobBase::JobGuard::JobGuard(utils::shared_ptr<Description> description)
{
	if (JobBase::tlsJD.get() == nullptr){
		JobBase::tlsJD.reset(new utils::shared_ptr<Description>(description));
	}
	else{
		(*JobBase::tlsJD) = description;
	}
}

JobBase::JobGuard::~JobGuard()
{
	(*(*JobBase::tlsJD)) = Description({ "No explicit owner", "No explicit name" });
}

JobBase::JobBase()
{

}

JobBase::JobBase(utils::shared_ptr<Description> description)
	: description(description)
{

}

JobBase::JobBase(JobBase && Other) : description(std::move(Other.description))
{

}

JobBase::~JobBase()
{

}

JobBase& JobBase::operator=(JobBase&& Other) NOEXCEPT
{
	description = std::move(Other.description);
	return *this;
}

void JobBase::swap(JobBase & Other) NOEXCEPT
{
	std::swap(description, Other.description);
}

const std::string JobBase::owner() const
{
	return description != nullptr ? description->owner : "No explicit owner";
}

const std::string JobBase::name() const
{
	return description != nullptr ? description->name : "No explicit name";
}

const std::string JobBase::currentOwner()
{
	return (*tlsJD)->owner;
}

const std::string JobBase::currentName()
{
	return (*tlsJD)->name;
}

JobBase::TLS JobBase::tlsJD;	