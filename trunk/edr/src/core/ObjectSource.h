/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:43
    filename: ObjectSource.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__OBJECTSOURCE_H__
#define HEADER_GUARD_CORE__OBJECTSOURCE_H__

#include <vector>
#include <core/IObjectSource.h>
#include <core/ObjectWrapperCollection.h>
#include "ObjectSlots.h"
#include "DataManager.h"

class ObjectSource : public ObjectSlots, public core::IObjectSource
{
//private:
//    std::vector<bool> passDirectly;


public:
    //! \param info Informacje o slotach.
    ObjectSource( SlotsInfo&& info ) :
    ObjectSlots(info)//, passDirectly(info.size())
    {}
    //! \param info Informacje o slotach.
    ObjectSource( const SlotsInfo& info ) :
    ObjectSlots(info)//, passDirectly(info.size())
    {}

public:

    //! Czy przekazywaæ obiekt bezpoœrednio czy te¿ zawsze konieczne jest
    //! klonowanie?
    //! \param idx
    //! \param passDirectly
    /*void setPassDirectly(int idx, bool passDirectly)
    {
        UTILS_ASSERT((idx >= 0 && idx < getNumSources()), "Bledny indeks wejscia");
        this->passDirectly[idx] = passDirectly;
    }*/
    //! \return Czy obiekt w danym slocie mo¿na przekazaæ bezpoœrednio, bez klonowania?
    bool isPassedDirectly(int inputNo) const
    {
        UTILS_ASSERT((inputNo >= 0 && inputNo < getNumSources()), "Bledny indeks wejscia");
        //return passDirectly[idx];
        return ObjectSlots::getSlotInfo(inputNo).modify == false;
    }

    virtual bool isChanged(int inputNo) const
    {
        UTILS_ASSERT((inputNo >= 0 && inputNo < getNumSources()), "Bledny indeks wejscia");
        throw std::runtime_error("not supported");
        return false;
    }

    virtual int getNumSources() const
    {
        return ObjectSlots::getNumSlots();
    }

    virtual core::IObjectSource::InputObjectsCollection getObjects(int inputNo/*, boost::true_type*/) const
    {
        UTILS_ASSERT((inputNo >= 0 && inputNo < getNumSources()), "Bledny indeks wejscia");
        return core::IObjectSource::InputObjectsCollection(ObjectSlots::getConstObjects(inputNo));
    }

    //virtual core::ConstObjectWrapperCollectionConstPtr getObjects(int inputNo/*, boost::true_type*/) const
    //{
    //    UTILS_ASSERT((inputNo >= 0 && inputNo < getNumSources()), "Bledny indeks wejscia");
    //    return core::ConstObjectWrapperCollectionConstPtr(ObjectSlots::getConstObjects(inputNo));
    //}
};

#endif  // HEADER_GUARD_CORE__OBJECTSOURCE_H__