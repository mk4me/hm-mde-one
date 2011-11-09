/********************************************************************
	created:	2011/11/06
	created:	6:11:2011   14:44
	filename: 	IllnessUnit.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__ILLNESSUNIT_H__
#define HEADER_GUARD_HMM__ILLNESSUNIT_H__

#include "FilterCommand.h"
#include "TreeItemHelper.h"

//! klasa zapewnia filtrowanie po typie
template<class CollectionPtr>
class ChannelExtractorFilter : public DataFilter
{
public:
    ChannelExtractorFilter(core::IMemoryDataManager * memoryDataManager) : DataFilter(memoryDataManager) {}
    
    virtual SessionPtr filterData(SessionConstPtr session) const
    {
        typedef CollectionPtr::element_type Collection;
        typedef Collection::ChannelType Channel;
        typedef Collection::ChannelPtr ChannelPtr;
        std::vector<core::TypeInfo> types; 
        types.push_back(typeid(Channel));
        SessionPtr filtered(new Session());
        SessionPtr s = boost::const_pointer_cast<Session>(session);
        BOOST_FOREACH(MotionPtr motion, s->getMotions()) {
            if (motion->hasObjectOfType(typeid(Collection))) {
                Motion* mm = new Motion(memoryDataManager);
                MotionPtr m(mm);
                m->addType(typeid(Channel));
                m->setName(motion->getName());
                
                BOOST_FOREACH(core::ObjectWrapperPtr ow, motion->getWrappers(typeid(Collection))) {
                    CollectionPtr c = ow->get();
                    for (int i = 0 ; i < c->getNumChannels(); i++) {
                       ChannelPtr channel = c->getChannel(i);
                       core::ObjectWrapperPtr cw = core::ObjectWrapper::create<Channel>();
                       cw->set(channel);
                       cw->setName(channel->getName());
                       cw->setSource(channel->getName());
                       m->addWrapper(cw);
                    }
                }
                filtered->addMotion(m);
            }
        }

        return filtered;
    }

private:
    std::vector<core::TypeInfo> types;
};

class SessionNumberFilter : public DataFilter
{
public:
    SessionNumberFilter(core::IMemoryDataManager* manager, int nr) : DataFilter(manager) { numbers.push_back(nr); }
    SessionNumberFilter(core::IMemoryDataManager* manager, int nr1, int nr2) : DataFilter(manager) { numbers.push_back(nr1); numbers.push_back(nr2); }
    SessionNumberFilter(core::IMemoryDataManager* manager, const std::vector<int>& numbers) : DataFilter(manager), numbers(numbers) { }
    virtual SessionPtr filterData( SessionConstPtr session ) const
    {
        std::string name = session->getName();
        int size = name.size();
        if (size > 2) {
            std::string numberStr = name.substr(size - 2, 2);
            int number = boost::lexical_cast<int>(numberStr);
            for(auto it = numbers.begin(); it != numbers.end(); it++) {
                if (*it == number) {
                    return SessionPtr(new Session(*session));
                }
            }
        }
        return SessionPtr();
    }

private:
    std::vector<int> numbers;
};

class NameFilter : public DataFilter
{
public:
    NameFilter(core::IMemoryDataManager* manager, const std::string& name) : DataFilter(manager) { names.push_back(name); }
    NameFilter(core::IMemoryDataManager* manager, const std::vector<std::string>& names) : DataFilter(manager), names(names) {}

public:
    virtual SessionPtr filterData( SessionConstPtr session ) const
    {
        SessionPtr filtered(new Session());
        SessionPtr s = boost::const_pointer_cast<Session>(session);
        BOOST_FOREACH(MotionPtr motion, s->getMotions()) {
            MotionPtr m(new Motion(memoryDataManager));
            m->setName(motion->getName());
            BOOST_FOREACH(const core::ObjectWrapperPtr wrapper, motion->getWrappers()) {
                for (auto it = names.begin(); it != names.end(); it++) {
                    if (wrapper->getName() == *it) {
                        m->addWrapper(wrapper);
                    }
                }
                
            }
            filtered->addMotion(m);
        }

        return filtered;
    }

private:
    std::vector<std::string> names;

};

class Endo : public IFilterCommand
{
public:
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions );

private:
    void createEMGBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions );
    void createKineticBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions );
    void createAngleBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions );

protected:
    void createVectorEntry(QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions, const QString& name,
        DataFilterPtr leftPrev, DataFilterPtr rightPrev, DataFilterPtr leftPost, DataFilterPtr rightPost, int index);
    void createEMGEntry(QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions, const QString& name,
        DataFilterPtr leftPrev, DataFilterPtr rightPrev, DataFilterPtr leftPost, DataFilterPtr rightPost);

    DataFilterPtr createCustomEMGFilter(core::IMemoryDataManager* manager, bool post, const std::string& name);
    std::vector<core::ObjectWrapperPtr> extractWrappersFromEMG(const std::vector<SessionPtr>& sessions);
    
    template<class CollectionPtr>
    DataFilterPtr createCustomV3Filter(core::IMemoryDataManager* manager, bool post, const std::string& name);
    std::vector<core::ObjectWrapperPtr> extractWrappersFromVector( const std::vector<SessionPtr>& sessions, int scalarIndex );
};


class Stroke : public Endo
{
public:
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions );
};

class Spine : public Endo
{
public:
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const std::vector<SessionConstPtr>& sessions );

private:
    void createEMGBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions );
    void createAngleBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions );
    void createKineticBranch( QTreeWidgetItem* root, const std::vector<SessionConstPtr>& sessions );
};
#endif
