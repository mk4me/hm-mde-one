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

namespace core 
{
    typedef TypeInfo Type;
    typedef ObjectWrapper::Types Types;
} // namespace core

class ObjectSlots
{
public:
    //!
    struct SlotInfo
    {
        //! Nazwa slotu.
        std::string name;
        //! Typy wspierane w slocie.
        core::Types types;

        void swap(SlotInfo& slotInfo)
        {
            if ( &slotInfo != this ) {
                name.swap(slotInfo.name);
                types.swap(slotInfo.types);
            }
        }
    };
    //! Informacje o slotach.
    typedef std::vector<SlotInfo> SlotsInfo;

    //!
    struct Slot
    {
        //! Obiekt w wariancie zmiennym.
        core::ObjectWrapperPtr object;
        //! Obiekt w wariancie niezmiennym.
        core::ObjectWrapperConstPtr constObject;
    };

private:
    //! Informacje o slotach.
    //! TODO: referencja albo wska�nik, jedna instancja danych.
    SlotsInfo info;
    //! Sloty.
    std::vector<Slot> objects;

public:
    //! \param info Informacje o slotach.
    ObjectSlots( const std::vector<SlotInfo>& info ) :
    info(info), objects(info.size())
    {}
    //! Konstruktor kopiuj�cy.
    ObjectSlots( const ObjectSlots& objectSlots ) : 
    info(objectSlots.info), objects(objectSlots.objects)
    {}

public:
    //! \return Liczba slot�w.
    int getNumSlots() const
    {
        return info.size();
    }
    //! \return Nazwa slotu.
    const std::string& getSlotName(int i) const
    {
        return info[i].name;
    }
    //! \return Typy wspierane przez slot.
    const core::Types& getSlotTypes(int i) const
    {
        return info[i].types;
    }
    //! \return Informacje o slocie.
    const SlotInfo& getSlotInfo(int i) const
    {
        return info[i];
    }

    //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
    bool isAssignable(int slotNo, const core::ObjectWrapper* object) const;

    //! Dodaje zmienny obiekt.
    void setObject(int slotNo, const core::ObjectWrapperPtr& object);

    //! Dodanie niezmiennego obiektu.
    void setObject(int slotNo, const core::ObjectWrapperConstPtr& object);

    //! \return Obiekt w zmiennym wariancie.
    const core::ObjectWrapperPtr& getObject(int slotNo)
    {
        return objects[slotNo].object;
    }
    //! \return Obiekt w sta�ym wariancie.
    const core::ObjectWrapperConstPtr& getConstObject(int slotNo) const
    {
        return objects[slotNo].constObject;
    }
    
};

class ObjectSource : public ObjectSlots, public core::IObjectSource
{
public:
    //! \param info Informacje o slotach.
    ObjectSource( const std::vector<SlotInfo>& info ) :
    ObjectSlots(info)
    {}
    //! Konstruktor kopiuj�cy.
    ObjectSource( const ObjectSource& objectSlots ) : 
    ObjectSlots(objectSlots)
    {}

public:
    virtual bool isChanged(int inputNo) const
    {
        return false;
    }
    virtual int getNumObjects() const
    {
        return ObjectSlots::getNumSlots();
    }
    virtual core::ObjectWrapperPtr getObject(int idx, boost::false_type)
    {
        core::ObjectWrapperPtr result = ObjectSlots::getObject(idx);
        if ( !result ) {
            // hmmm nie ma zmiennego, trzeba wi�c sklonowa� niezmienny obiekt
            throw std::runtime_error("Cloning not implemented yet.");
        }
        return result;
    }
    virtual core::ObjectWrapperConstPtr getObject(int idx, boost::true_type)
    {
        return ObjectSlots::getConstObject(idx);
    }
};

class ObjectOutput : public ObjectSlots, public core::IObjectOutput
{
public:
    //! \param info Informacje o slotach.
    ObjectOutput( const std::vector<SlotInfo>& info ) :
    ObjectSlots(info)
    {}
    //! Konstruktor kopiuj�cy.
    ObjectOutput( const ObjectOutput& objectSlots ) : 
    ObjectSlots(objectSlots)
    {}

// core::IObjectOutput
public:
    virtual core::ObjectWrapperPtr getWrapper(int idx, const core::Type& type)
    {
        const core::ObjectWrapperPtr& object = ObjectSlots::getObject(idx);
        UTILS_ASSERT(object);
        if ( !object->isSupported(type) ) {
            throw std::runtime_error("Type mismatch.");
        }
        return object;
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