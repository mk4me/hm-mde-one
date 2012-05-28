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
 *	Reprezentuje pewn� kolekcj� obiekt�w domenowych. Odwo�ania do nich
 *  wyst�puj� pod postaci� zmiennych i/lub niezmiennych wska�nik�w. 
 *  Obiekty przypisane s� do slot�w, kt�re charakteryzuj� si� nazwami
 *  oraz list� wspieranych przez nie typ�w.
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
        //! Czy dane z wej�cia b�da modyfikowane
        bool modify;
        //! Czy slot musi by� pod��czony
        bool required;
        //! Zbi�r slot�w wymaganych
        DependentSlots dependentSlots;
        //! Pomocnicza metoda, optymalizuj�ca podmian�.
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
    //! W�a�ciwy slot.
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
    //! TODO: referencja albo wska�nik, jedna instancja danych. Spojrze�
    //!       jak to jest zrobione w wizualizatorach.
    SlotsInfo info;

public:
    //! \param info Informacje o slotach.
    ObjectSlots( const SlotsInfo& info );
    //! \param info Informacje o slotach.
    ObjectSlots( SlotsInfo&& info );
    //! Konstruktor kopiuj�cy.
    ObjectSlots( const ObjectSlots& objectSlots );

public:
    //! \return Liczba slot�w.
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

    //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
    bool isAssignable(int slotNo, const core::TypeInfo & type) const;

    //! Dodaje zmienny obiekt. Ustawia r�wnie� wska�nik z niezmiennego obiektu.
    void setObjects(int slotNo, const core::ObjectWrapperCollectionPtr& objects);

    //! Dodanie niezmiennego obiektu. Zeruje wska�nik zmiennego obiektu.
    void setObjects(int slotNo, const core::ObjectWrapperCollectionConstPtr& objects);

    //! \return Obiekt w zmiennym wariancie.
    const core::ObjectWrapperCollectionPtr& getObjects(int slotNo) const
    {
        return objects[slotNo].objects;
    }
    //! \return Obiekt w sta�ym wariancie.
    const core::ObjectWrapperCollectionConstPtr& getConstObjects(int slotNo) const
    {
        return objects[slotNo].constObjects;
    }
};


#endif  // HEADER_GUARD_CORE__OBJECTSLOTS_H__