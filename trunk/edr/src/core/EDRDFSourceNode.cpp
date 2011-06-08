#include "CorePCH.h"
#include "EDRDFSourceNode.h"
#include "EDRDFPin.h"
#include "OutputDescription.h"

EDRDFSourceNode::EDRDFSourceNode(const WorkflowItemPtr & workflowItem, const std::string & name)
    : dflm::DFNode(name), dflm::DFSourceNode(name), EDRDFNode(workflowItem, name), dataCount(1)
{

}

EDRDFSourceNode::~EDRDFSourceNode()
{

}

void EDRDFSourceNode::doInitialization(const dflm::Node::PinsAdderPtr & pinsAdder)
{
    OutputDescription * outputDescription = dynamic_cast<OutputDescription*>(getWorkflowItem().get());

    if(outputDescription != nullptr){
        //stwórz piny wyjsciowe
        for(int i = 0; i < outputDescription->getNumOutputs(); i++){
            pinsAdder->addOutPin(dflm::PinPtr(new EDRDFPin(outputDescription->getOutput(), i)));
        }
    }
}

bool EDRDFSourceNode::hasMoreData() const
{
    //TODO
    //specyfikacja Ÿróde³, okreœlania kiedy jeszcze maj¹ dane
    static bool ok = true;
    if(ok == true){
        ok = false;
        return true;
    }

    return false;
}