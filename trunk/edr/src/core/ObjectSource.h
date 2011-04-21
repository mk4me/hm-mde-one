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
    //! TODO: referencja albo wskaünik, jedna instancja danych.
    SlotsInfo info;
    //! Sloty.
    std::vector<Slot> objects;

public:
    //! \param info Informacje o slotach.
    ObjectSlots( const std::vector<SlotInfo>& info ) :
    info(info), objects(info.size())
    {}
    //! Konstruktor kopiujπcy.
    ObjectSlots( const ObjectSlots& objectSlots ) : 
    info(objectSlots.info), objects(objectSlots.objects)
    {}

public:
    //! \return Liczba slotÛw.
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

    //! Czy dany obiekt moøna przypisaÊ danemu ürÛd≥u?
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
    //! \return Obiekt w sta≥ym wariancie.
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
    //! Konstruktor kopiujπcy.
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
            // hmmm nie ma zmiennego, trzeba wiÍc sklonowaÊ niezmienny obiekt
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
    //! Konstruktor kopiujπcy.
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