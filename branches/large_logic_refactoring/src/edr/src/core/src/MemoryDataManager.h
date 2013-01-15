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
#include <core/IDataManagerReader.h>
#include <core/IMemoryDataManager.h>
#include <utils/SynchronizationPolicies.h>

namespace core {

class MemoryDataManager : public plugin::IDataManagerReader, public plugin::IMemoryDataManager
{
private:

	class MemoryTransaction;
	friend class MemoryTransaction;

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

	//! \param objects [out] Obiekty pasuj�ce do zapytania
	//! \param type Typ obniekt�w o kt�re pytamy
	//! \param exact Czy typ musi by� zgodny czy moga to by� tez typy pochodne
	virtual void getObjects(ConstObjectsList & objects, const TypeInfo & type, bool exact);
	
	//! \param objects [out] Obiekty zarz�dzane przez DM
	virtual void getObjects(ObjectWrapperCollection& objects);

	virtual const bool isManaged(const ObjectWrapperConstPtr & object) const;

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

	virtual const TransactionPtr transaction();

private:

	void rawAddData(const ObjectWrapperPtr & data);

	void rawRemoveData(const ObjectWrapperConstPtr & data);

	void rawUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData);

	const bool rawIsManaged(const ObjectWrapperConstPtr & object) const;

	void updateObservers(const ChangeList & changes );

};

}

#endif	//	HEADER_GUARD___MEMORYDATAMANAGER_H__
