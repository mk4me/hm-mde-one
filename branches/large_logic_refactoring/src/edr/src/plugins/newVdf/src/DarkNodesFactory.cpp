#include "NewVdfPCH.h"
#include "DarkNodesFactory.h"
#include "DarkNodeStrategy.h"
#include "StyleItem.h"
#include "StyleSinkNode.h"
#include "StyleSourceNode.h"
#include "StyleProcessingNode.h"

//IVisualSinkNode * NodesFactory::createSinkNode() const
//{
//    return new SimpleSinkNode();
//}
//
//IVisualSourceNode * NodesFactory::createSourceNode() const
//{
//    return new SimpleSourceNode();
//}
//
//IVisualProcessingNode * NodesFactory::createProcessingNode() const
//{
//    return new SimpleProcessingNode();
//}
//
//const QUuid NodesFactory::id() const
//{
//    return QUuid::createUuid();
//}
//
//const QString NodesFactory::name() const
//{
//    return "Simple node";
//}
//
//const QString NodesFactory::description() const
//{
//    return "Simple node description";
//}


IVisualSinkNodePtr DarkNodesFactory::createSinkNode() const
{
	auto sink = IVisualSinkNodePtr(new StyleSinkNode());
	auto strategy = INodeStrategyPtr(new DarkNodeStrategy);
	strategy->setNode(sink);
	sink->setVisualStrategy(strategy);
	return sink;
}

IVisualSourceNodePtr DarkNodesFactory::createSourceNode() const
{
	auto source  = IVisualSourceNodePtr(new StyleSourceNode());
	auto strategy = INodeStrategyPtr(new DarkNodeStrategy);
	strategy->setNode(source);
	source->setVisualStrategy(strategy);
	return source;
}

IVisualProcessingNodePtr DarkNodesFactory::createProcessingNode() const
{
	auto processing = IVisualProcessingNodePtr(new StyleProcessingNode());
	auto strategy = INodeStrategyPtr(new DarkNodeStrategy);
	strategy->setNode(processing);
	processing->setVisualStrategy(strategy);
	return processing;
}

const QUuid DarkNodesFactory::id() const
{
	return QUuid::createUuid();
}

const QString DarkNodesFactory::name() const
{
	return "Style Nodes Factory";
}

const QString DarkNodesFactory::description() const
{
	return "Style Nodes Factory desc";
}

