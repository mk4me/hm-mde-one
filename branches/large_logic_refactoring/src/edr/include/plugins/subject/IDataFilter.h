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
#include <core/IServiceManager.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include <core/PluginCommon.h>

namespace PluginSubject {

class IFilteredDataFactory
{
public:
    virtual ~IFilteredDataFactory() {}

    virtual core::ObjectWrapperPtr createFilteredMotion(const core::ObjectWrapperConstPtr & originalMotion, const core::ConstObjectsList & wrappers) const = 0;
    virtual core::ObjectWrapperPtr createFilteredSession(const core::ObjectWrapperConstPtr & originalSession, const core::ConstObjectsList & motions, const core::ConstObjectsList & wrappers) const = 0;
	virtual core::ObjectWrapperPtr createFilteredSubject(const core::ObjectWrapperConstPtr & originalSubject, const core::ConstObjectsList & sessions) const = 0;
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
    core::ObjectWrapperPtr filterData(const core::ObjectWrapperConstPtr & session) const
    {
        if(!filteredDataFactory || !session){
            return core::ObjectWrapper::create<ISession>();
        }

        return doDataFiltering(session);
    }

    core::ConstObjectsList filterData(const core::ConstObjectsList& sessions) const
    {
        if(filteredDataFactory == nullptr){
            return core::ConstObjectsList();
        }

        core::ConstObjectsList ret;
        for (auto it = sessions.begin(); it != sessions.end(); ++it) {
            auto filtered = doDataFiltering(*it);
            if (filtered) {
                ret.push_back(filtered);
            }
        }

        return ret;
    }

protected:

    virtual core::ObjectWrapperPtr doDataFiltering(const core::ObjectWrapperConstPtr & session) const = 0;

    core::ObjectWrapperPtr cloneSession(const core::ObjectWrapperConstPtr & sessionOW) const
    {
        core::ConstObjectsList motions;
		PluginSubject::SessionConstPtr session = sessionOW->get();
        session->getMotions(motions);

        core::ConstObjectsList newMotions;

        for(auto it = motions.begin(); it != motions.end(); ++it){
            newMotions.push_back(cloneMotion(*it));
        }

        core::ConstObjectsList objects;
        session->getWrappers(objects);

        return createFilteredSession(sessionOW, newMotions, objects);
    }

    core::ObjectWrapperPtr cloneMotion(const core::ObjectWrapperConstPtr & motionOW) const
    {
        core::ConstObjectsList objects;
		PluginSubject::MotionConstPtr motion = motionOW->get();
        motion->getWrappers(objects);
        return createFilteredMotion(motionOW, objects);
    }

    virtual core::ObjectWrapperPtr createFilteredMotion(const core::ObjectWrapperConstPtr & originalMotion, const core::ConstObjectsList & wrappers) const
    {
        return filteredDataFactory->createFilteredMotion(originalMotion, wrappers);
    }

    virtual core::ObjectWrapperPtr createFilteredSession(const core::ObjectWrapperConstPtr & originalSession, const core::ConstObjectsList & motions, const core::ConstObjectsList & wrappers) const
    {
        return filteredDataFactory->createFilteredSession(originalSession, motions, wrappers);
    }

private:

    core::shared_ptr<IFilteredDataFactory> filteredDataFactory;

};

}

#endif HEADER_GUARD___IDATAFILTER_H__
