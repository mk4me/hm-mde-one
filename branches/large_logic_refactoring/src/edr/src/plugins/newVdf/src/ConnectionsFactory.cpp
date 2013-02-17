#include "NewVdfPCH.h"
#include "ConnectionsFactory.h"
#include "SimpleConnection.h"
#include "StyleConnection.h"
#include <plugins/newVdf/IVisualPin.h>

IVisualConnectionPtr ConnectionsFactory::createConnection() const
{
    return IVisualConnectionPtr(new SimpleConnection());
}

const QUuid ConnectionsFactory::id() const
{
    return QUuid::createUuid();
}

const QString ConnectionsFactory::name() const
{
    return "Simple";
}

const QString ConnectionsFactory::description() const
{
    return "Simple line";
}
//
//IVisualConnection * StyleConnectionsFactory::createConnection() const
//{
//    auto connection = new StyleConnection();
//	auto strategy = IConnectionStrategyPtr(new ConnectionStrategy());
//	connection->setVisualStrategy(strategy);
//	return connection;
//}
//
//const QUuid StyleConnectionsFactory::id() const
//{
//    return QUuid::createUuid();
//}
//
//const QString StyleConnectionsFactory::name() const
//{
//    return "Curved";
//}
//
//const QString StyleConnectionsFactory::description() const
//{
//    return "Curved line";
//}
//
