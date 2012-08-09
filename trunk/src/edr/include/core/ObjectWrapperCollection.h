/********************************************************************
	created:	2011/05/23
	created:	23:5:2011   23:14
	filename: 	ObjectWrapperCollection.h
	author:		Wojciech Knie�

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

//! Klasa sluzy do agregowania obiektow domenowych tego samego typu lub pocohdnych od tego samego typu
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
    //! typ przechowywanych obiektow
    TypeInfo typeInfo;
    //! Czy kolekcja przechowyje elementy wylacznie danego typu czy rowniez pochodne mu
    bool exact;

// rev - zmiana na public, bo z tej metody korzysta DataManager
// gcc jakos nie widzi slowa friend wczesniej !?
public:

    //! Metoda dedykowana DataManager do szybkiego �adowania kolekcji bez sprawdzania typ�w ladowanych danych - powinno byc juz sprawdzone i zagwarantowane
    //! \param begin Iterator pocz�tku zakresu ObjectWrapperPtr
    //! \param begin Iterator ko�ca zakresu ObjectWrapperPtr
    template<class Iter>
    void loadCollectionWithData(Iter begin, Iter end)
    {
        constObjects.insert(constObjects.end(), begin, end);
    };

public:

    //! Chroniony konstruktor, klasa pochodna powinna zdeklarowac przechowywany typ
    //! \param info typ przechowywanych obiektow
    ObjectWrapperCollection(TypeInfo info, bool exact = true) :
      typeInfo(info), exact(exact) {}

      ObjectWrapperCollection(const ObjectWrapperCollection & owc) : typeInfo(owc.typeInfo),
          constObjects(owc.constObjects), exact(owc.exact) {}

    virtual ~ObjectWrapperCollection() {}

public:

    //! \return Najni�szy typ w hierarchi dziedziczenia kt�ry kolekcja mo�e przechowa�
    const TypeInfo & getTypeInfo() const
    {
        return typeInfo;
    }

    //! \return Czy dane musza by� dok��dnie tego samego typu dla kt�rego utworzono kolekcj� czy mog� te� by� pochodne od niego
    bool exactTypes() const
    {
        return exact;
    }

    //! \param exact Czy dane maj� by� dok�adnie tego typu dla kt�rego utworzono kolekcj�
    //! \param clear Czy czy�ci� dane niezgodne z typem kolekcji - pochodne mu
    void setExactTypes(bool exact)
    {
        this->exact = exact;

        if(exact == true){
            removeDerivedTypes();
        }
    }

    //! Czy�ci dane innego typu ni� ten dla kt�rego utworozno kolekcj�
    void removeDerivedTypes()
    {
        //iteruj po kolekcji i usun te typy ktore nie sa dokladnie typu kolekcji
        auto cIT = constObjects.begin();
        while( cIT != constObjects.end() ) {
            if((*cIT)->getTypeInfo() != typeInfo){
                cIT = constObjects.erase(cIT);
            }else{
                ++cIT;
            }
        }
    }

    //! return Iterator pocz�tku kolekcji
    iterator begin()
    {
        return constObjects.begin();
    }

    //! return Iterator ko�ca kolekcji
    iterator end()
    {
        return constObjects.end();
    }

    //! return Const iterator pocz�tku kolekcji
    const_iterator begin() const
    {
        return constObjects.begin();
    }

    //! return Const iterator ko�ca kolekcji
    const_iterator end() const
    {
        return constObjects.end();
    }

    //! \return Czy kolekcja jest pusta
    bool empty() const
    {
            return constObjects.empty();
    }

    //! \return Ilo�� element�w w kolekcji
    size_type size() const
    {
        return constObjects.size();
    }

    //! Czy�ci wszystkie elementy kolekcji
    void clear()
    {
        ConstObjects().swap(constObjects);
    }

    //! Metoda udostepnia obiekt domenowy z agregatu
    //! \param index indesk pobieranego elementu
    //! \return Niemodyfikowalny obiekt domenowy
    const ObjectWrapperConstPtr & getObject(int index) const
    {
        // wyjatek zamiast asercji (na wypadek trybu release)
        if (!(index < static_cast<int>(size()) && index >= 0)) {
            throw std::runtime_error("ObjectWrapperCollection::getObject - wrong index");
        }

        return constObjects[index];
    }

    //! \param idx Indeks kt�ry usuwamy z kolekcji
    void removeObject(int idx)
    {
        auto cIT = constObjects.begin();
        std::advance(cIT, idx);
        constObjects.erase(cIT);
    }

    //! Dodanie obiektu do agregatu
    //! \param object Wskaznik do niemodyfikowalnego obiektu domenowego
    void addObject(const ObjectWrapperConstPtr & object)
    {
        // sprawdzenie poprawnosci typu
        if (exact == true){
            if(object->isTypeEqual(typeInfo) == true) {
                constObjects.push_back(object);
            } else {
                // bad_cast nie pobiera parametrow
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
