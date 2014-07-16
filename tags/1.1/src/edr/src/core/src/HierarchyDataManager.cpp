#include "CorePCH.h"
#include "HierarchyDataManager.h"

using namespace core;

class HierarchyDataManager::HierarchyTransaction : public IMemoryDataManager::IMemoryDataHierarchyTransaction
{
private:
    HierarchyDataManager* mdm;
    HierarchyChangeList modifications;
    bool transactionRollbacked;

public:
    HierarchyTransaction(HierarchyDataManager * mdm) : mdm(mdm), transactionRollbacked(false)
    {
        mdm->sync.lock();
    }

    ~HierarchyTransaction()
    {
        if(transactionRollbacked == false){
            if(modifications.empty() == false){
                mdm->updateObservers(modifications);
            }
            mdm->sync.unlock();
        }
    }

public:

    virtual const bool isRolledback() const
    {
        return transactionRollbacked;
    }

    virtual void rollback()
    {
        transactionRollbacked = true;

        // TODO
        // W obecnym modelu zarzadzania updateami nie można  ich sensownie cofnąć 

        ////cofanie pozosta�ych zmian od ko�ca
        for(auto it = modifications.rbegin(); it != modifications.rend(); ++it){
            switch((*it).modification){

            case IDataManagerReader::ADD_OBJECT:
                mdm->removeRoot((*it).value);
                break;

            case IDataManagerReader::REMOVE_OBJECT:
                mdm->addRoot(utils::const_pointer_cast<IHierarchyItem>((*it).value));
                break;
            }
        }

        mdm->sync.unlock();
    }

    virtual void updateRoot( IHierarchyItemConstPtr ptr )
    {
        HierarchyChange hc;
        hc.value = ptr;
        hc.modification = IDataManagerReader::UPDATE_OBJECT;
        modifications.push_back(hc);
    }

    virtual void addRoot( IHierarchyItemConstPtr ptr )
    {
        HierarchyChange hc;
        hc.value = ptr;
        hc.modification = IDataManagerReader::ADD_OBJECT;
        modifications.push_back(hc);
        mdm->addRoot(ptr);
    }

    virtual void removeRoot( IHierarchyItemConstPtr ptr )
    {
        HierarchyChange hc;
        hc.value = ptr;
        hc.modification = IDataManagerReader::REMOVE_OBJECT;
        modifications.push_back(hc);
        mdm->removeRoot(ptr);
    }

};


IMemoryDataManager::HierarchyTransactionPtr core::HierarchyDataManager::hierarchyTransaction()
{
    return IMemoryDataManager::HierarchyTransactionPtr(new HierarchyTransaction(this));
}

void core::HierarchyDataManager::addRoot( IHierarchyItemConstPtr ptr )
{
    if (roots.find(ptr) != roots.end()) {
        throw std::runtime_error("HierarchyDataManager - root is already added");
    }

    roots.insert(ptr);
}

void core::HierarchyDataManager::removeRoot( IHierarchyItemConstPtr ptr )
{
    auto it = roots.find(ptr);
    if (it == roots.end()) {
        throw std::runtime_error("HierarchyDataManager - root was not found");
    }

    roots.erase(it);
    
}

void core::HierarchyDataManager::updateRoot( IHierarchyItemConstPtr ptr )
{
    UTILS_ASSERT(false, "NYI");
}

void core::HierarchyDataManager::addObserver( const HierarchyObserverPtr & objectWatcher )
{
    ScopedLock lock(sync);
    if(std::find(observers.begin(), observers.end(), objectWatcher) != observers.end()){
        throw std::runtime_error("Watcher already registered");
    }

    observers.push_back(objectWatcher);
}

void core::HierarchyDataManager::removeObserver( const HierarchyObserverPtr & objectWatcher )
{
    ScopedLock lock(sync);
    auto it = std::find(observers.begin(), observers.end(), objectWatcher);
    if(it == observers.end()){
        throw std::runtime_error("Watcher not registered");
    }

    observers.erase(it);
}

void core::HierarchyDataManager::updateObservers(const HierarchyChangeList & changes )
{
    for(auto it = observers.begin(); it != observers.end(); ++it){
        try{
            (*it)->observe(changes);
        }catch(...){
            //TODO
            //rozwin�� obserwator�w aby si� jako� identyfikowali!! ewentualnie robi� to przez w�asn� implementacj� dostarczan� konretnym obiektom
            //(osobne interfejsy reader�w dla ka�dego elemnentu �adowanego do aplikacji - service, source, datasink, itp)
            CORE_LOG_WARNING("Error while updating memory data manager observer");
        }
    }
}

core::HierarchyDataManager::hierarchyConstIterator core::HierarchyDataManager::hierarchyBegin() const
{
    return roots.cbegin();
}

core::HierarchyDataManager::hierarchyConstIterator core::HierarchyDataManager::hierarchyEnd() const
{
    return roots.cend();
}


