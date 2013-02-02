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
#include <corelib/BaseDataTypes.h>

class SubjectService;

//! Klasa reprezentuje pojedyncza próbę pomiarowa w ramach konkretnej sesji.
class Motion : public PluginSubject::IMotion
{
    friend class SubjectService;

private:
	
    Motion(const core::ObjectWrapperConstPtr & session, const PluginSubject::SessionConstPtr & unpackedSession, PluginSubject::SubjectID localMotionID, const core::ConstObjectsList & wrappers);

	static void generateName(std::string & name, std::string & localName, PluginSubject::SubjectID motionID, PluginSubject::SubjectID motionLocalID, const std::string & sessionLocalName);

	static PluginSubject::SubjectID nextGlobalID();

public:

	virtual ~Motion();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    //const std::string & getName() const;
    virtual const core::ObjectWrapperConstPtr & getSession() const;
	virtual const PluginSubject::SessionConstPtr & getUnpackedSession() const;

    virtual int size() const;
    virtual const core::ObjectWrapperConstPtr & get(int i) const;

    virtual bool isSupported( const core::TypeInfo& typeToCheck ) const;

    virtual void getWrappers(core::ConstObjectsList & wrappers) const;    

private:
	core::ConstObjectsList wrappers;
	std::vector<core::TypeInfo> types;
    PluginSubject::SubjectID motionID;
    PluginSubject::SubjectID localMotionID;

    core::ObjectWrapperConstPtr session;
	PluginSubject::SessionConstPtr unpackedSession;

    std::string name;
    std::string localName;

	static PluginSubject::SubjectID globalID;
};

class FilteredMotion : public PluginSubject::IMotion
{
public:

    FilteredMotion(const core::ObjectWrapperConstPtr & originalMotion, const PluginSubject::MotionConstPtr & originalUnpackedMotion, const core::ConstObjectsList & wrappers);
    virtual ~FilteredMotion();

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    //const std::string & getName() const;
    virtual const core::ObjectWrapperConstPtr & getSession() const;
	virtual const PluginSubject::SessionConstPtr & getUnpackedSession() const;

    virtual int size() const;
    virtual const core::ObjectWrapperConstPtr & get(int i) const;


    void setSession(const core::ObjectWrapperConstPtr & session, const PluginSubject::SessionConstPtr & unpackedSession);

    const core::ObjectWrapperConstPtr & getOriginalMotion() const;
	const PluginSubject::MotionConstPtr & getOriginalUnpackedMotion() const;

private:
    core::ObjectWrapperConstPtr session;
	PluginSubject::SessionConstPtr unpackedSession;
    core::ObjectWrapperConstPtr originalMotion;
	PluginSubject::MotionConstPtr originalUnpackedMotion;
    core::ConstObjectsList wrappers;
};

#endif
