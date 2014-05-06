/********************************************************************
    created:  2013/02/07
    created:  7:2:2013   11:24
    filename: BasicDataStorage.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___BASICDATASTORAGE_H__
#define HEADER_GUARD___BASICDATASTORAGE_H__

#include <map>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>
#include <threading/SynchronizationPolicies.h>

class BasicDataStorage
{
private:

	//! Typ mapy obiekt�w.
	typedef std::map< utils::TypeInfo, core::ConstVariantsSet > ObjectsByTypes;
	//! Typ synchronizacji
	typedef utils::RecursiveSyncPolicy SyncPolicy;
	//! Typ lokalnej synchronizacji
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

private:

	//! Obiekty pochodz�ce z parser�w.
	ObjectsByTypes objectsByTypes;
	//! Obiekt synchronizuj�cy
	mutable SyncPolicy sync;

public:
	//! Domy�lny konstruktor
	BasicDataStorage();
	//! Destruktor
	~BasicDataStorage();
	//! \param objects [out] Placeholder na przechowywane i pobierane obiekty
	void getObjects(core::ConstVariantsList & objects) const;
	//! \param objects [out] Obiekty pasuj�ce do zapytania
	//! \param type Typ obniekt�w o kt�re pytamy
	//! \param exact Czy typ musi by� zgodny czy moga to by� tez typy pochodne
	void getObjects(core::ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const;
	//! \param objects [out] Obiekty zarz�dzane przez DM
	void getObjects(core::VariantsCollection& objects) const;

	const bool isManaged(const core::VariantConstPtr & object) const;

	const bool hasObject(const utils::TypeInfo & type, bool exact) const;

	void addData(const core::VariantConstPtr & data);

	void removeData(const core::VariantConstPtr & data);

	const bool tryAddData(const core::VariantConstPtr & data);

	const bool tryRemoveData(const core::VariantConstPtr & data);

private:

	static void requestedTypes(const utils::TypeInfo & type, bool exact, utils::TypeInfoSet & types);
	std::pair<ObjectsByTypes::iterator, core::ConstVariantsSet::iterator> find(const core::VariantConstPtr & data);
};

#endif	//	HEADER_GUARD___BASICDATASTORAGE_H__
