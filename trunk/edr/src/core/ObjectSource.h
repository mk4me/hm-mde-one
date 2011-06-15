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
    //virtual core::ObjectWrapperCollectionPtr getObjects(int inputNo, boost::false_type)
    //{
    //    UTILS_ASSERT((inputNo >= 0 && inputNo < getNumSources()), "Bledny indeks wejscia");

    //    // najpierw sprawdzamy, czy mo¿emy przekazaæ bez klonowania
    //    if ( isPassedDirectly(inputNo) ) {
    //        core::ObjectWrapperCollectionPtr result = ObjectSlots::getObjects(inputNo);
    //        if ( result ) {
    //            return result;
    //        }
    //    }
    //    // klonujemy obiekt z jego sta³ego wariantu
    //    core::ObjectWrapperCollectionConstPtr constResult = ObjectSlots::getConstObjects(inputNo);
    //    if ( constResult ) {
    //        return constResult->clone();
    //    } else {
    //        //return core::ObjectWrapperCollectionPtr();
    //        return DataManager::getInstance()->createWrapperCollection(ObjectSlots::getSlotType(inputNo));
    //    }
    //}
    virtual core::ConstObjectWrapperCollectionConstPtr getObjects(int inputNo/*, boost::true_type*/) const
    {
        UTILS_ASSERT((inputNo >= 0 && inputNo < getNumSources()), "Bledny indeks wejscia");
        return core::ConstObjectWrapperCollectionConstPtr(ObjectSlots::getConstObjects(inputNo));
    }
};

// 
// //! ród³o obiektów.
// class ObjectSource_old : public core::IObjectSource
// {
// public:
//     //! ród³o faktyczne danych.
//     typedef std::pair<core::ObjectWrapperPtr, core::ObjectWrapperConstPtr> Source;
//     //! Lista Ÿróde³ danych.
//     typedef std::vector<Source> Sources;
//     //! Lista typów kolejnych Ÿróde³.
//     typedef std::vector< std::pair<std::string, core::ObjectWrapper::Types> > SourcesTypes; 
// 
// private:
//     //! Lista Ÿróde³ danych.
//     Sources sources;
//     //! Lista typów danych dla Ÿróde³.
//     //! Wczeœniej by³a lokalna kopia, teraz to tylko referencja - w³aœciwe dane trzyma VisualizerManager.
//     const SourcesTypes& sourcesTypes;
// 
// public:
//     //! \param numSources Liczba Ÿróde³.
//     // ObjectSource(const SourcesTypes& types);
//     //! \param visualizer Wizualizator z którego mozna pobraæ informacje o wejœciach.
//     ObjectSource_old(core::IVisualizer* visualizer);
//     //! Konstruktor kopiuj¹cy.
//     ObjectSource_old(const ObjectSource_old& source);
// 
// public:
//     //! Czy dany obiekt mo¿na przypisaæ danemu Ÿród³u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapperPtr& object) const;
//     //! Czy dany obiekt mo¿na przypisaæ danemu Ÿród³u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapperConstPtr& object) const;
// 
//     //! Dodaje zmienny obiekt.
//     void setObject(int sourceNo, const core::ObjectWrapperPtr& object);
//     //! Dodanie niezmienny obiekt.
//     void setObject(int sourceNo, const core::ObjectWrapperConstPtr& object);
// 
//     //! \return Lista typów dla danego slotu.
//     const core::ObjectWrapper::Types& getTypes(int sourceNo) const;
//     //! \return Nazwa danego slotu.
//     const std::string& getName(int sourceNo) const;
// 
// // IObjectSource
// public:
//     virtual bool isChanged(int inputNo) const;
//     virtual int getNumObjects() const;
//     virtual core::ObjectWrapperPtr getObject(int idx, boost::false_type);
//     virtual core::ObjectWrapperConstPtr getObject(int idx, boost::true_type);
// 
// private:
//     //! Czy dany obiekt mo¿na przypisaæ danemu Ÿród³u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapper* object) const;
// };
// 



#endif  // HEADER_GUARD_CORE__OBJECTSOURCE_H__