#include "CommunicationPCH.h"
#include "HMDBSourceContext.h"
#include <plugins/medusaDatabase/IHMDBSession.h>
#include <plugins/medusaDatabase/IHMDBStorage.h>
#include <plugins/medusaDatabase/IHMDBDataContext.h>
#include <plugins/medusaDatabase/IHMDBLocalContext.h>
#include <plugins/medusaDatabase/IHMDBRemoteContext.h>
#include <hmdbserviceslib/DateTimeUtils.h>
#include "HMDBSourceContextOperations.h"
#include <boost/lexical_cast.hpp>

using namespace medusaDatabase;

HMDBSourceContext::HMDBSourceContext(IHMDBDataContextPtr dataContext,
	IHMDBLocalContextPtr localContext,
	IHMDBRemoteContextPtr remoteContext)
	: dataContext_(dataContext), localContext_(localContext), remoteContext_(remoteContext)
{

}

HMDBSourceContext::~HMDBSourceContext()
{
	
}

const IHMDBLocalContextPtr HMDBSourceContext::localContext()
{
	return localContext_;
}

const IHMDBLocalContextConstPtr HMDBSourceContext::localContext() const
{
	return localContext_;
}

const IHMDBRemoteContextPtr HMDBSourceContext::remoteContext()
{
	return remoteContext_;
}

const IHMDBRemoteContextConstPtr HMDBSourceContext::remoteContext() const
{
	return remoteContext_;
}

const IHMDBDataContextPtr HMDBSourceContext::dataContext()
{
	return dataContext_;
}

const IHMDBDataContextConstPtr HMDBSourceContext::dataContext() const
{
	return dataContext_;
}