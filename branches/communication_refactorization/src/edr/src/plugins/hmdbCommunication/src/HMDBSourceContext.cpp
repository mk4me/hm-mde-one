#include "CommunicationPCH.h"
#include "HMDBSourceContext.h"
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
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
	delete localContext_;
	delete remoteContext_;
}

IHMDBLocalContext * HMDBSourceContext::localContext()
{
	return localContext_;
}

const IHMDBLocalContext * HMDBSourceContext::localContext() const
{
	return localContext_;
}

IHMDBRemoteContext * HMDBSourceContext::remoteContext()
{
	return remoteContext_;
}

const IHMDBRemoteContext * HMDBSourceContext::remoteContext() const
{
	return remoteContext_;
}

IHMDBDataContext * HMDBSourceContext::dataContext()
{
	return dataContext_;
}

const IHMDBDataContext * HMDBSourceContext::dataContext() const
{
	return dataContext_;
}