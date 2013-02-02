/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   13:02
	filename: 	Session.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SESSION_H__
#define HEADER_GUARD_SUBJECT__SESSION_H__

#include <plugins/subject/Types.h>
#include <plugins/subject/ISession.h>
#include <vector>
#include <corelib/SmartPtr.h>
#include <corelib/BaseDataTypes.h>

class SubjectService;

class Session : public PluginSubject::ISession
{
    friend class SubjectService;

private:
    PluginSubject::SubjectID sessionID;

    PluginSubject::SubjectID localSessionID;

    mutable PluginSubject::SubjectID currentMotionID;
    core::ObjectWrapperConstPtr subject;
	PluginSubject::SubjectConstPtr unpackedSubject;

    core::ConstObjectsList wrappers;

    std::string name;
    std::string localName;

	static PluginSubject::SubjectID globalID;
	
private:

	static PluginSubject::SubjectID nextGlobalID();

    Session(const core::ObjectWrapperConstPtr & subject,
		const PluginSubject::SubjectConstPtr & unpackedSubject,	PluginSubject::SubjectID localSessionID,
		const core::ConstObjectsList & wrappers);

	static void generateNames(std::string & localName, std::string & globalName, PluginSubject::SubjectID subjectID,
		PluginSubject::SubjectID localSessionID, PluginSubject::SubjectID sessionID);

public:

	virtual ~Session();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    virtual void getMotions(core::ConstObjectsList & motions) const;

    virtual const core::ObjectWrapperConstPtr & getSubject() const;
	virtual const PluginSubject::SubjectConstPtr & getUnpackedSubject() const;

    virtual void getWrappers(core::ConstObjectsList & wrappers) const;

    virtual bool hasObjectOfType(const core::TypeInfo& type) const;

	PluginSubject::SubjectID nextMotionID() const;
};

class FilteredSession : public PluginSubject::ISession
{
public:

    FilteredSession(const core::ObjectWrapperConstPtr & originalSession, const PluginSubject::SessionConstPtr & originalUnpackedSession, const core::ConstObjectsList & motions, const core::ConstObjectsList & wrappers);
    virtual ~FilteredSession();

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    virtual void getMotions(core::ConstObjectsList & motions) const;

    virtual const core::ObjectWrapperConstPtr & getSubject() const;
	virtual const PluginSubject::SubjectConstPtr & getUnpackedSubject() const;

    virtual void getWrappers(core::ConstObjectsList & wrappers) const;

    const core::ObjectWrapperConstPtr & getOriginalSession() const;
	const PluginSubject::SessionConstPtr & getOriginalUnpackedSession() const;

	void setSubject(const core::ObjectWrapperConstPtr & subject, const PluginSubject::SubjectConstPtr & unpackedSubject);

private:
    core::ObjectWrapperConstPtr originalSession;
	PluginSubject::SessionConstPtr originalUnpackedSession;
	core::ObjectWrapperConstPtr subject;
	PluginSubject::SubjectConstPtr unpackedSubject;
    core::ConstObjectsList motions;
    core::ConstObjectsList wrappers;
    std::string name;
    std::string localName;
};

#endif
