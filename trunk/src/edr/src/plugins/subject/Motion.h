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
#include <core/ObjectWrapper.h>
#include <core/IDataManager.h>

#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/video/Wrappers.h>
#include <core/PluginCommon.h>
#include <core/DataAccessors.h>
#include <core/SmartPtr.h>
#include <core/TypeInfo.h>
//#include <boost/range.hpp>

class SubjectService;

//! Klasa reprezentuje pojedyncza probe pomiarowa w ramach konkretnej sesji.
class Motion : public PluginSubject::IMotion
{
    friend class SubjectService;

private:
	
    Motion(PluginSubject::SubjectID motionID, const PluginSubject::SessionConstPtr & session, PluginSubject::SubjectID localMotionID, const std::vector<core::ObjectWrapperConstPtr> & wrappers);

public:

	virtual ~Motion();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    //const std::string & getName() const;
    const PluginSubject::SessionConstPtr & getSession() const;

    virtual int size() const;
    virtual const core::ObjectWrapperConstPtr & get(int i) const;

    virtual bool isSupported( const core::TypeInfo& typeToCheck ) const;

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const;    

private:
	std::vector<core::ObjectWrapperConstPtr> wrappers;
	std::vector<core::TypeInfo> types;
	//std::string name;
    PluginSubject::SubjectID motionID;
    PluginSubject::SubjectID localMotionID;

    PluginSubject::SessionConstPtr session;

    core::IMemoryDataManager * memoryDataManager;
    std::string name;
    std::string localName;
};

class FilteredMotion : public PluginSubject::IMotion
{
public:

    FilteredMotion(const PluginSubject::MotionConstPtr & originalMotion, const std::vector<core::ObjectWrapperConstPtr> & wrappers);
    virtual ~FilteredMotion();

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;
    //const std::string & getName() const;
    virtual const PluginSubject::SessionConstPtr & getSession() const;

    virtual int size() const;
    virtual const core::ObjectWrapperConstPtr & get(int i) const;


    void setSession(const PluginSubject::SessionConstPtr & session);

    const PluginSubject::MotionConstPtr & getOriginalMotion() const;

private:
    PluginSubject::SessionConstPtr session;
    PluginSubject::MotionConstPtr originalMotion;
    std::vector<core::ObjectWrapperConstPtr> wrappers;
};

#endif
