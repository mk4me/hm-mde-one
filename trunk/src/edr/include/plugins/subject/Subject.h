/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   9:34
	filename: 	Subject.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SUBJECT_H__
#define HEADER_GUARD_SUBJECT__SUBJECT_H__

#include <vector>
#include <plugins/subject/ISubjectSystem.h>
#include <plugins/subject/Motion.h>

//! Reprezentacja subjecta. Z jednym subjectem zwiazane sa sytemy (np. miesniowy, szkieletowy) oraz jeden ruch (motion)
class Subject
{
private:
	std::vector<ISubjectSystemPtr> systems;
	MotionConstPtr motion;
public:
	Subject() {}
	Subject(const Subject& obj);
	virtual ~Subject() {}

public:
	void addSystem(ISubjectSystemPtr system);
	void setCurrentMotion(MotionConstPtr motion);

	template<class System>
	System getSystem() const;
};

//template<class System>
//System Subject<System>::getSystem() const
//{
//
//}

typedef core::shared_ptr<Subject> SubjectPtr;
typedef core::shared_ptr<const Subject> SubjectConstPtr;
CORE_DEFINE_WRAPPER(Subject, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

#endif
