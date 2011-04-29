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
#include <core/ObjectWrapper.h>

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
        //! Nazwa slotu.
        std::string name;
        //! Typy wspierane w slocie.
        core::TypeInfoList types;
        //! Pomocnicza metoda, optymalizuj�ca podmian�.
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

private:
    //! W�a�ciwy slot.
    struct Slot
    {
        //! Obiekt w wariancie zmiennym.
        core::ObjectWrapperPtr object;
        //! Obiekt w wariancie niezmiennym.
        core::ObjectWrapperConstPtr constObject;
    };
    //! Sloty.
    std::vector<Slot> objects;
    //! Informacje o slotach.
    //! TODO: referencja albo wska�nik, jedna instancja danych. Spojrze�
    //!       jak to jest zrobione w wizualizatorach.
    SlotsInfo info;

public:
    //! \param info Informacje o slotach.
    ObjectSlots( const std::vector<SlotInfo>& info );
    //! \param info Informacje o slotach.
    ObjectSlots( std::vector<SlotInfo>&& info );
    //! Konstruktor kopiuj�cy.
    ObjectSlots( const ObjectSlots& objectSlots );

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
    const core::TypeInfoList& getSlotTypes(int i) const
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

    //! Dodaje zmienny obiekt. Ustawia r�wnie� wska�nik z niezmiennego obiektu.
    void setObject(int slotNo, const core::ObjectWrapperPtr& object);

    //! Dodanie niezmiennego obiektu. Zeruje wska�nik zmiennego obiektu.
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


#endif  // HEADER_GUARD_CORE__OBJECTSLOTS_H__