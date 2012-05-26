/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:43
    filename: ObjectSource.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__OBJECTSOURCE_H__
#define HEADER_GUARD_CORE__OBJECTSOURCE_H__

#include <core/IObjectSource.h>
//#include <core/ObjectWrapperCollection.h>
#include "ObjectSlots.h"

class ObjectSource : public ObjectSlots, public core::IObjectSource
{

public:
    //! \param info Informacje o slotach.
    ObjectSource( SlotsInfo&& info ) :
    ObjectSlots(info)
    {}
    //! \param info Informacje o slotach.
    ObjectSource( const SlotsInfo& info ) :
    ObjectSlots(info)
    {}

public:

    virtual int getNumSources() const
    {
        return ObjectSlots::getNumSlots();
    }

    virtual core::IObjectSource::InputObjectsCollection getObjects(int inputNo) const
    {
        UTILS_ASSERT((inputNo >= 0 && inputNo < getNumSources()), "Bledny indeks wejscia");
        return core::IObjectSource::InputObjectsCollection(ObjectSlots::getConstObjects(inputNo));
    }
};

#endif  // HEADER_GUARD_CORE__OBJECTSOURCE_H__