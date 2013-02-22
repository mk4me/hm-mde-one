/********************************************************************
	created:	2012/12/16
	created:	16:12:2012   19:39
	filename: 	ConnectionsFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__CONNECTIONSFACTORY_H__
#define HEADER_GUARD_NEWVDF__CONNECTIONSFACTORY_H__

#include <plugins/newVdf/IConnectionsFactory.h>

namespace vdf {

class ConnectionsFactory : public IConnectionsFactory
{
public:
	typedef core::shared_ptr<ConnectionsFactory> Ptr;
	typedef core::shared_ptr<const ConnectionsFactory> ConstPtr;

public:
	virtual ~ConnectionsFactory() {}

    virtual IVisualConnectionPtr createConnection() const;

    virtual const QUuid id() const;

    virtual const QString name() const;

    virtual const QString description() const;

};
typedef core::shared_ptr<ConnectionsFactory> ConnectionFactoryPtr;
typedef core::shared_ptr<const ConnectionsFactory> ConnectionFactoryConstPtr;

}

#endif
