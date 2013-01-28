/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   8:07
	filename: 	SubjectDataFilters.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__IDATAFILTER_H__
#define HEADER_GUARD_SUBJECT__IDATAFILTER_H__

#include <corelib/BaseDataTypes.h>
#include <plugins/subject/IDataFilter.h>
#include <plugins/subject/IMotion.h>
#include <plugins/subject/ISession.h>

//! klasa zapewnia filtrowanie po typie
class TypeFilter : public PluginSubject::IDataFilter
{
public:

    TypeFilter(const core::TypeInfo& type);
    TypeFilter(const std::vector<core::TypeInfo>& types);

    virtual ~TypeFilter() {}

protected:
    virtual PluginSubject::SessionPtr doDataFiltering(const PluginSubject::SessionConstPtr & session) const;

private:
    std::vector<core::TypeInfo> types;
};

//! klasa jest zlozeniem filtrów prostych
class CustomFilter : public PluginSubject::IDataFilter
{
public:
    CustomFilter(const std::vector<PluginSubject::DataFilterPtr>& filters);
    virtual ~CustomFilter() {}

public:
    virtual PluginSubject::SessionPtr doDataFiltering(const PluginSubject::SessionConstPtr & session) const;    

private:
    std::vector<PluginSubject::DataFilterPtr> filters;
};


#endif
