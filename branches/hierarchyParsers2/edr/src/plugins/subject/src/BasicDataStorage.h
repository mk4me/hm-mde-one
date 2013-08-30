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
#include <corelib/ObjectWrapperCollection.h>
#include <threading/SynchronizationPolicies.h>

class BasicDataStorage
{
private:

	//! Typ mapy obiekt�w.
	typedef std::map< core::TypeInfo, core::ConstObjects > ObjectsByTypes;

	typedef utils::RecursiveSyncPolicy SyncPolicy;
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

private:

	//! Obiekty pochodz�ce z parser�w.
	ObjectsByTypes objectsByTypes;

	mutable SyncPolicy sync;

public:
	BasicDataStorage();
	~BasicDataStorage();

	void getObjects(core::ConstObjectsList & objects) const;
	//! \param objects [out] Obiekty pasuj�ce do zapytania
	//! \param type Typ obniekt�w o kt�re pytamy
	//! \param exact Czy typ musi by� zgodny czy moga to by� tez typy pochodne
	void getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact) const;

	//! \param objects [out] Obiekty zarz�dzane przez DM
	void getObjects(core::ObjectWrapperCollection& objects) const;

	const bool isManaged(const core::ObjectWrapperConstPtr & object) const;

	const bool hasObject(const core::TypeInfo & type, bool exact) const;

	void addData(const core::ObjectWrapperConstPtr & data);

	void removeData(const core::ObjectWrapperConstPtr & data);

	const bool tryAddData(const core::ObjectWrapperConstPtr & data);

	const bool tryRemoveData(const core::ObjectWrapperConstPtr & data);

private:

	static void requestedTypes(const core::TypeInfo & type, bool exact, core::TypeInfoSet & types);
	std::pair<ObjectsByTypes::iterator, core::ConstObjects::iterator> find(const core::ObjectWrapperConstPtr & data);
};

#endif	//	HEADER_GUARD___BASICDATASTORAGE_H__
