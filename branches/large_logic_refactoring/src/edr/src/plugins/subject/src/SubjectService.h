/********************************************************************
	created:	2011/07/21
	created:	21:7:2011   18:43
	filename: 	SubjectService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
#define HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__

#include <corelib/IService.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/IDataFilter.h>

class FilteredDataFacory : public PluginSubject::IFilteredDataFactory
{
public:
    FilteredDataFacory();
    virtual ~FilteredDataFacory();

    virtual core::ObjectWrapperPtr createFilteredMotion(const core::ObjectWrapperConstPtr & originalMotion, const core::ConstObjectsList & wrappers) const;
    virtual core::ObjectWrapperPtr createFilteredSession(const core::ObjectWrapperConstPtr & originalSession, const core::ConstObjectsList & motions, const core::ConstObjectsList & wrappers) const;
	virtual core::ObjectWrapperPtr createFilteredSubject(const core::ObjectWrapperConstPtr & originalSubject, const core::ConstObjectsList & sessions) const;
};



class SubjectService : public core::IService, public PluginSubject::ISubjectService
{
	UNIQUE_ID("{F39418DE-4BCB-46C1-8D84-93F435641C63}", "Subject Service");

public:
	SubjectService();
	virtual ~SubjectService();

//core::IService
public:

	virtual void init(plugin::ISourceManager * sourceManager,
		plugin::IVisualizerManager * visualizerManager,
		plugin::IMemoryDataManager * memoryDataManager,
		plugin::IFileDataManager * fileDataManager,
		plugin::IActionsGroupManager * actionsManager);

	//! 
	//! \param actions 
	virtual QWidget* getWidget( core::IActionsGroupManager * actionsManager );
	//! 
	//! \param actions 
	virtual QWidget* getSettingsWidget( core::IActionsGroupManager * actionsManager );
	//! 
	virtual const std::string& getName() const;
	//! 
	//! \param actions 
	virtual QWidget* getControlWidget( core::IActionsGroupManager * actionsManager );

//ISubjectService
public:

    core::ObjectWrapperPtr createSubject();

    core::ObjectWrapperPtr createSession(const core::ObjectWrapperConstPtr & subject, const core::ConstObjectsList & wrappers);

    core::ObjectWrapperPtr createMotion(const core::ObjectWrapperConstPtr & session,
        const core::ConstObjectsList & wrappers);

private:

    virtual const PluginSubject::FilteredDataFacoryPtr & getFilteredDataFacotry() const;

private:

    PluginSubject::FilteredDataFacoryPtr filteredDataFactory;

    PluginSubject::SubjectID currentSubjectID;
    PluginSubject::SubjectID currentSessionID;
    PluginSubject::SubjectID currentMotionID;
};

#endif //   HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
