/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   8:07
	filename: 	SubjectDataFilters.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__IDATAFILTER_H__
#define HEADER_GUARD_SUBJECT__IDATAFILTER_H__

//#include <boost/date_time.hpp>
#include "SubjectDataFilters.h"
#include <core/TypeInfo.h>
#include <plugins/subject/IDataFilter.h>
#include <plugins/subject/IMotion.h>
#include <plugins/subject/ISession.h>



//! klasa zapewnia filtrowanie po typie
class TypeFilter : public Sub::IDataFilter
{
public:

    TypeFilter(const core::TypeInfo& type);
    TypeFilter(const std::vector<core::TypeInfo>& types);

    virtual ~TypeFilter() {}

protected:
    virtual SessionPtr doDataFiltering(const SessionConstPtr & session) const;

private:
    std::vector<core::TypeInfo> types;
};

//typedef boost::shared_ptr<TypeFilter> TypeFilterPtr;
//typedef boost::shared_ptr<const TypeFilter> TypeFilterConstPtr;

////! klasa zapewnia filtrowanie po dacie
//class DateFilter : public Sub::IDataFilter
//{
//public:
//    DateFilter(const boost::gregorian::date& from, const boost::gregorian::date& to) : fromTime(from), toTime(to) 
//    {
//
//    }
//
//public:
//    virtual SessionPtr doDataFiltering(const SessionConstPtr & session) const;
//
//private:
//    boost::gregorian::date fromTime;
//    boost::gregorian::date toTime;
//};

//! klasa jest zlozeniem filtrow prostych
class CustomFilter : public Sub::IDataFilter
{
public:
    CustomFilter(const std::vector<DataFilterPtr>& filters);
    virtual ~CustomFilter() {}

public:
    virtual SessionPtr doDataFiltering(const SessionConstPtr & session) const;    

private:
    std::vector<DataFilterPtr> filters;
};


#endif
