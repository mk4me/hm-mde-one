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
 *	Reprezentuje pewn¹ kolekcjê obiektów domenowych. Odwo³ania do nich
 *  wystêpuj¹ pod postaci¹ zmiennych i/lub niezmiennych wskaŸników. 
 *  Obiekty przypisane s¹ do slotów, które charakteryzuj¹ siê nazwami
 *  oraz list¹ wspieranych przez nie typów.
 */
class ObjectSlots
{
public:
    //! Typ opisu slotu.
    struct SlotInfo
    {
        //! Typ kolekcji slotow zaleznych (ich indeksow)
        typedef std::set<int> DependentSlots;

        //! Nazwa slotu.
        std::string name;
        //! Typy wspierane w slocie.
        core::TypeInfo type;
        //! Czy dane z wejœcia bêda modyfikowane
        bool modify;
        //! Czy slot musi byæ pod³¹czony
        bool required;
        //! Zbiór slotów wymaganych
        DependentSlots dependentSlots;
        //! Pomocnicza metoda, optymalizuj¹ca podmianê.
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
    //! W³aœciwy slot.
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
    //! TODO: referencja albo wskaŸnik, jedna instancja danych. Spojrzeæ
    //!       jak to jest zrobione w wizualizatorach.
    SlotsInfo info;

public:
    //! \param info Informacje o slotach.
    ObjectSlots( const SlotsInfo& info );
    //! \param info Informacje o slotach.
    ObjectSlots( SlotsInfo&& info );
    //! Konstruktor kopiuj¹cy.
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

    //! Czy dany obiekt mo¿na przypisaæ danemu Ÿród³u?
    bool isAssignable(int slotNo, const core::TypeInfo & type) const;

    //! Dodaje zmienny obiekt. Ustawia równie¿ wskaŸnik z niezmiennego obiektu.
    void setObjects(int slotNo, const core::ObjectWrapperCollectionPtr& objects);

    //! Dodanie niezmiennego obiektu. Zeruje wskaŸnik zmiennego obiektu.
    void setObjects(int slotNo, const core::ObjectWrapperCollectionConstPtr& objects);

    //! \return Obiekt w zmiennym wariancie.
    const core::ObjectWrapperCollectionPtr& getObjects(int slotNo) const
    {
        return objects[slotNo].objects;
    }
    //! \return Obiekt w sta³ym wariancie.
    const core::ObjectWrapperCollectionConstPtr& getConstObjects(int slotNo) const
    {
        return objects[slotNo].constObjects;
    }
};


#endif  // HEADER_GUARD_CORE__OBJECTSLOTS_H__