/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   8:07
	filename: 	SubjectDataFilters.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__IDATAFILTER_H__
#define HEADER_GUARD_SUBJECT__IDATAFILTER_H__

#include <plugins/subject/Export.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/subject/Types.h>

class ISubjectHierarchyFilter;
typedef core::shared_ptr<ISubjectHierarchyFilter> SubjectHierarchyFilterPtr;
typedef core::shared_ptr<const ISubjectHierarchyFilter> SubjectHierarchyFilterConstPtr;

class ISubjectHierarchyFilter
{
public:
	virtual ~ISubjectHierarchyFilter() {}

	virtual void filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const = 0;
	virtual void filterSubjectData(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectData) const = 0;
	virtual void filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const = 0;

	virtual void filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const = 0;
	virtual void filterSessionData(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionData) const = 0;
	virtual void filterSessionMotions(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionMotions) const = 0;

	virtual void filterMotions(const core::ConstObjectsList & inputMotion, core::ConstObjectsList & outputMotions) const = 0;
	virtual void filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const = 0;	
};

class PLUGINSUBJECT_EXPORT SubjectHierarchyEmptyFilter : public ISubjectHierarchyFilter
{
public:

	SubjectHierarchyEmptyFilter();

	virtual ~SubjectHierarchyEmptyFilter();

	virtual void filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const;

	virtual void filterSubjectData(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectData) const;

	virtual void filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const;

	virtual void filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const;

	virtual void filterSessionData(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionData) const;

	virtual void filterSessionMotions(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionMotions) const;

	virtual void filterMotions(const core::ConstObjectsList & inputMotion, core::ConstObjectsList & outputMotions) const;

	virtual void filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const;
};

class PLUGINSUBJECT_EXPORT SubjectHierarchyCompoundFilter : public ISubjectHierarchyFilter
{
public:
	typedef std::list<SubjectHierarchyFilterPtr> Filters;

public:

	SubjectHierarchyCompoundFilter(const Filters & filters);

	virtual ~SubjectHierarchyCompoundFilter();

	virtual void filterSubjects(const core::ConstObjectsList & inputSubjects, core::ConstObjectsList & outputSubjects) const;

	virtual void filterSubjectData(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectData) const;

	virtual void filterSubjectSessions(const core::ObjectWrapperConstPtr & subject, core::ConstObjectsList & outputSubjectSessions) const;

	virtual void filterSessions(const core::ConstObjectsList & inputSessions, core::ConstObjectsList & outputSessions) const;

	virtual void filterSessionData(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionData) const;

	virtual void filterSessionMotions(const core::ObjectWrapperConstPtr & session, core::ConstObjectsList & outputSessionMotions) const;

	virtual void filterMotions(const core::ConstObjectsList & inputMotion, core::ConstObjectsList & outputMotions) const;

	virtual void filterMotionData(const core::ObjectWrapperConstPtr & motion, core::ConstObjectsList & outputMotionData) const;

private:

	Filters filters;
};

#endif
