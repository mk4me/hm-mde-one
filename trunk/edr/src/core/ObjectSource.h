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
#include <core/IObjectOutput.h>
#include <core/ObjectWrapper.h>
#include <core/IVisualizer.h>
#include "ObjectSlots.h"

class ObjectSource : public ObjectSlots, public core::IObjectSource
{
private:
    std::vector<bool> passDirectly;


public:
    //! \param info Informacje o slotach.
    ObjectSource( std::vector<SlotInfo>&& info ) :
    ObjectSlots(info), passDirectly(info.size())
    {}
    //! \param info Informacje o slotach.
    ObjectSource( const std::vector<SlotInfo>& info ) :
    ObjectSlots(info), passDirectly(info.size())
    {}

public:

    //! Czy przekazywaÊ obiekt bezpoúrednio czy teø zawsze konieczne jest
    //! klonowanie?
    //! \param idx
    //! \param passDirectly
    void setPassDirectly(int idx, bool passDirectly)
    {
        this->passDirectly[idx] = passDirectly;
    }
    //! \return Czy obiekt w danym slocie moøna przekazaÊ bezpoúrednio, bez klonowania?
    bool isPassedDirectly(int idx) const
    {
        return passDirectly[idx];
    }

    virtual bool isChanged(int inputNo) const
    {
        throw std::runtime_error("not supported");
        return false;
    }
    virtual int getNumObjects() const
    {
        return ObjectSlots::getNumSlots();
    }
    virtual core::ObjectWrapperPtr getObject(int idx, boost::false_type)
    {
        // najpierw sprawdzamy, czy moøemy przekazaÊ bez klonowania
        if ( isPassedDirectly(idx) ) {
            core::ObjectWrapperPtr result = ObjectSlots::getObject(idx);
            if ( result ) {
                return result;
            }
        }
        // klonujemy obiekt z jego sta≥ego wariantu
        core::ObjectWrapperConstPtr constResult = ObjectSlots::getConstObject(idx);
        if ( constResult ) {
            return constResult->clone();
        } else {
            return core::ObjectWrapperPtr();
        }
    }
    virtual core::ObjectWrapperConstPtr getObject(int idx, boost::true_type)
    {
        return ObjectSlots::getConstObject(idx);
    }
};

// TODO: przenieúÊ do osobnego pliku
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
    virtual core::ObjectWrapperPtr getWrapper(int idx)
    {
        return ObjectSlots::getObject(idx);
    }

    //! \return Liczba slotÛw wyjúciowych.
    virtual int getNumObjects() const
    {
        return ObjectSlots::getNumSlots();
    }

};

// 
// //! èrÛd≥o obiektÛw.
// class ObjectSource_old : public core::IObjectSource
// {
// public:
//     //! èrÛd≥o faktyczne danych.
//     typedef std::pair<core::ObjectWrapperPtr, core::ObjectWrapperConstPtr> Source;
//     //! Lista ürÛde≥ danych.
//     typedef std::vector<Source> Sources;
//     //! Lista typÛw kolejnych ürÛde≥.
//     typedef std::vector< std::pair<std::string, core::ObjectWrapper::Types> > SourcesTypes; 
// 
// private:
//     //! Lista ürÛde≥ danych.
//     Sources sources;
//     //! Lista typÛw danych dla ürÛde≥.
//     //! Wczeúniej by≥a lokalna kopia, teraz to tylko referencja - w≥aúciwe dane trzyma VisualizerManager.
//     const SourcesTypes& sourcesTypes;
// 
// public:
//     //! \param numSources Liczba ürÛde≥.
//     // ObjectSource(const SourcesTypes& types);
//     //! \param visualizer Wizualizator z ktÛrego mozna pobraÊ informacje o wejúciach.
//     ObjectSource_old(core::IVisualizer* visualizer);
//     //! Konstruktor kopiujπcy.
//     ObjectSource_old(const ObjectSource_old& source);
// 
// public:
//     //! Czy dany obiekt moøna przypisaÊ danemu ürÛd≥u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapperPtr& object) const;
//     //! Czy dany obiekt moøna przypisaÊ danemu ürÛd≥u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapperConstPtr& object) const;
// 
//     //! Dodaje zmienny obiekt.
//     void setObject(int sourceNo, const core::ObjectWrapperPtr& object);
//     //! Dodanie niezmienny obiekt.
//     void setObject(int sourceNo, const core::ObjectWrapperConstPtr& object);
// 
//     //! \return Lista typÛw dla danego slotu.
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
//     //! Czy dany obiekt moøna przypisaÊ danemu ürÛd≥u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapper* object) const;
// };
// 



#endif  // HEADER_GUARD_CORE__OBJECTSOURCE_H__