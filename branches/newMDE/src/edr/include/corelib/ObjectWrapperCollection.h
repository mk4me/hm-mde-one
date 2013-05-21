/********************************************************************
	created:	2011/05/23
	created:	23:5:2011   23:14
	filename: 	ObjectWrapperCollection.h
	author:		Wojciech Knieć

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__OBJECTWRAPPERCOLLECTION_H__
#define HEADER_GUARD_CORE__OBJECTWRAPPERCOLLECTION_H__

#include <corelib/Export.h>
#include <vector>
#include <stdexcept>
#include <utils/SmartPtr.h>
#include <corelib/BaseDataTypes.h>

namespace core {

class MemoryDataManager;

//! Klasa służy do agregowania obiektów domenowych tego samego typu lub pocohdnych od tego samego typu
class CORELIB_EXPORT ObjectWrapperCollection
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

private:

	ObjectWrapperCollection() {}

public:

	//template<class T>
	//ObjectWrapperCollection(bool exact = true) : typeInfo(typeid(T)), exact(exact) {}

    //! \param info typ przechowywanych obiektów
    ObjectWrapperCollection(const TypeInfo & typeInfo, bool exact = true);

    ObjectWrapperCollection(const ObjectWrapperCollection & owc);

    virtual ~ObjectWrapperCollection();

public:

    //! \return Najniższy typ w hierarchi dziedziczenia który kolekcja może przechować
    const TypeInfo & getTypeInfo() const;

    //! \return Czy dane musza być dokłądnie tego samego typu dla którego utworzono kolekcję czy mogą też być pochodne od niego
    const bool exactTypes() const;

    //! \param exact Czy dane mają być dokładnie tego typu dla którego utworzono kolekcję
    //! \param clear Czy czyścić dane niezgodne z typem kolekcji - pochodne mu
    void setExactTypes(bool exact);

	iterator begin();

	iterator end();

	const_iterator begin() const;

	const_iterator end() const;

	reverse_iterator rbegin();

	reverse_iterator rend();

	const_reverse_iterator crbegin() const;

	const_reverse_iterator crend() const;

	const bool empty() const;

	const size_type size() const;

	void erase(const const_iterator & it);

	void erase(const const_iterator & itS, const const_iterator & itE);

	void push_front(const ObjectWrapperConstPtr & obj);

	void pop_front();

	void push_back(const ObjectWrapperConstPtr & obj);

	void pop_back();

	reference front();

	const_reference front() const;

	reference back();

	const_reference back() const;

	iterator insert(iterator position, const ObjectWrapperConstPtr& val);

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

	iterator erase(iterator position);

	iterator erase(iterator first, iterator last);

	void swap(ObjectWrapperCollection & owc);

	void clear();

	void remove (const ObjectWrapperConstPtr& val);
	
	template <class Predicate>
	void remove_if (Predicate pred)
	{
		data.remove_if(pred);
	}

	void unique();

	template<class BinaryPredicate>
	void unique (BinaryPredicate binary_pred)
	{
		data.unique(binary_pred);
	}

	void sort();

	template<class Compare>
	void sort (Compare comp)
	{
		data.sort(comp);
	}

	void reverse();

    //! Czyści dane innego typu niż ten dla którego utworozno kolekcję
    void removeDerivedTypes();
};

typedef utils::shared_ptr<ObjectWrapperCollection> ObjectWrapperCollectionPtr;
typedef utils::shared_ptr<const ObjectWrapperCollection> ObjectWrapperCollectionConstPtr;

typedef const utils::shared_ptr<const ObjectWrapperCollection> ConstObjectWrapperCollectionConstPtr;

typedef utils::weak_ptr<ObjectWrapperCollection> ObjectWrapperCollectionWeakPtr;
typedef utils::weak_ptr<const ObjectWrapperCollection> ObjectWrapperCollectionConstWeakPtr;

}

#endif
