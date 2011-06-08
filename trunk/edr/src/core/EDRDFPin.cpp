#include "CorePCH.h"
#include "EDRDFPin.h"
#include "DataManager.h"

EDRDFPin::EDRDFPin(ObjectSlots * objectSlots, int slotNo, const dflm::Pin::ReqPinsSet & requiredPins)
    : dflm::DFPin(objectSlots->getSlotInfo(slotNo).name, objectSlots->getSlotInfo(slotNo).required, requiredPins), objectSlots(objectSlots),
    slotNo(slotNo)
{

}

EDRDFPin::~EDRDFPin()
{

}

bool EDRDFPin::isCompatible(const dflm::CPinPtr & pin) const
{
    const EDRDFPin* edrpin = dynamic_cast<const EDRDFPin*>(pin.get());

    if(edrpin == nullptr){
        return false;
    }

    if(DataManager::getInstance()->isTypeCompatible(edrpin->objectSlots->getSlotType(edrpin->slotNo), objectSlots->getSlotType(slotNo)) == true){
        return true;
    }

    /*for(auto it = objectSlots->getSlotInfo(slotNo).type.begin(); it != objectSlots->getSlotInfo(slotNo).type.end(); it++){
        for(auto iT = edrpin->objectSlots->getSlotInfo(edrpin->slotNo).type.begin(); iT != edrpin->objectSlots->getSlotInfo(edrpin->slotNo).type.end(); iT++){
            if(*it == *iT){
                return true;
            }
        }
    }*/

    return false;
}

const core::ObjectWrapperCollectionPtr & EDRDFPin::getSlotData()
{
    return objectSlots->getObjects(slotNo);
}

const core::ObjectWrapperCollectionConstPtr & EDRDFPin::getSlotData() const
{
    return objectSlots->getObjects(slotNo);
}

void EDRDFPin::setSlotData(const core::ObjectWrapperCollectionPtr & data)
{
    objectSlots->setObjects(slotNo, data);
}

void EDRDFPin::setSlotData(const core::ObjectWrapperCollectionConstPtr & data)
{
    objectSlots->setObjects(slotNo, data);
}