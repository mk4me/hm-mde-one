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


class SubjectService : public plugin::IService, public PluginSubject::ISubjectService
{
	UNIQUE_ID("{F39418DE-4BCB-46C1-8D84-93F435641C63}");
	CLASS_DESCRIPTION("Subject Service", "Subject Service");

public:
	SubjectService();
	virtual ~SubjectService();

//core::IService
public:

	virtual void init(core::ISourceManager * sourceManager,
		core::IVisualizerManager * visualizerManager,
		core::IMemoryDataManager * memoryDataManager,
		core::IStreamDataManager * streamDataManager,
		core::IFileDataManager * fileDataManager);

	//! 
	//! \param actions 
	virtual QWidget* getWidget();
	//! 
	//! \param actions 
	virtual QWidget* getSettingsWidget();
	//! 
	//! \param actions 
	virtual QWidget* getControlWidget();

	virtual const bool lateInit() { return true; }
	virtual void finalize() {}
	virtual void update(double) {}


//ISubjectService
public:

    core::ObjectWrapperPtr createSubject();

    core::ObjectWrapperPtr createSession(const core::ObjectWrapperConstPtr & subject);

    core::ObjectWrapperPtr createMotion(const core::ObjectWrapperConstPtr & session);

private:

    PluginSubject::SubjectID currentSubjectID;
    PluginSubject::SubjectID currentSessionID;
    PluginSubject::SubjectID currentMotionID;
};

#endif //   HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
