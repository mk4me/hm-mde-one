/********************************************************************
    created:  2011/05/07
    created:  7:5:2011   16:33
    filename: ObjectOutput.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__OBJECTOUTPUT_H__
#define HEADER_GUARD_CORE__OBJECTOUTPUT_H__

#include "ObjectSlots.h"
#include <core/IObjectOutput.h>

namespace core {

class ObjectOutput : public ObjectSlots, public plugin::IObjectOutput
{
public:
    //! \param info Informacje o slotach.
    ObjectOutput( std::vector<SlotInfo>&& info ) :
      ObjectSlots(info)
      {}
      //! \param info Informacje o slotach.
      ObjectOutput( const std::vector<SlotInfo>& info ) :
      ObjectSlots(info)
      {}

      // core::IObjectOutput
public:
    virtual plugin::IObjectOutput::OutputObjectsCollection getObjects(int idx)
    {
        return plugin::IObjectOutput::OutputObjectsCollection(ObjectSlots::getObjects(idx));
    }

    //! \return Liczba slotów wyjściowych.
    virtual int getNumOutputs() const
    {
        return ObjectSlots::getNumSlots();
    }

};

}

#endif  //  HEADER_GUARD_CORE__OBJECTOUTPUT_H__
