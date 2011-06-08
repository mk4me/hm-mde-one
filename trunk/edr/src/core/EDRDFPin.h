/********************************************************************
    created:  2011/05/20
    created:  20:5:2011   11:54
    filename: EDRDFPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___EDRDFPIN_H__
#define HEADER_GUARD___EDRDFPIN_H__

#include "ObjectSlots.h"

#ifdef IN
#undef IN
#endif

#ifdef OUT
#undef OUT
#endif

#include <dfmlib/DFPin.h>

class EDRDFPin : public dflm::DFPin
{
public:
    EDRDFPin(ObjectSlots * objectSlots, int slotNo, const dflm::Pin::ReqPinsSet & requiredPins = dflm::Pin::ReqPinsSet());

    ~EDRDFPin();

    //! \param pin Pin którego kompatybilnoœc testujemy
    //! \return Prawda jeœli piny s¹ kompatybilne (aktualny pin potrafi obs³u¿yæ testowany pin)
    virtual bool isCompatible(const dflm::CPinPtr & pin) const;

    const core::ObjectWrapperCollectionPtr & getSlotData();
    const core::ObjectWrapperCollectionConstPtr & getSlotData() const;

    void setSlotData(const core::ObjectWrapperCollectionPtr & data);
    void setSlotData(const core::ObjectWrapperCollectionConstPtr & data);

private:
    int slotNo;
    ObjectSlots * objectSlots;
};

#endif  //  HEADER_GUARD___EDRDFPIN_H__