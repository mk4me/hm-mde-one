/********************************************************************
	created:	2011/05/23
	created:	23:5:2011   23:14
	filename: 	ObjectWrapperCollection.h
	author:		Wojciech Knieæ
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__OBJECTWRAPPERCOLLECTION_H__
#define HEADER_GUARD_CORE__OBJECTWRAPPERCOLLECTION_H__

#include <vector>
#include <core/ObjectWrapper.h>

namespace core {

//! Klasa sluzy do agregowania obiektow domenowych tego samego typu
class ObjectWrapperCollection
{
protected:
    //! Return type resolver
    struct get_t
    {
        ObjectWrapper* wrapper;
        const ObjectWrapper* constWrapper;

        //! operator zwraca wskaznik do modyfikowalnego obiektu,
        //! obiekt domenowy jest kopiowany, jesli zachodzi taka potrzeba.
        template <class Ptr>
        operator Ptr()
        {
            Ptr result;
            if (wrapper && wrapper->tryGet(result, exact)) {
                return result;
            } else if (constWrapper){
                return constWrapper->clone();
            } else {
                throw std::bad_cast("Invalid cast");
            }
        }

        //! Operator zwraca staly wskaznik do obiektu domenowego
        template <class Ptr>
        operator Ptr() const
        {
            Ptr result;
            if (constWrapper && constWrapper->tryGet(result, exact)) {
                return result;
            } else if (wrapper) {
                const ObjectWrapper* ptr = wrapper;
                ptr->tryGet(result, exact);
                return result;
            } else {
                throw std::bad_cast("Invalid cast");
            }
        }
    };
private:
    //! przechowuje modyfikowalne obiekty
    std::vector<ObjectWrapperPtr> objects;
    //! przechowuje stale, niemodyfikowalne obiekty
    std::vector<ObjectWrapperConstPtr> constObjects;
    //! typ przechowywanych obiektow
    core::TypeInfo typeInfo;

protected:
    //! Chroniony konstruktor, klasa pochodna powinna zdeklarowac przechowywany typ
    //! \param info typ przechowywanych obiektow
    ObjectWrapperCollection(TypeInfo info) :
         typeInfo(info) {}
public:
    virtual ~ObjectWrapperCollection() {}

public:
    //! Metoda udostepnia obiekt domenowy z agregatu
    //! \param index indesk pobieranego elementu
    //! \return Return type resolver decyduje o zwracanym typie
    get_t getObject(int index) 
    {
        // wyjatek zamiast asercji (na wypadek trybu release)
        if (!(index < getNumObjects() && index >= 0)) {
            throw std::runtime_error("ObjectWrapperCollection::getObject - wrong index");
        }

        get_t c;
        // dobor kolecji w zaleznosci od indeksu
        int treshold = objects.size();
        if (index < treshold) {
            // najpierw zwykle obiekty
            c.wrapper = objects[index].get();
            c.constWrapper = nullptr;
        } else {
            // pozniej stale obiekty
            c.constWrapper = constObjects[index - treshold].get();
            c.wrapper = nullptr;
        }

        return c;
    }
   
    //! Dodanie obiektu do agregatu
    //! \param object Wskaznik do modyfikowalnego obiektu domenowego
    void addObject(ObjectWrapperPtr object) 
    {
        insert(object, objects);
    }

    //! Dodanie obiektu do agregatu
    //! \param object Wskaznik do niemodyfikowalnego obiektu domenowego
    void addObject(ObjectWrapperConstPtr object) 
    {
        insert(object, constObjects);
    }

    //! wyczyszczenie kolekcji
    void clear() 
    {
        objects.clear();
        constObjects.clear();
    }

    //! \return liczba obiektow zarejstrowanych w agregacie
    int getNumObjects()
    {
        return objects.size() + constObjects.size();
    }

private:
    //! Pomocnicza metoda dodajaca elementy do kolekcji
    //! \param object dodawany obiekt
    //! \param collection kolekcja do ktorej dodajemy
    template <typename C>
    void insert( C object , std::vector<C>& collection) 
    {
        // sprawdzenie poprawnosci typu
        if (object->getType() == typeInfo) {
            collection.push_back(object);
        } else {
            throw std::bad_cast("Type of object not equal to type of collection");
        }
    }
};

//! \brief Klasa dostarcza typu, ktorego elementy beda przechowywane
template <class T>
class ObjectWrapperCollectionT : public ObjectWrapperCollection
{
public:
    ObjectWrapperCollectionT<T>() :
      ObjectWrapperCollection(type_of(T))
      {}
};

}

#endif