/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   10:52
	filename: 	Motion.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__MOTION_TESTETESTEH__
#define HEADER_GUARD_SUBJECT__MOTION_TESTETESTEH__

#include <plugins/subject/IMotion.h>
#include <corelib/BaseDataTypes.h>
#include "BasicDataStorage.h"

class SubjectService;

//! Klasa reprezentuje pojedyncza próbę pomiarowa w ramach konkretnej sesji.
class Motion : public PluginSubject::IMotion
{
public:

	Motion(const core::ObjectWrapperConstPtr & session, const PluginSubject::SessionConstPtr & unpackedSession, PluginSubject::SubjectID localMotionID);

	virtual ~Motion();

	static std::string generateName(PluginSubject::SubjectID motionID);
	static std::string generateLocalName(PluginSubject::SubjectID motionLocalID, const std::string & sessionLocalName);

	static PluginSubject::SubjectID nextGlobalID();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;

    virtual const core::ObjectWrapperConstPtr & getSession() const;
	virtual const PluginSubject::SessionConstPtr & getUnpackedSession() const;

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

private:

	BasicDataStorage storage;

    const PluginSubject::SubjectID motionID;
    const PluginSubject::SubjectID localMotionID;

    const core::ObjectWrapperConstPtr session;
	const PluginSubject::SessionConstPtr unpackedSession;

    const std::string name;
    const std::string localName;

	static PluginSubject::SubjectID globalID;
};

#endif
