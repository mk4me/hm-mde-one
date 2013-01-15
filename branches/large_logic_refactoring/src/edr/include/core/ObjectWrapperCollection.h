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
    friend class MemoryDataManager;

public:
    typedef ConstObjectsList::size_type size_type;
    typedef ConstObjectsList::const_iterator const_iterator;
    typedef ConstObjectsList::iterator iterator;
	typedef ConstObjectsList::reverse_iterator reverse_iterator;
	typedef ConstObjectsList::const_reverse_iterator const_reverse_iterator;
	typedef ConstObjectsList::reference reference;
	typedef ConstObjectsList::const_reference const_reference;

private:
    //! Czy kolekcja przechowyje elementy wylacznie danego typu czy również pochodne mu
    bool exact;
	//! Dane
	ConstObjectsList data;
	//! Typ danych
	TypeInfo typeInfo;

private:

	template<class InputIterator>
	void nonCheckInsert(iterator position, InputIterator first, InputIterator last)
	{
		data.insert(position, first, last);
	}

public:

	template<class T>
	ObjectWrapperCollection(bool exact = true) : typeInfo(typeid(T)), exact(exact) {}

    //! \param info typ przechowywanych obiektów
    ObjectWrapperCollection(const TypeInfo & typeInfo, bool exact = true) : typeInfo(typeInfo), exact(exact) {}

    ObjectWrapperCollection(const ObjectWrapperCollection & owc) : data(owc.data), typeInfo(owc.typeInfo), exact(owc.exact) {}

    virtual ~ObjectWrapperCollection() {}

public:

    //! \return Najniższy typ w hierarchi dziedziczenia który kolekcja może przechować
    const TypeInfo & getTypeInfo() const
	{
		return typeInfo;
	}

    //! \return Czy dane musza być dokłądnie tego samego typu dla którego utworzono kolekcję czy mogą też być pochodne od niego
    const bool exactTypes() const
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

	iterator begin()
	{
		return data.begin();
	}

	iterator end()
	{
		return data.end();
	}

	const_iterator begin() const
	{
		return data.begin();
	}

	const_iterator end() const
	{
		return data.end();
	}

	reverse_iterator rbegin()
	{
		return data.rbegin();
	}

	reverse_iterator rend()
	{
		return data.rend();
	}

	const_reverse_iterator crbegin() const
	{
		return data.crbegin();
	}

	const_reverse_iterator crend() const
	{
		return data.crend();
	}

	const bool empty() const
	{
		return data.empty();
	}

	const size_type size() const
	{
		return data.size();
	}

	void erase(const const_iterator & it)
	{
		data.erase(it);
	}

	void erase(const const_iterator & itS, const const_iterator & itE)
	{
		data.erase(itS, itE);
	}

	void push_front(const ObjectWrapperConstPtr & obj)
	{
		insert(data.begin(), obj);
	}

	void pop_front()
	{
		data.pop_front();
	}

	void push_back(const ObjectWrapperConstPtr & obj)
	{
		insert(data.end(), obj);
	}

	void pop_back()
	{
		data.pop_back();
	}

	reference front()
	{
		return data.front();
	}

	const_reference front() const
	{
		return data.front();
	}

	reference back()
	{
		return data.back();
	}

	const_reference back() const
	{
		return data.back();
	}

	iterator insert(iterator position, const ObjectWrapperConstPtr& val)
	{
		// sprawdzenie poprawności typu
		if (exact == true){
			if(val->getTypeInfo() != getTypeInfo()) {
				// bad_cast nie pobiera parametrów
				//throw std::bad_cast("Type of object not equal to type of collection");
				throw std::bad_cast();
			}
		}else if(val->isSupported(typeInfo) == false){
			//throw std::bad_cast("Type of object not supported by collection");
			throw std::bad_cast();
		}

		return data.insert(position, val);
	}

	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last)
	{
		ConstObjectsList tmp(first, last);

		for(auto it = tmp.begin(); it != tmp.end(); ++it){
			if( (exact == true && (*it)->getTypeInfo() != typeInfo) || (*it)->isSupported(typeInfo) == false ) {
				throw std::bad_cast();
			}
		}

		data.insert(position, tmp.begin(), tmp.end());
	}

	iterator erase(iterator position)
	{
		return data.erase(position);
	}

	iterator erase(iterator first, iterator last)
	{
		return data.erase(first, last);
	}

	void swap(ObjectWrapperCollection & owc)
	{
		data.swap(owc.data);
		std::swap(typeInfo, owc.typeInfo);
	}

	void clear()
	{
		data.clear();
	}

	void remove (const ObjectWrapperConstPtr& val)
	{
		data.remove(val);
	}
	
	template <class Predicate>
	void remove_if (Predicate pred)
	{
		data.remove_if(pred);
	}

	void unique()
	{
		data.unique();
	}

	template<class BinaryPredicate>
	void unique (BinaryPredicate binary_pred)
	{
		data.unique(binary_pred);
	}

	void sort()
	{
		data.sort();
	}

	template<class Compare>
	void sort (Compare comp)
	{
		data.sort(comp);
	}

	void reverse()
	{
		data.reverse();
	}

    //! Czyści dane innego typu niż ten dla którego utworozno kolekcję
    void removeDerivedTypes()
    {
        //iteruj po kolekcji i usun te typy które nie są dokładnie typu kolekcji
        auto cIT = begin();
        while( cIT != end() ) {
            if((*cIT)->getTypeInfo() != typeInfo){
                cIT = data.erase(cIT);
            }else{
                ++cIT;
            }
        }
    }
};

}

#endif
