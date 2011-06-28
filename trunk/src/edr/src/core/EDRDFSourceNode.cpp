#include "CorePCH.h"
#include "EDRDFSourceNode.h"
#include "EDRDFPin.h"
#include "OutputDescription.h"

EDRDFSourceNode::EDRDFSourceNode(const WorkflowItemPtr & workflowItem, const std::string & name)
    : dflm::DFNode(name), dflm::DFSourceNode(name),
    EDRDFNode(workflowItem, name), source(dynamic_cast<const core::IDataSource*>(workflowItem->getImplementation()))
{

}

EDRDFSourceNode::~EDRDFSourceNode()
{

}

void EDRDFSourceNode::doInitialization(const dflm::Node::PinsAdderPtr & pinsAdder)
{
    OutputDescription * outputDescription = dynamic_cast<OutputDescription*>(getWorkflowItem().get());

    //stwórz piny wyjsciowe
    for(int i = 0; i < outputDescription->getNumOutputs(); i++){
        pinsAdder->addOutPin(dflm::PinPtr(new EDRDFPin(outputDescription->getOutput(), i)));
    }
}

bool EDRDFSourceNode::hasMoreData() const
{
    return !(source->empty());
}

void EDRDFSourceNode::processData()
{
    EDRDFNode::processData();
}