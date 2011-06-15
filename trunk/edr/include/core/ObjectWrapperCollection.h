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

//! Klasa sluzy do agregowania obiektow domenowych tego samego typu
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

    const TypeInfo & getTypeInfo() const
    {
        return typeInfo;
    }


    bool exactTypes() const
    {
        return exact;
    }

    void setExactTypes(bool exact, bool clear = false)
    {
        this->exact = exact;

        if(exact == true && clear == true){
            removeDerivedTypes();
        }
    }

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

    iterator begin()
    {
        return constObjects.begin();
    }

    iterator end()
    {
        return constObjects.end();
    }

    const_iterator begin() const
    {
        return constObjects.begin();
    }

    const_iterator end() const
    {
        return constObjects.end();
    }

    bool empty() const
    {
            return constObjects.empty();
    }

    size_type size() const
    {
        return constObjects.size();
    }

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
        if (!(index < size() && index >= 0)) {
            throw std::runtime_error("ObjectWrapperCollection::getObject - wrong index");
        }

        return constObjects[index];
    }

    template<class T>
    const T & getObject(int index) const
    {
        return getObject(int)->get(exact);
    }

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
        if(std::find(constObjects.begin(), constObjects.end(), object) != constObjects.end()){
            throw std::runtime_error("Object already in collection");
        }

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