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

    return false;
}

const core::ObjectWrapperCollectionPtr & EDRDFPin::getSlotData()
{
    return objectSlots->getObjects(slotNo);
}

void EDRDFPin::setSlotData(const core::ObjectWrapperCollectionPtr & data)
{
    objectSlots->setObjects(slotNo, data);
}

void EDRDFPin::onReset()
{
    if(this->getType() == dflm::Pin::OUT){
        const auto& objects = getSlotData();
        if(objects != nullptr){
            objects->clear();
        }
    }
}
