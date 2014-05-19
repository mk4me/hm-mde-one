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
#include <threadingUtils/SynchronizationPolicies.h>

namespace core {
	class MemoryDataManager : public IDataManagerReader, public IMemoryDataManager
	{
	protected:

		class MemoryTransaction;
		friend class MemoryTransaction;

		class MemoryReaderTransaction;
		friend class MemoryReaderTransaction;

		//! Typ mapy obiekt�w.
		typedef std::map< utils::TypeInfo, VariantsSet > ObjectsByTypes;

		typedef threadingUtils::RecursiveSyncPolicy SyncPolicy;
		typedef threadingUtils::ScopedLock<SyncPolicy> ScopedLock;

	protected:

		//! Obiekty pochodz�ce z parser�w.
		ObjectsByTypes objectsByTypes;

		//! Obiekt na potrzeby synchronizacji
		mutable SyncPolicy sync;

		//! Obiekty obserwuj�ce stan DM
		std::list<ObjectObserverPtr> observers;

	public:

		MemoryDataManager();

		virtual ~MemoryDataManager();

	public:
		// IDataManagerReader API
		//! \param objectWatcher Obserwator DM do zarejestrowania
		virtual void addObserver(const ObjectObserverPtr & objectWatcher);
		//! \param objectWatcher Obserwator DM do wyrejestrowania
		virtual void removeObserver(const ObjectObserverPtr & objectWatcher);

		virtual void getObjects(ConstVariantsList & objects) const;
		//! \param objects [out] Obiekty pasuj�ce do zapytania
		//! \param type Typ obniekt�w o kt�re pytamy
		//! \param exact Czy typ musi by� zgodny czy moga to by� tez typy pochodne
		virtual void getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const;

		//! \param objects [out] Obiekty zarz�dzane przez DM
		virtual void getObjects(VariantsCollection& objects) const;

		virtual const bool isManaged(const VariantConstPtr & object) const;

		virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const;

		virtual IDataManagerReader::TransactionPtr transaction() const;

	public:
		// IMemoryDataManager API

		virtual void addData(const VariantPtr & data);

		virtual void removeData(const VariantConstPtr & data);

		//! Ta metoda notyfikuje o zmianie stanu DM!!
		//! \param Obiekt kt�ry zostanie zmieniony je�li zarz�dza nim DataManager, operacja nie musi by� wykonana od razu
		//! mo�e trafi� do kolejki zada� i by� wykonana p�niej - powinno si� obserwowa� obiekt i wykona� specyficzne
		//! operacje po uzyskaniu notyfikacji o zmianie
		virtual void updateData(const VariantConstPtr & data, const VariantConstPtr & newData);

		virtual const bool tryAddData(const VariantPtr & data);

		virtual const bool tryRemoveData(const VariantConstPtr & data);

		virtual const bool tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData);

		virtual IMemoryDataManager::TransactionPtr transaction();

	protected:

		const bool rawHasObject(const utils::TypeInfo & type, bool exact) const;

		void rawAddData(const VariantPtr & data);

		void rawRemoveData(const VariantConstPtr & data);

		void rawUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData);

		const bool rawIsManaged(const VariantConstPtr & object) const;

		void rawGetObjects(ConstVariantsList & objects) const;

		void rawGetObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const;

		void rawGetObjects(VariantsCollection& objects) const;

		void updateObservers(const ChangeList & changes);

		static void requestedTypes(const utils::TypeInfo & type, bool exact, utils::TypeInfoSet & types);
	};
}

#endif	//	HEADER_GUARD___MEMORYDATAMANAGER_H__
