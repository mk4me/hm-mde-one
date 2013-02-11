/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   22:31
    filename: MemoryDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MEMORYDATAMANAGER_H__
#define HEADER_GUARD___MEMORYDATAMANAGER_H__

#include <list>
#include <corelib/IDataManagerReader.h>
#include <corelib/IMemoryDataManager.h>
#include <utils/SynchronizationPolicies.h>

namespace core {

class MemoryDataManager : public IDataManagerReader, public IMemoryDataManager
{
private:

	class MemoryTransaction;
	friend class MemoryTransaction;

	class MemoryReaderTransaction;
	friend class MemoryReaderTransaction;

	//! Typ mapy obiekt�w.
	typedef std::map< TypeInfo, Objects > ObjectsByTypes;

	typedef utils::RecursiveSyncPolicy SyncPolicy;
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

private:

	//! Obiekty pochodz�ce z parser�w.
	ObjectsByTypes objectsByTypes;

	//! Obiekt na potrzeby synchronizacji
	mutable SyncPolicy sync;

	//! Obiekty obserwuj�ce stan DM
	std::list<ObjectObserverPtr> observers;

public:
	// IDataManagerReader API
	//! \param objectWatcher Obserwator DM do zarejestrowania
	virtual void addObserver(const ObjectObserverPtr & objectWatcher);
	//! \param objectWatcher Obserwator DM do wyrejestrowania
	virtual void removeObserver(const ObjectObserverPtr & objectWatcher);


	virtual void getObjects(core::ConstObjectsList & objects) const;
	//! \param objects [out] Obiekty pasuj�ce do zapytania
	//! \param type Typ obniekt�w o kt�re pytamy
	//! \param exact Czy typ musi by� zgodny czy moga to by� tez typy pochodne
	virtual void getObjects(ConstObjectsList & objects, const TypeInfo & type, bool exact) const;
	
	//! \param objects [out] Obiekty zarz�dzane przez DM
	virtual void getObjects(ObjectWrapperCollection& objects) const;

	virtual const bool isManaged(const ObjectWrapperConstPtr & object) const;

	virtual const bool hasObject(const TypeInfo & type, bool exact) const;

	virtual IDataManagerReader::TransactionPtr transaction() const;

public:
	// IMemoryDataManager API

	virtual void addData(const ObjectWrapperPtr & data);

	virtual void removeData(const ObjectWrapperConstPtr & data);

	//! Ta metoda notyfikuje o zmianie stanu DM!!
	//! \param Obiekt kt�ry zostanie zmieniony je�li zarz�dza nim DataManager, operacja nie musi by� wykonana od razu
	//! mo�e trafi� do kolejki zada� i by� wykonana p�niej - powinno si� obserwowa� obiekt i wykona� specyficzne
	//! operacje po uzyskaniu notyfikacji o zmianie
	virtual void updateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData);

	virtual const bool tryAddData(const ObjectWrapperPtr & data);

	virtual const bool tryRemoveData(const ObjectWrapperConstPtr & data);

	virtual const bool tryUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData);

	virtual IMemoryDataManager::TransactionPtr transaction();

private:	

	const bool rawHasObject(const TypeInfo & type, bool exact) const;

	void rawAddData(const ObjectWrapperPtr & data);

	void rawRemoveData(const ObjectWrapperConstPtr & data);

	void rawUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData);

	const bool rawIsManaged(const ObjectWrapperConstPtr & object) const;

	void rawGetObjects(core::ConstObjectsList & objects) const;

	void rawGetObjects(ConstObjectsList & objects, const TypeInfo & type, bool exact) const;

	void rawGetObjects(ObjectWrapperCollection& objects) const;	

	void updateObservers(const ChangeList & changes );

	static void requestedTypes(const TypeInfo & type, bool exact, TypeInfoSet & types);

};

}

#endif	//	HEADER_GUARD___MEMORYDATAMANAGER_H__
