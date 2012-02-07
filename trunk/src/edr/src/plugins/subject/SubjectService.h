/********************************************************************
	created:	2011/07/21
	created:	21:7:2011   18:43
	filename: 	SubjectService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
#define HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__

#include <OpenThreads/Mutex>

#include <core/IDataManager.h>
#include <core/IService.h>
#include <core/PluginCommon.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/IDataFilter.h>

class FilteredDataFacory : public Sub::IFilteredDataFactory
{
public:
    FilteredDataFacory();
    virtual ~FilteredDataFacory();

    virtual MotionPtr createFilteredMotion(const MotionConstPtr & originalMotion, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const;
    virtual SessionPtr createFilteredSession(const SessionConstPtr & originalSession, const std::vector<MotionPtr> & motions, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const;
};



class SubjectService : public core::IService, public ISubjectService
{
	UNIQUE_ID("{F39418DE-4BCB-46C1-8D84-93F435641C63}", "Subject Service");

public:
	SubjectService();
	virtual ~SubjectService();

//core::IService
public:

    virtual void init(core::IManagersAccessor * managersAccessor);

    //virtual void createOrUpdateSession(int year, int month, int day, int body, int session, int motion, const core::Objects & objects);

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

    virtual SubjectPtr createSubject();

    virtual SessionPtr createSession(const SubjectConstPtr & subject, unsigned int year,
        unsigned char month, unsigned char day, const std::vector<core::ObjectWrapperConstPtr> & wrappers);

    virtual MotionPtr createMotion(const SessionConstPtr & session,
        const std::vector<core::ObjectWrapperConstPtr> & wrappers);

private:

    virtual const FilteredDataFacoryPtr & getFilteredDataFacotry() const;

private:

    FilteredDataFacoryPtr filteredDataFactory;

    core::IMemoryDataManager * memoryDataManager;

    OpenThreads::Mutex subjectCreationMutex;
    OpenThreads::Mutex sessionCreationMutex;
    OpenThreads::Mutex motionCreationMutex;

    SubjectID currentSubjectID;
    SubjectID currentSessionID;
    SubjectID currentMotionID;

    std::map<SubjectConstPtr, SubjectID> localSessionIDs;
    std::map<SessionConstPtr, SubjectID> localMotionIDs;
};

#endif //   HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
