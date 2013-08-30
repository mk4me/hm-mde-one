/********************************************************************
	created:	2013/07/21
	created:	21:7:2013   12:53
	filename: 	HierarchyDataManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYDATAMANAGER_H__
#define HEADER_GUARD_CORE__HIERARCHYDATAMANAGER_H__

#include <set>
#include <corelib/IHierarchyItem.h>
#include "MemoryDataManager.h"

namespace core {

class HierarchyDataManager : public MemoryDataManager, public IMemoryDataManagerHierarchy
{
    //! Interfejs obserwatora zmian
    template <class Changes>
    class IChangesObserver
    {
    public:

        virtual ~IChangesObserver() {}

        //! \param previousValue 
        //! \param currentVal 
        //! \param type 
        //! \param modyfication 
        virtual void observe(const Changes & changes) = 0;
    };
public:
	virtual ~HierarchyDataManager() {}

public:
    virtual IMemoryDataManager::HierarchyTransactionPtr hierarchyTransaction();

    virtual void addRoot( IHierarchyItemConstPtr ptr );

    virtual void removeRoot( IHierarchyItemConstPtr ptr );

    virtual void updateRoot( IHierarchyItemConstPtr ptr);
        
    // IDataManagerReader API
    //! \param objectWatcher Obserwator DM do zarejestrowania
    virtual void addObserver(const HierarchyObserverPtr & objectWatcher);
    //! \param objectWatcher Obserwator DM do wyrejestrowania
    virtual void removeObserver(const HierarchyObserverPtr & objectWatcher);
    virtual void updateObservers(const HierarchyChangeList & changes );

    virtual hierarchyConstIterator hierarchyBegin() const;

    virtual hierarchyConstIterator hierarchyEnd() const;

private:
    class HierarchyTransaction;
    friend class HierarchyTransaction;

private:
    std::set<IHierarchyItemConstPtr> roots;
    //! Obiekty obserwuj¹ce stan DM
    std::list<HierarchyObserverPtr> observers;
};
DEFINE_SMART_POINTERS(HierarchyDataManager);
}

#endif
