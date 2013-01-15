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
#include <core/SmartPtr.h>
#include <core/ObjectWrapper.h>

class SubjectService;

class Session : public PluginSubject::ISession
{
    friend class SubjectService;


private:
    PluginSubject::SubjectID sessionID;

    PluginSubject::SubjectID localSessionID;

    PluginSubject::SubjectID currentMotionID;
    PluginSubject::SubjectConstPtr subject;

    std::vector<core::ObjectWrapperConstPtr> wrappers;

    std::string name;
    std::string localName;
	
private:
    Session(PluginSubject::SubjectID sessionID, const PluginSubject::SubjectConstPtr & subject,
		PluginSubject::SubjectID localSessionID, const std::vector<core::ObjectWrapperConstPtr> & wrappers);

public:

	virtual ~Session();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    //const std::string & getName() const;
    virtual void getMotions(PluginSubject::Motions & motions) const;

    virtual const PluginSubject::SubjectConstPtr & getSubject() const;

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const;

    virtual bool hasObjectOfType(const core::TypeInfo& type) const; 
};

class FilteredSession : public PluginSubject::ISession
{
public:

    FilteredSession(const PluginSubject::SessionConstPtr & originalSession, const std::vector<PluginSubject::MotionPtr> & motions, const std::vector<core::ObjectWrapperConstPtr> & wrappers);
    virtual ~FilteredSession();

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    virtual void getMotions(PluginSubject::Motions & motions) const;

    virtual const PluginSubject::SubjectConstPtr & getSubject() const;

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const;

    const PluginSubject::SessionConstPtr & getOriginalSession() const;

private:
    PluginSubject::SessionConstPtr originalSession;
    std::vector<PluginSubject::MotionConstPtr> motions;
    std::vector<core::ObjectWrapperConstPtr> wrappers;
    std::string name;
    std::string localName;
};

#endif
