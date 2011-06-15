#include "CorePCH.h"
#include "EDRDFNode.h"
#include "EDRDFPin.h"
#include "InputDescription.h"
#include "OutputDescription.h"

EDRDFNode::EDRDFNode(const WorkflowItemPtr & workflowItem, const std::string & nodeName)
    : dflm::DFNode(nodeName), workflowItem(workflowItem)
{
    UTILS_ASSERT((workflowItem != nullptr), "Z³y element przetwarzajacy");
}

EDRDFNode::~EDRDFNode()
{

}

const WorkflowItemPtr & EDRDFNode::getWorkflowItem() const
{
    return workflowItem;
}

void EDRDFNode::configure()
{
    QWidget* confWidget = workflowItem->getConfigurationWidget();
    if(confWidget != nullptr){
        //TODO
    }
}

void EDRDFNode::doInitialization(const dflm::Node::PinsAdderPtr & pinsAdder)
{
    //cast workflow item na 2 obiekty - InputDescription i OutputDescription
    //dla kazdego z tych interfejsow tworz odpowiednio inputpins lub output pins
    InputDescription * inputDescription = dynamic_cast<InputDescription*>(workflowItem.get());

    if(inputDescription != nullptr){
        //stwórz piny wejsciowe
        for(int i = 0; i < inputDescription->getNumInputs(); i++){
            pinsAdder->addInPin(dflm::PinPtr(new EDRDFPin(inputDescription->getSource(), i)));
        }
    }

    OutputDescription * outputDescription = dynamic_cast<OutputDescription*>(workflowItem.get());

    if(outputDescription != nullptr){        
        //stwórz piny wyjsciowe
        for(int i = 0; i < outputDescription->getNumOutputs(); i++){
            //buduj piny zalezne
            dflm::Pin::ReqPinsSet reqPins;
            for(auto it = outputDescription->getRequiredInputs(i).begin();
                it != outputDescription->getRequiredInputs(i).end(); it++) {
                reqPins.insert(getInPin(*it));
            }

            pinsAdder->addOutPin(dflm::PinPtr(new EDRDFPin(outputDescription->getOutput(), i, reqPins)));
        }
    }
}

void EDRDFNode::processData()
{
    workflowItem->tryRun();
}

void EDRDFNode::reset()
{
    workflowItem->tryReset();
}