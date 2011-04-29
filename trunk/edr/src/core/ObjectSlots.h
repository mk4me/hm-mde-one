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
        //! Nazwa slotu.
        std::string name;
        //! Typy wspierane w slocie.
        core::TypeInfoList types;
        //! Pomocnicza metoda, optymalizuj¹ca podmianê.
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
    //! W³aœciwy slot.
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
    //! TODO: referencja albo wskaŸnik, jedna instancja danych. Spojrzeæ
    //!       jak to jest zrobione w wizualizatorach.
    SlotsInfo info;

public:
    //! \param info Informacje o slotach.
    ObjectSlots( const std::vector<SlotInfo>& info );
    //! \param info Informacje o slotach.
    ObjectSlots( std::vector<SlotInfo>&& info );
    //! Konstruktor kopiuj¹cy.
    ObjectSlots( const ObjectSlots& objectSlots );

public:
    //! \return Liczba slotów.
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

    //! Czy dany obiekt mo¿na przypisaæ danemu Ÿród³u?
    bool isAssignable(int slotNo, const core::ObjectWrapper* object) const;

    //! Dodaje zmienny obiekt. Ustawia równie¿ wskaŸnik z niezmiennego obiektu.
    void setObject(int slotNo, const core::ObjectWrapperPtr& object);

    //! Dodanie niezmiennego obiektu. Zeruje wskaŸnik zmiennego obiektu.
    void setObject(int slotNo, const core::ObjectWrapperConstPtr& object);

    //! \return Obiekt w zmiennym wariancie.
    const core::ObjectWrapperPtr& getObject(int slotNo)
    {
        return objects[slotNo].object;
    }
    //! \return Obiekt w sta³ym wariancie.
    const core::ObjectWrapperConstPtr& getConstObject(int slotNo) const
    {
        return objects[slotNo].constObject;
    }
};


#endif  // HEADER_GUARD_CORE__OBJECTSLOTS_H__