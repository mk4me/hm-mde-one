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

    //! Czy przekazywa� obiekt bezpo�rednio czy te� zawsze konieczne jest
    //! klonowanie?
    //! \param idx
    //! \param passDirectly
    void setPassDirectly(int idx, bool passDirectly)
    {
        this->passDirectly[idx] = passDirectly;
    }
    //! \return Czy obiekt w danym slocie mo�na przekaza� bezpo�rednio, bez klonowania?
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
        // najpierw sprawdzamy, czy mo�emy przekaza� bez klonowania
        if ( isPassedDirectly(idx) ) {
            core::ObjectWrapperPtr result = ObjectSlots::getObject(idx);
            if ( result ) {
                return result;
            }
        }
        // klonujemy obiekt z jego sta�ego wariantu
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

// TODO: przenie�� do osobnego pliku
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

    //! \return Liczba slot�w wyj�ciowych.
    virtual int getNumObjects() const
    {
        return ObjectSlots::getNumSlots();
    }

};

// 
// //! �r�d�o obiekt�w.
// class ObjectSource_old : public core::IObjectSource
// {
// public:
//     //! �r�d�o faktyczne danych.
//     typedef std::pair<core::ObjectWrapperPtr, core::ObjectWrapperConstPtr> Source;
//     //! Lista �r�de� danych.
//     typedef std::vector<Source> Sources;
//     //! Lista typ�w kolejnych �r�de�.
//     typedef std::vector< std::pair<std::string, core::ObjectWrapper::Types> > SourcesTypes; 
// 
// private:
//     //! Lista �r�de� danych.
//     Sources sources;
//     //! Lista typ�w danych dla �r�de�.
//     //! Wcze�niej by�a lokalna kopia, teraz to tylko referencja - w�a�ciwe dane trzyma VisualizerManager.
//     const SourcesTypes& sourcesTypes;
// 
// public:
//     //! \param numSources Liczba �r�de�.
//     // ObjectSource(const SourcesTypes& types);
//     //! \param visualizer Wizualizator z kt�rego mozna pobra� informacje o wej�ciach.
//     ObjectSource_old(core::IVisualizer* visualizer);
//     //! Konstruktor kopiuj�cy.
//     ObjectSource_old(const ObjectSource_old& source);
// 
// public:
//     //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapperPtr& object) const;
//     //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapperConstPtr& object) const;
// 
//     //! Dodaje zmienny obiekt.
//     void setObject(int sourceNo, const core::ObjectWrapperPtr& object);
//     //! Dodanie niezmienny obiekt.
//     void setObject(int sourceNo, const core::ObjectWrapperConstPtr& object);
// 
//     //! \return Lista typ�w dla danego slotu.
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
//     //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
//     bool isAssignable(int sourceNo, const core::ObjectWrapper* object) const;
// };
// 



#endif  // HEADER_GUARD_CORE__OBJECTSOURCE_H__