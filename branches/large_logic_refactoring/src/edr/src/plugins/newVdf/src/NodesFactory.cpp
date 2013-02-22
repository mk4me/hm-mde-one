#include "NewVdfPCH.h"
#include "NodesFactory.h"
#include "SimpleItem.h"
#include "SimpleSinkNode.h"
#include "SimpleSourceNode.h"
#include "SimpleProcessingNode.h"
#include "StyleItem.h"
#include "StyleSinkNode.h"
#include "StyleSourceNode.h"
#include "StyleProcessingNode.h"

using namespace vdf;

IVisualSinkNodePtr NodesFactory::createSinkNode() const
{
    return IVisualSinkNodePtr(new SimpleSinkNode());
}

IVisualSourceNodePtr NodesFactory::createSourceNode() const
{
    return IVisualSourceNodePtr(new SimpleSourceNode());
}

IVisualProcessingNodePtr NodesFactory::createProcessingNode() const
{
    return IVisualProcessingNodePtr(new SimpleProcessingNode());
}

const QUuid NodesFactory::id() const
{
    return QUuid::createUuid();
}

const QString NodesFactory::name() const
{
    return "Simple node";
}

const QString NodesFactory::description() const
{
    return "Simple node description";
}


//IVisualSinkNode * StyleNodesFactory::createSinkNode() const
//{
//	auto* sink  = new StyleSinkNode();
//	auto strategy = INodeStrategyPtr(new NodeStrategy);
//	strategy->setNode(sink);
//	sink->setVisualStrategy(strategy);
//	return sink;
//}
//
//IVisualSourceNode * StyleNodesFactory::createSourceNode() const
//{
//	auto* source  = new StyleSourceNode();
//	auto strategy = INodeStrategyPtr(new NodeStrategy);
//	strategy->setNode(source);
//	source->setVisualStrategy(strategy);
//	return source;
//}
//
//IVisualProcessingNode * StyleNodesFactory::createProcessingNode() const
//{
//	auto* processing  = new StyleProcessingNode();
//	auto strategy = INodeStrategyPtr(new NodeStrategy);
//	strategy->setNode(processing);
//	processing->setVisualStrategy(strategy);
//	return processing;
//}
//
//const QUuid StyleNodesFactory::id() const
//{
//	return QUuid::createUuid();
//}
//
//const QString StyleNodesFactory::name() const
//{
//	return "Style Nodes Factory";
//}
//
//const QString StyleNodesFactory::description() const
//{
//	return "Style Nodes Factory desc";
//}

