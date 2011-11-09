/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   8:07
	filename: 	DataFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__IDATAFILTER_H__
#define HEADER_GUARD_SUBJECT__IDATAFILTER_H__

#include <boost/date_time.hpp>
#include <plugins/subject/Session.h>
#include <vector>

//! klasa stanowi interfejs dla wszystkich filtrow sesji
class DataFilter
{
public:
    DataFilter(core::IMemoryDataManager * memoryDataManager) : memoryDataManager(memoryDataManager) {}
	virtual ~DataFilter() {}
public:
    virtual SessionPtr filterData(SessionConstPtr session) const = 0;

    std::vector<SessionPtr> filterSessions(const std::vector<SessionConstPtr>& sessions) const 
    {
        std::vector<SessionPtr> ret;
        for (auto it = sessions.begin(); it != sessions.end(); it++) {
            auto filtered = filterData(*it);
            if (filtered) {
                ret.push_back(filtered);
            }
        }

        return ret;
    }

protected:
    core::IMemoryDataManager * memoryDataManager;

};
typedef boost::shared_ptr<DataFilter> DataFilterPtr;
typedef boost::shared_ptr<const DataFilter> DataFilterConstPtr;


//! klasa zapewnia filtrowanie po typie
class TypeFilter : public DataFilter
{
public:

    TypeFilter(core::IMemoryDataManager * memoryDataManager, const core::TypeInfo& type) : DataFilter(memoryDataManager)
    {
        types.push_back(type);
    }

    TypeFilter(core::IMemoryDataManager * memoryDataManager, const std::vector<core::TypeInfo>& types) : DataFilter(memoryDataManager),
        types(types)
    {
    }

public:
    virtual SessionPtr filterData(SessionConstPtr session) const
    {
        SessionPtr filtered(new Session());
        SessionPtr s = boost::const_pointer_cast<Session>(session);
        BOOST_FOREACH(MotionPtr motion, s->getMotions()) {
            MotionPtr m(new Motion(memoryDataManager));
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
    DateFilter(core::IMemoryDataManager * memoryDataManager, const boost::gregorian::date& from, const boost::gregorian::date& to) : DataFilter(memoryDataManager),
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
    CustomFilter(core::IMemoryDataManager * memoryDataManager, const std::vector<DataFilterPtr>& filters) : DataFilter(memoryDataManager),
      filters(filters)
      {}

public:
    virtual SessionPtr filterData(SessionConstPtr session) const 
    {
        SessionPtr temp = boost::const_pointer_cast<Session>(session);
        BOOST_FOREACH(DataFilterConstPtr filter, filters) {
            if (temp) {
                temp = filter->filterData(temp);
            }
        }

        return temp;
    }

    

private:
    std::vector<DataFilterPtr> filters;
};


#endif
