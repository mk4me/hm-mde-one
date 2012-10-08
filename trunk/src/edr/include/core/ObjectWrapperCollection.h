/********************************************************************
	created:	2011/05/23
	created:	23:5:2011   23:14
	filename: 	ObjectWrapperCollection.h
	author:		Wojciech Knieć

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__OBJECTWRAPPERCOLLECTION_H__
#define HEADER_GUARD_CORE__OBJECTWRAPPERCOLLECTION_H__

#include <vector>
#include <stdexcept>
#include <core/ObjectWrapper.h>

class DataManager;

namespace core {

class ObjectWrapperCollection;

typedef shared_ptr<ObjectWrapperCollection> ObjectWrapperCollectionPtr;
typedef shared_ptr<const ObjectWrapperCollection> ObjectWrapperCollectionConstPtr;

typedef const shared_ptr<const ObjectWrapperCollection> ConstObjectWrapperCollectionConstPtr;

typedef weak_ptr<ObjectWrapperCollection> ObjectWrapperCollectionWeakPtr;
typedef weak_ptr<const ObjectWrapperCollection> ObjectWrapperCollectionConstWeakPtr;

//! Klasa służy do agregowania obiektów domenowych tego samego typu lub pocohdnych od tego samego typu
class ObjectWrapperCollection
{
    friend class DataManager;

public:
    typedef std::vector<ObjectWrapperConstPtr>::size_type size_type;
    typedef std::vector<ObjectWrapperConstPtr>::const_iterator const_iterator;
    typedef std::vector<ObjectWrapperConstPtr>::iterator iterator;

private:
    typedef std::vector<ObjectWrapperConstPtr> ConstObjects;

private:

    //! przechowuje stale, niemodyfikowalne obiekty
    ConstObjects constObjects;
    //! typ przechowywanych obiektów
    TypeInfo typeInfo;
    //! Czy kolekcja przechowyje elementy wylacznie danego typu czy również pochodne mu
    bool exact;

// rev - zmiana na public, bo z tej metody korzysta DataManager
// gcc jakos nie widzi slowa friend wczesniej !?
public:

    //! Metoda dedykowana DataManager do szybkiego ładowania kolekcji bez sprawdzania typów ladowanych danych - powinno być już sprawdzone i zagwarantowane
    //! \param begin Iterator początku zakresu ObjectWrapperPtr
    //! \param begin Iterator końca zakresu ObjectWrapperPtr
    template<class Iter>
    void loadCollectionWithData(Iter begin, Iter end)
    {
        constObjects.insert(constObjects.end(), begin, end);
    };

public:

    //! Chroniony konstruktor, klasa pochodna powinna zdeklarować przechowywany typ
    //! \param info typ przechowywanych obiektów
    ObjectWrapperCollection(TypeInfo info, bool exact = true) :
      typeInfo(info), exact(exact) {}

      ObjectWrapperCollection(const ObjectWrapperCollection & owc) : typeInfo(owc.typeInfo),
          constObjects(owc.constObjects), exact(owc.exact) {}

    virtual ~ObjectWrapperCollection() {}

public:

    //! \return Najniższy typ w hierarchi dziedziczenia który kolekcja może przechować
    const TypeInfo & getTypeInfo() const
    {
        return typeInfo;
    }

    //! \return Czy dane musza być dokłądnie tego samego typu dla którego utworzono kolekcję czy mogą też być pochodne od niego
    bool exactTypes() const
    {
        return exact;
    }

    //! \param exact Czy dane mają być dokładnie tego typu dla którego utworzono kolekcję
    //! \param clear Czy czyścić dane niezgodne z typem kolekcji - pochodne mu
    void setExactTypes(bool exact)
    {
        this->exact = exact;

        if(exact == true){
            removeDerivedTypes();
        }
    }

    //! Czyści dane innego typu niż ten dla którego utworozno kolekcję
    void removeDerivedTypes()
    {
        //iteruj po kolekcji i usun te typy które nie są dokładnie typu kolekcji
        auto cIT = constObjects.begin();
        while( cIT != constObjects.end() ) {
            if((*cIT)->getTypeInfo() != typeInfo){
                cIT = constObjects.erase(cIT);
            }else{
                ++cIT;
            }
        }
    }

    //! return Iterator początku kolekcji
    iterator begin()
    {
        return constObjects.begin();
    }

    //! return Iterator końca kolekcji
    iterator end()
    {
        return constObjects.end();
    }

    //! return Const iterator początku kolekcji
    const_iterator begin() const
    {
        return constObjects.begin();
    }

    //! return Const iterator końca kolekcji
    const_iterator end() const
    {
        return constObjects.end();
    }

    //! \return Czy kolekcja jest pusta
    bool empty() const
    {
            return constObjects.empty();
    }

    //! \return Ilość elementów w kolekcji
    size_type size() const
    {
        return constObjects.size();
    }

    //! Czyści wszystkie elementy kolekcji
    void clear()
    {
        ConstObjects().swap(constObjects);
    }

    //! Metoda udostępnia obiekt domenowy z agregatu
    //! \param index indesk pobieranego elementu
    //! \return Niemodyfikowalny obiekt domenowy
    const ObjectWrapperConstPtr & getObject(int index) const
    {
        // wyjątek zamiast asercji (na wypadek trybu release)
        if (!(index < static_cast<int>(size()) && index >= 0)) {
            throw std::runtime_error("ObjectWrapperCollection::getObject - wrong index");
        }

        return constObjects[index];
    }

    //! \param idx Indeks który usuwamy z kolekcji
    void removeObject(int idx)
    {
        auto cIT = constObjects.begin();
        std::advance(cIT, idx);
        constObjects.erase(cIT);
    }

    //! Dodanie obiektu do agregatu
    //! \param object wskaźnik do niemodyfikowalnego obiektu domenowego
    void addObject(const ObjectWrapperConstPtr & object)
    {
        // sprawdzenie poprawności typu
        if (exact == true){
            if(object->isTypeEqual(typeInfo) == true) {
                constObjects.push_back(object);
            } else {
                // bad_cast nie pobiera parametrów
                //throw std::bad_cast("Type of object not equal to type of collection");
                throw std::bad_cast();
            }
        }else if(object->isSupported(typeInfo) == true){
            constObjects.push_back(object);
        } else {
            // rev jw
            //throw std::bad_cast("Type of object not supported by collection");
            throw std::bad_cast();
        }
    }
};

}

#endif
