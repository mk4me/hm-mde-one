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
typedef weak_ptr<ObjectWrapperCollection> ObjectWrapperCollectionWeakPtr;
typedef weak_ptr<const ObjectWrapperCollection> ObjectWrapperCollectionConstWeakPtr;

//! Klasa sluzy do agregowania obiektow domenowych tego samego typu
class ObjectWrapperCollection
{
public:
    typedef std::vector<ObjectWrapperConstPtr>::const_iterator iterator;
    typedef std::vector<ObjectWrapperConstPtr>::size_type size_type;

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
        auto it = constObjects.begin();
        while( it != constObjects.end() ) {
            if((*it)->getTypeInfo() != typeInfo){
                it = constObjects.erase(it);
            }else{
                it++;
            }
        }
    }

    iterator begin() const
    {
        return constObjects.begin();
    }

    iterator end() const
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

    //! Metoda udostepnia obiekt domenowy z agregatu
    //! \param index indesk pobieranego elementu
    //! \return Niemodyfikowalny obiekt domenowy
    ObjectWrapperConstPtr getObject(int index) const
    {
        // wyjatek zamiast asercji (na wypadek trybu release)
        if (!(index < size() && index >= 0)) {
            throw std::runtime_error("ObjectWrapperCollection::getObject - wrong index");
        }

        return constObjects[index];
    }
   
    //! Dodanie obiektu do agregatu
    //! \param object Wskaznik do niemodyfikowalnego obiektu domenowego
    void addObject(ObjectWrapperConstPtr object) 
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
            throw std::bad_cast("Type of object not not supported by collection");
        }
    }

   /* ObjectWrapperPtr replaceObject(const ObjectWrapperConstPtr & object)
    {
        auto it = std::find(constObjects.begin(), constObjects.end(), object);

        if(it == constObjects.end()){
            throw std::runtime_error("Object does not belong to this collection");
        }

        ObjectWrapperPtr clone = (*it)->clone();

        *it = clone;
        
        return clone;
    }*/

    ObjectWrapperPtr replaceObject(int index)
    {
        // wyjatek zamiast asercji (na wypadek trybu release)
        if (!(index < size() && index >= 0)) {
            throw std::runtime_error("ObjectWrapperCollection::getObject - wrong index");
        }

        ObjectWrapperConstPtr obj = constObjects[index];
        ObjectWrapperPtr clone = obj->clone();
        constObjects[index] = clone;
        return clone;
    }

    //! wyczyszczenie kolekcji
    void clear() 
    {
        constObjects.swap(ConstObjects());
    }

    //! \return kopia calej kolekcji
    virtual ObjectWrapperCollectionPtr clone() const
    {
        return ObjectWrapperCollectionPtr(new ObjectWrapperCollection(*this));
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