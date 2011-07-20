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

class ObjectWrapperCollection;

typedef shared_ptr<ObjectWrapperCollection> ObjectWrapperCollectionPtr;
typedef shared_ptr<const ObjectWrapperCollection> ObjectWrapperCollectionConstPtr;

typedef const shared_ptr<const ObjectWrapperCollection> ConstObjectWrapperCollectionConstPtr;

typedef weak_ptr<ObjectWrapperCollection> ObjectWrapperCollectionWeakPtr;
typedef weak_ptr<const ObjectWrapperCollection> ObjectWrapperCollectionConstWeakPtr;

//! Klasa sluzy do agregowania obiektow domenowych tego samego typu lub pocohdnych od tego samego typu
class ObjectWrapperCollection
{
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

protected:

    ObjectWrapperCollection(const ObjectWrapperCollection & owc) : typeInfo(owc.typeInfo),
        constObjects(owc.constObjects), exact(owc.exact) {}
public:

    //! Chroniony konstruktor, klasa pochodna powinna zdeklarowac przechowywany typ
    //! \param info typ przechowywanych obiektow
    ObjectWrapperCollection(TypeInfo info, bool exact = true) :
      typeInfo(info) {}

    virtual ~ObjectWrapperCollection() {}

public:

    //! \return Najni¿szy typ w hierarchi dziedziczenia który kolekcja mo¿e przechowaæ
    const TypeInfo & getTypeInfo() const
    {
        return typeInfo;
    }

    //! \return Czy dane musza byæ dok³¹dnie tego samego typu dla którego utworzono kolekcjê czy mog¹ te¿ byæ pochodne od niego
    bool exactTypes() const
    {
        return exact;
    }

    //! \param exact Czy dane maj¹ byæ dok³adnie tego typu dla którego utworzono kolekcjê
    //! \param clear Czy czyœciæ dane niezgodne z typem kolekcji - pochodne mu
    void setExactTypes(bool exact, bool clear = false)
    {
        this->exact = exact;

        if(exact == true && clear == true){
            removeDerivedTypes();
        }
    }

    //! Czyœci dane innego typu ni¿ ten dla którego utworozno kolekcjê
    void removeDerivedTypes()
    {
        //iteruj po kolekcji i usun te typy ktore nie sa dokladnie typu kolekcji
        auto cIT = constObjects.begin();
        while( cIT != constObjects.end() ) {
            if((*cIT)->getTypeInfo() != typeInfo){
                cIT = constObjects.erase(cIT);
            }else{
                cIT++;
            }
        }
    }

    //! return Iterator pocz¹tku kolekcji
    iterator begin()
    {
        return constObjects.begin();
    }

    //! return Iterator koñca kolekcji
    iterator end()
    {
        return constObjects.end();
    }

    //! return Const iterator pocz¹tku kolekcji
    const_iterator begin() const
    {
        return constObjects.begin();
    }

    //! return Const iterator koñca kolekcji
    const_iterator end() const
    {
        return constObjects.end();
    }

    //! \return Czy kolekcja jest pusta
    bool empty() const
    {
            return constObjects.empty();
    }

    //! \return Iloœæ elementów w kolekcji
    size_type size() const
    {
        return constObjects.size();
    }

    //! Czyœci wszystkie elementy kolekcji
    void clear()
    {
        constObjects.swap(ConstObjects());
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

    //! \param idx Indeks który usuwamy z kolekcji
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
                throw std::bad_cast("Type of object not equal to type of collection");
            }
        }else if(object->isSupported(typeInfo) == true){
            constObjects.push_back(object);
        } else {
            throw std::bad_cast("Type of object not supported by collection");
        }
    }
};

//! \brief Klasa dostarcza typu, ktorego elementy beda przechowywane
template <class T>
class ObjectWrapperCollectionT : public ObjectWrapperCollection
{
public:
    ObjectWrapperCollectionT() :
      ObjectWrapperCollection(typeid(T))
      {}
};

}

#endif