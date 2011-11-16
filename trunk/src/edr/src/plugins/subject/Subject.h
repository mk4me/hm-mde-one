/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   9:34
	filename: 	Subject.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SUBJECT_H__
#define HEADER_GUARD_SUBJECT__SUBJECT_H__

#include <plugins/subject/ISubject.h>
#include <core/IDataManager.h>
//#include <plugins/subject/ISubjectSystem.h>
//#include <plugins/subject/Motion.h>

class SubjectService;

//! Reprezentacja subjecta. Z jednym subjectem zwiazane sa sytemy (np. miesniowy, szkieletowy) oraz jeden ruch (motion)
class Subject : public ISubject
{
    friend class SubjectService;

private:
    SubjectID subjectID;
    std::string name;
    SubjectID currentSessionID;

    //std::string name;
	//std::vector<ISubjectSystemPtr> systems;
	//MotionConstPtr motion;
	// TODO - subject powinien agregowac wszystkie sesje z nim zwiazane
    core::IMemoryDataManager * memoryDataManager;

public:
	Subject(core::IMemoryDataManager * memoryDataManager, SubjectID subjectID);

	//Subject(const Subject& obj);
	virtual ~Subject();

    virtual const std::string & getName() const;

    virtual SubjectID getID() const;
    //const std::string & getName() const;
	//void addSystem(ISubjectSystemPtr system);
	//void setCurrentMotion(MotionConstPtr motion);

    virtual void getSessions(Sessions & sessions) const;

    //ISystemPtr getConfiguredSystem(ISystemPtr system, ObjectWrapperPtr data) const;

	/*template<class System>
	System getSystem() const;*/
};


//CORE_DEFINE_WRAPPER(Subject, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);


#endif
