/********************************************************************
    created:  2011/05/06
    created:  6:5:2011   16:13
    filename: EDRPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VDF__EDRPIN_H__
#define HEADER_GUARD_VDF__EDRPIN_H__

#include <dfmlib/DFPin.h>
#include <core/DataProcessor.h>

class EDRPin : public dflm::DFPin
{
public:
    EDRPin(const std::string & name = std::string(), bool required = false,
        const dflm::DFPin::REQ_PINS_SET & requiredPins = dflm::DFPin::REQ_PINS_SET());

    virtual bool isCompatible(CPinPtr pin);
};

#endif  //  HEADER_GUARD_VDF__EDRPIN_H__