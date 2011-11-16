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
template<class Collection>
class ChannelExtractorFilter : public Sub::IDataFilter
{
public:
    ChannelExtractorFilter() {}
    
    virtual SessionPtr doDataFiltering(const SessionConstPtr & session) const
    {
        typedef core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
        typedef core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;
        typedef typename Collection::ChannelType Channel;
        typedef core::ObjectWrapperT<Channel>::Ptr ChannelPtr;
        typedef core::ObjectWrapperT<Channel>::ConstPtr ChannelConstPtr;
        Motions motions;

        std::vector<MotionPtr> newMotions;

        session->getMotions(motions);
        BOOST_FOREACH(MotionConstPtr motion, motions) {
            if (motion->hasObjectOfType(typeid(Collection))) {
                std::vector<core::ObjectWrapperConstPtr> newMotionObjects;
                //m->setName(motion->getName());
                std::vector<core::ObjectWrapperConstPtr> objects;
                motion->getWrappers(objects, typeid(Collection));
                BOOST_FOREACH(core::ObjectWrapperConstPtr ow, objects) {
                    CollectionConstPtr c = ow->get();
                    for (int i = 0 ; i < c->getNumChannels(); i++) {
                       ChannelConstPtr channel = c->getChannel(i);
                       core::ObjectWrapperPtr cw = core::ObjectWrapper::create<Channel>();
                       cw->set(core::const_pointer_cast<Channel>(channel));
                       cw->setName(channel->getName());
                       cw->setSource(channel->getName());
                       newMotionObjects.push_back(cw);
                    }
                }

                newMotions.push_back(createFilteredMotion(motion, newMotionObjects));
            }
        }

        std::vector<core::ObjectWrapperConstPtr> objects;
        session->getWrappers(objects);

        return createFilteredSession(session, newMotions, objects);
    }

private:
    std::vector<core::TypeInfo> types;
};

class SessionNumberFilter : public Sub::IDataFilter
{
public:
    SessionNumberFilter(int nr) { numbers.push_back(nr); }
    SessionNumberFilter(int nr1, int nr2) { numbers.push_back(nr1); numbers.push_back(nr2); }
    SessionNumberFilter(const std::vector<int>& numbers) : numbers(numbers) { }
    virtual SessionPtr doDataFiltering(const SessionConstPtr & session ) const
    {
        SessionPtr ret;

        bool found = false;
        SubjectID id = session->getLocalID();
        for(auto it = numbers.begin(); it != numbers.end(); it++){
            if(*it == id){
                found = true;
                break;
            }
        }

        if(found == true){
            ret = cloneSession(session);
        }

        return ret;
    }

private:
    std::vector<int> numbers;
};

class NameFilter : public Sub::IDataFilter
{
public:
    NameFilter(const std::string& name) { names.push_back(name); }
    NameFilter(const std::vector<std::string>& names) : names(names) {}

public:
    virtual SessionPtr doDataFiltering( const SessionConstPtr & session ) const
    {
        SessionPtr ret;
        std::vector<MotionPtr> newMotions;
        std::vector<core::ObjectWrapperConstPtr> objects;
        session->getWrappers(objects);
        
        Motions motions;
        session->getMotions(motions);

        for(auto motionIT = motions.begin(); motionIT != motions.end(); motionIT++){
            std::vector<core::ObjectWrapperConstPtr> objects;
            std::vector<core::ObjectWrapperConstPtr> validObjects;
            (*motionIT)->getWrappers(objects);

            for(auto objectIT = objects.begin(); objectIT != objects.end(); objectIT++){
                for (auto it = names.begin(); it != names.end(); it++) {
                    if ((*objectIT)->getName() == *it) {
                        validObjects.push_back(*objectIT);
                        break;
                    }
                }
            }

            if(objects.empty() == true || validObjects.empty() == false){
                newMotions.push_back(createFilteredMotion(*motionIT, validObjects));
            }
        }

        return createFilteredSession(session, newMotions, objects);
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

    DataFilterPtr createCustomEMGFilter(bool post, const std::string& name);
    std::vector<core::ObjectWrapperConstPtr> extractWrappersFromEMG(const std::vector<SessionConstPtr>& sessions);
    
    template<class CollectionPtr>
    DataFilterPtr createCustomV3Filter(bool post, const std::string& name);
    std::vector<core::ObjectWrapperConstPtr> extractWrappersFromVector( const std::vector<SessionConstPtr>& sessions, int scalarIndex );
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
