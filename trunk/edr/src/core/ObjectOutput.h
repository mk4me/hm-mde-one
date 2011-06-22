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

class ObjectOutput : public ObjectSlots, public core::IObjectOutput
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
    virtual core::IObjectOutput::OutputObjectsCollection getObjects(int idx)
    {
        //return ObjectSlots::getObjects(idx);
        return core::IObjectOutput::OutputObjectsCollection(ObjectSlots::getObjects(idx));
    }

    /*virtual void setObjects(int idx, const core::ObjectWrapperCollectionPtr & objects)
    {
        return ObjectSlots::setObjects(idx, objects);
    }

    virtual void setObjects(int idx, const core::ObjectWrapperCollectionConstPtr & objects)
    {
        return ObjectSlots::setObjects(idx, objects);
    }*/

    //! \return Liczba slotów wyjœciowych.
    virtual int getNumOutputs() const
    {
        return ObjectSlots::getNumSlots();
    }

};

#endif  //  HEADER_GUARD_CORE__OBJECTOUTPUT_H__
