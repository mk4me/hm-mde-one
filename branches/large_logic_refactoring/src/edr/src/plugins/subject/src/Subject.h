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

class SubjectService;

//! Reprezentacja subjecta. Z jednym subjectem związane są sytemy (np. miesniowy, szkieletowy) oraz jeden ruch (motion)
class Subject : public PluginSubject::ISubject
{
    friend class SubjectService;

private:
    PluginSubject::SubjectID subjectID;
    std::string name;
    mutable PluginSubject::SubjectID currentSessionID;
	static PluginSubject::SubjectID globalID;

protected:

	static void generateName(std::string & name, PluginSubject::SubjectID subjectID);

	static PluginSubject::SubjectID nextGlobalID();

public:
	Subject();

	virtual ~Subject();

    virtual const std::string & getName() const;

    virtual PluginSubject::SubjectID getID() const;

    virtual void getSessions(core::ConstObjectsList & sessions) const;

	PluginSubject::SubjectID nextSessionID() const;
};

class FilteredSubject : public PluginSubject::ISubject
{
public:

	FilteredSubject(const core::ObjectWrapperPtr & originalSubject, const PluginSubject::SubjectConstPtr & originalUnpackedSubject, const core::ConstObjectsList & sessions);
	virtual ~FilteredSubject();

	virtual const std::string & getName() const;

	virtual PluginSubject::SubjectID getID() const;

	virtual void getSessions(core::ConstObjectsList & sessions) const;

private:
	core::ObjectWrapperPtr originalSubject;
	PluginSubject::SubjectConstPtr originalUnpackedSubject;
	core::ConstObjectsList sessions;
};

#endif
