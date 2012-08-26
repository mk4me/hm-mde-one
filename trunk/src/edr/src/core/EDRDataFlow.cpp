#include "CorePCH.h"
#include "EDRDataFlow.h"
#include "EDRDFPin.h"
#include "EDRDFNode.h"
#include <dfmlib/Connection.h>

EDRDataFlow::EDRDataFlow() : DFModel()
{

}

EDRDataFlow::~EDRDataFlow()
{

}

bool EDRDataFlow::isNodeSupported(const dflm::NPtr & node) const
{
    return dflm::DFModel::isNodeSupported(node) && boost::dynamic_pointer_cast<EDRDFNode>(node) != nullptr;
}

void EDRDataFlow::beforeRemoveConnection(const dflm::ConnPtr & connection)
{
    EDRDFPin * destPin = dynamic_cast<EDRDFPin*>(connection->getDest().get());

    destPin->setSlotData(core::ObjectWrapperCollectionPtr());
}

void EDRDataFlow::afterConnect(const dflm::ConnPtr & connection)
{
    EDRDFPin * srcPin = dynamic_cast<EDRDFPin*>(connection->getSrc().get());
    EDRDFPin * destPin = dynamic_cast<EDRDFPin*>(connection->getDest().get());

    destPin->setSlotData(srcPin->getSlotData());
}
