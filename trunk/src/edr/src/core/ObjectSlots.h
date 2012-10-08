/********************************************************************
    created:  2011/04/29
    created:  29:4:2011   10:10
    filename: ObjectSlots.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__OBJECTSLOTS_H__
#define HEADER_GUARD_CORE__OBJECTSLOTS_H__

#include <vector>
#include <set>
#include <core/ObjectWrapperCollection.h>
/**
 *	Reprezentuje pewną kolekcję obiektów domenowych. Odwołania do nich
 *  występują pod postacią zmiennych i/lub niezmiennych wskaźników. 
 *  Obiekty przypisane są do slotów, które charakteryzują się nazwami
 *  oraz listą wspieranych przez nie typów.
 */
class ObjectSlots
{
public:
    //! Typ opisu slotu.
    struct SlotInfo
    {
        //! Typ kolekcji slotow zaleznych (ich indeksów)
        typedef std::set<int> DependentSlots;

        //! Nazwa slotu.
        std::string name;
        //! Typy wspierane w slocie.
        core::TypeInfo type;
        //! Czy dane z wejścia będa modyfikowane
        bool modify;
        //! Czy slot musi być podłączony
        bool required;
        //! Zbiór slotów wymaganych
        DependentSlots dependentSlots;
        //! Pomocnicza metoda, optymalizująca podmianę.
        void swap(SlotInfo& slotInfo)
        {
            if ( &slotInfo != this ) {
                name.swap(slotInfo.name);
                std::swap(type,slotInfo.type);
                std::swap(required, slotInfo.required);
                dependentSlots.swap(slotInfo.dependentSlots);
            }
        }
    };
    //! Informacje o slotach.
    typedef std::vector<SlotInfo> SlotsInfo;

    typedef SlotInfo::DependentSlots DependentSlots;

private:
    //! Właściwy slot.
    struct Slot
    {
        //! Obiekt w wariancie zmiennym.
        core::ObjectWrapperCollectionPtr objects;
        //! Obiekt w wariancie niezmiennym.
        core::ObjectWrapperCollectionConstPtr constObjects;
    };
    //! Sloty.
    std::vector<Slot> objects;
    //! Informacje o slotach.
    //! TODO: referencja albo wskaźnik, jedna instancja danych. Spojrzeć
    //!       jak to jest zrobione w wizualizatorach.
    SlotsInfo info;

public:
    //! \param info Informacje o slotach.
    ObjectSlots( const SlotsInfo& info );
    //! \param info Informacje o slotach.
    ObjectSlots( SlotsInfo&& info );
    //! Konstruktor kopiujący.
    ObjectSlots( const ObjectSlots& objectSlots );

public:
    //! \return Liczba slotów.
    int getNumSlots() const
    {
        return info.size();
    }
    //! \return Nazwa slotu.
    const std::string& getSlotName(int slotNo) const
    {
        return info[slotNo].name;
    }
    //! \return Typy wspierane przez slot.
    const core::TypeInfo& getSlotType(int slotNo) const
    {
        return info[slotNo].type;
    }
    //! \return Informacje o slocie.
    const SlotInfo& getSlotInfo(int slotNo) const
    {
        return info[slotNo];
    }

    //! Czy dany obiekt można przypisać danemu źródłu?
    bool isAssignable(int slotNo, const core::TypeInfo & type) const;

    //! Dodaje zmienny obiekt. Ustawia również wskaźnik z niezmiennego obiektu.
    void setObjects(int slotNo, const core::ObjectWrapperCollectionPtr& objects);

    //! Dodanie niezmiennego obiektu. Zeruje wskaźnik zmiennego obiektu.
    void setObjects(int slotNo, const core::ObjectWrapperCollectionConstPtr& objects);

    //! \return Obiekt w zmiennym wariancie.
    const core::ObjectWrapperCollectionPtr& getObjects(int slotNo) const
    {
        return objects[slotNo].objects;
    }
    //! \return Obiekt w stałym wariancie.
    const core::ObjectWrapperCollectionConstPtr& getConstObjects(int slotNo) const
    {
        return objects[slotNo].constObjects;
    }
};


#endif  // HEADER_GUARD_CORE__OBJECTSLOTS_H__
