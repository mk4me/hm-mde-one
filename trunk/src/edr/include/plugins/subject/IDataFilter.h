/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:00
    filename: IDataFilter.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAFILTER_H__
#define HEADER_GUARD___IDATAFILTER_H__

#include <plugins/subject/Types.h>
#include <core/SmartPtr.h>
#include <core/PluginCommon.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>

namespace PluginSubject {

class IFilteredDataFactory
{
public:
    virtual ~IFilteredDataFactory() {}

    virtual MotionPtr createFilteredMotion(const MotionConstPtr & originalMotion, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const = 0;
    virtual SessionPtr createFilteredSession(const SessionConstPtr & originalSession, const std::vector<MotionPtr> & motions, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const = 0;
};

//! klasa stanowi interfejs dla wszystkich filtrow sesji
class IDataFilter : protected IFilteredDataFactory
{
public:
    IDataFilter()
    {
        auto subjectService = core::queryServices<ISubjectService>(core::getServiceManager());
        if(subjectService != nullptr){
            filteredDataFactory = subjectService->getFilteredDataFacotry();
        }
    }

    virtual ~IDataFilter() {}
public:
    SessionPtr filterData(const SessionConstPtr & session) const
    {
        if(!filteredDataFactory || !session){
            return SessionPtr();
        }

        return doDataFiltering(session);
    }

    std::vector<SessionPtr> filterData(const std::vector<SessionConstPtr>& sessions) const 
    {
        if(filteredDataFactory == nullptr){
            return std::vector<SessionPtr>();
        }

        std::vector<SessionPtr> ret;
        for (auto it = sessions.begin(); it != sessions.end(); it++) {
            auto filtered = doDataFiltering(*it);
            if (filtered) {
                ret.push_back(filtered);
            }
        }

        return ret;
    }

protected:

    virtual SessionPtr doDataFiltering(const SessionConstPtr & session) const = 0;

    SessionPtr cloneSession(const SessionConstPtr & session) const
    {
        Motions motions;
        session->getMotions(motions);

        std::vector<MotionPtr> newMotions;

        for(auto it = motions.begin(); it != motions.end(); it++){
            newMotions.push_back(cloneMotion(*it));
        }

        std::vector<core::ObjectWrapperConstPtr> objects;
        session->getWrappers(objects);

        return createFilteredSession(session, newMotions, objects);
    }

    MotionPtr cloneMotion(const MotionConstPtr & motion) const
    {
        std::vector<core::ObjectWrapperConstPtr> objects;
        motion->getWrappers(objects);
        return createFilteredMotion(motion, objects);
    }
    
    virtual MotionPtr createFilteredMotion(const MotionConstPtr & originalMotion, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const
    {
        return filteredDataFactory->createFilteredMotion(originalMotion, wrappers);
    }

    virtual SessionPtr createFilteredSession(const SessionConstPtr & originalSession, const std::vector<MotionPtr> & motions, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const
    {
        return filteredDataFactory->createFilteredSession(originalSession, motions, wrappers);
    }

private:

    core::shared_ptr<IFilteredDataFactory> filteredDataFactory;

};

}

#endif HEADER_GUARD___IDATAFILTER_H__