#include "CommunicationPCH.h"
#include "HMDBSourceContext.h"
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <hmdbserviceslib/DateTimeUtils.h>
#include "HMDBSourceContextOperations.h"
#include <boost/lexical_cast.hpp>

using namespace hmdbCommunication;

HMDBSourceContext::HMDBSourceContext(IHMDBDataContext * dataContext,
	IHMDBLocalContext * localContext,
	IHMDBRemoteContext * remoteContext)
	: dataContext_(dataContext), localContext_(localContext), remoteContext_(remoteContext)
{

}

HMDBSourceContext::~HMDBSourceContext()
{
	
}

IHMDBLocalContext * HMDBSourceContext::localContext()
{
	return localContext_.get();
}

const IHMDBLocalContext * HMDBSourceContext::localContext() const
{
	return localContext_.get();
}

IHMDBRemoteContext * HMDBSourceContext::remoteContext()
{
	return remoteContext_.get();
}

const IHMDBRemoteContext * HMDBSourceContext::remoteContext() const
{
	return remoteContext_.get();
}

IHMDBDataContext * HMDBSourceContext::dataContext()
{
	return dataContext_.get();
}

const IHMDBDataContext * HMDBSourceContext::dataContext() const
{
	return dataContext_.get();
}