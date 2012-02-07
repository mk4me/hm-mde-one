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

class SubjectService;

//! Reprezentacja subjecta. Z jednym subjectem zwiazane sa sytemy (np. miesniowy, szkieletowy) oraz jeden ruch (motion)
class Subject : public ISubject
{
    friend class SubjectService;

private:
    SubjectID subjectID;
    std::string name;
    SubjectID currentSessionID;

public:
	Subject(SubjectID subjectID);

	virtual ~Subject();

    virtual const std::string & getName() const;

    virtual SubjectID getID() const;

    virtual void getSessions(Sessions & sessions) const;
};

#endif
