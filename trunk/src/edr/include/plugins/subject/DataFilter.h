/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   8:07
	filename: 	DataFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__DATAFILTER_H__
#define HEADER_GUARD_SUBJECT__DATAFILTER_H__

#include <boost/date_time.hpp>
#include <plugins/subject/Session.h>
#include <vector>

//! klasa stanowi interfejs dla wszystkich filtrow sesji
class DataFilter
{
public:
    DataFilter() {}
	virtual ~DataFilter() {}
public:
    virtual SessionPtr filterData(SessionConstPtr session) const = 0;
};
typedef boost::shared_ptr<DataFilter> DataFilterPtr;
typedef boost::shared_ptr<const DataFilter> DataFilterConstPtr;


//! klasa zapewnia filtrowanie po typie
class TypeFilter : public DataFilter
{
public:
    TypeFilter(const core::TypeInfo& type) 
    {
        types.push_back(type);
    }
    TypeFilter(const std::vector<core::TypeInfo>& types) :
        types(types)
    {
    }

public:
    virtual SessionPtr filterData(SessionConstPtr session) const
    {
        SessionPtr filtered(new Session());
        SessionPtr s = boost::const_pointer_cast<Session>(session);
        BOOST_FOREACH(MotionPtr motion, s->getMotions()) {
            MotionPtr m(new Motion());
            m->setName(motion->getName());
            BOOST_FOREACH(const core::TypeInfo& type, types) {
                m->addWrappers(motion->getWrappers(type));
            }
            filtered->addMotion(m);
        }

        return filtered;
    }

private:
    std::vector<core::TypeInfo> types;
};
typedef boost::shared_ptr<TypeFilter> TypeFilterPtr;
typedef boost::shared_ptr<const TypeFilter> TypeFilterConstPtr;

//! klasa zapewnia filtrowanie po dacie
class DateFilter : public DataFilter
{
public:
    DateFilter(const boost::gregorian::date& from, const boost::gregorian::date& to) :
      fromTime(from),
      toTime(to) 
      {
      }

public:
    virtual SessionPtr filterData(SessionConstPtr session) const;

private:
    boost::gregorian::date fromTime;
    boost::gregorian::date toTime;
};

//! klasa jest zlozeniem filtrow prostych
class CustomFilter : public DataFilter
{
public:
    CustomFilter(const std::vector<DataFilterPtr>& filters) :
      filters(filters)
      {}

public:
    virtual SessionPtr filterData(SessionConstPtr session) const 
    {
        SessionPtr temp = boost::const_pointer_cast<Session>(session);
        BOOST_FOREACH(DataFilterConstPtr filter, filters) {
            temp = filter->filterData(temp);
        }

        return temp;
    }

private:
    std::vector<DataFilterPtr> filters;
};

class DataFilterCombiner
{
private:
    DataFilterCombiner();
public:
    static SessionPtr combine(const std::vector<SessionPtr>& sessions)
    {
        std::set<core::ObjectWrapperPtr> sessionWrappers;
        std::map<MotionPtr, std::set<core::ObjectWrapperPtr>> motionWrappers;
        std::map<std::string, MotionPtr> motionMap;
        BOOST_FOREACH(SessionPtr session, sessions) {
            Session::WrapperRange sw = session->getWrappers();
            sessionWrappers.insert(sw.begin(), sw.end());
            BOOST_FOREACH(MotionPtr motion, session->getMotions()) {
                MotionPtr combinedMotion;
                auto it = motionMap.find(motion->getName());
                if (it != motionMap.end()) {
                    combinedMotion = it->second;
                } else {
                    combinedMotion = MotionPtr(new Motion);
                    combinedMotion->setName(motion->getName());
                    motionMap[motion->getName()] = combinedMotion;
                }
                Motion::range ww = motion->getWrappers();
                motionWrappers[combinedMotion].insert(ww.begin(), ww.end());
            }
        }

        SessionPtr combinedSession(new Session());
        combinedSession->addWrappers(sessionWrappers);
        for (auto it = motionWrappers.begin(); it != motionWrappers.end(); it++) {
            MotionPtr motion = it->first;
            auto& wrappers = it->second;
            motion->addWrappers(wrappers);
            combinedSession->addMotion(it->first);
        }
        return combinedSession;
    }
};
typedef boost::shared_ptr<DataFilterCombiner> DataFilterCollectorPtr;
typedef boost::shared_ptr<const DataFilterCombiner> DataFilterCollectorConstPtr;

#endif
