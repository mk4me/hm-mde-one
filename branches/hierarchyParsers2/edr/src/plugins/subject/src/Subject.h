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
#include "BasicDataStorage.h"

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

	BasicDataStorage dataStorage;
	BasicDataStorage sessionStorage;

public:

	static void generateName(std::string & name, PluginSubject::SubjectID subjectID);

	static PluginSubject::SubjectID nextGlobalID();

public:
	Subject();

	virtual ~Subject();

    virtual const std::string & getName() const;

    virtual PluginSubject::SubjectID getID() const;

    virtual void getSessions(core::ConstObjectsList & sessions) const;
	virtual void addSession(const core::ObjectWrapperConstPtr & session);
	virtual void removeSession(const core::ObjectWrapperConstPtr & session);

	PluginSubject::SubjectID nextSessionID() const;

	//! \data Dane wchodzące pod kontrolę DM
	virtual void addData(const core::ObjectWrapperConstPtr & data);
	//! Dane usuwane z DM
	virtual void removeData(const core::ObjectWrapperConstPtr & data);

	virtual const bool tryAddData(const core::ObjectWrapperConstPtr & data);

	virtual const bool tryRemoveData(const core::ObjectWrapperConstPtr & data);

	virtual void getObjects(core::ConstObjectsList & objects) const;

	virtual void getObjects(core::ConstObjectsList & objects, const core::TypeInfo & type, bool exact) const;

	virtual void getObjects(core::ObjectWrapperCollection& objects) const;

	virtual const bool isManaged(const core::ObjectWrapperConstPtr & object) const;

	virtual const bool hasObject(const core::TypeInfo & type, bool exact) const;
};

#endif
