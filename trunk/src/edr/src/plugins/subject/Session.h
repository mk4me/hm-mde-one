/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   13:02
	filename: 	Session.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SESSION_H__
#define HEADER_GUARD_SUBJECT__SESSION_H__

#include <core/IDataManager.h>
#include <plugins/subject/Types.h>
#include <plugins/subject/ISession.h>

#include <vector>
#include <core/SmartPtr.h>
#include <core/ObjectWrapper.h>

//#include <plugins/Subject/Motion.h>
//#include <boost/foreach.hpp>

//struct AntropometricData
//{
//	double width;
//};
//typedef core::shared_ptr<AntropometricData> AntropometricDataPtr;
//typedef core::shared_ptr<const AntropometricData> AntropometricDataConstPtr;
//
//struct ExaminationData
//{
//
//};
//typedef core::shared_ptr<ExaminationData> ExaminationDataPtr;
//typedef core::shared_ptr<const ExaminationData> ExaminationDataConstPtr;
//
//struct PersonalData
//{
//	std::string name;
//	std::string surname;
//};
//typedef core::shared_ptr<PersonalData> PersonalDataPtr;
//typedef core::shared_ptr<const PersonalData> PersonalDataConstPtr;

class SubjectService;

class Session : public ISession
{
    friend class SubjectService;


private:
	//std::string name;
    SubjectID sessionID;

    SubjectID localSessionID;

    SubjectID currentMotionID;
    SubjectConstPtr subject;

    unsigned int year;
    unsigned char month;
    unsigned char day;

    std::vector<core::ObjectWrapperConstPtr> wrappers;

    core::IMemoryDataManager * memoryDataManager;

    std::string name;
    std::string localName;
	
private:
    Session(core::IMemoryDataManager * memoryDataManager, SubjectID sessionID, const SubjectConstPtr & subject, SubjectID localSessionID, unsigned int year,
        unsigned char month, unsigned char day, const std::vector<core::ObjectWrapperConstPtr> & wrappers);

public:

	virtual ~Session();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual SubjectID getID() const;
    virtual SubjectID getLocalID() const;
    //const std::string & getName() const;
    virtual void getMotions(Motions & motions) const;

    virtual const SubjectConstPtr & getSubject() const;

    virtual unsigned int getYear() const;
    virtual unsigned char getMonth() const;
    virtual unsigned char getDay() const;

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const;

    virtual bool hasObjectOfType(const core::TypeInfo& type) const; 
};

class FilteredSession : public ISession
{
public:

    FilteredSession(const SessionConstPtr & originalSession, const std::vector<MotionPtr> & motions, const std::vector<core::ObjectWrapperConstPtr> & wrappers);
    virtual ~FilteredSession();

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual SubjectID getID() const;
    virtual SubjectID getLocalID() const;
    virtual void getMotions(Motions & motions) const;

    virtual const SubjectConstPtr & getSubject() const;

    virtual unsigned int getYear() const;
    virtual unsigned char getMonth() const;
    virtual unsigned char getDay() const;

    virtual void getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const;

    const SessionConstPtr & getOriginalSession() const;

private:
    SessionConstPtr originalSession;
    std::vector<MotionConstPtr> motions;
    std::vector<core::ObjectWrapperConstPtr> wrappers;
    std::string name;
    std::string localName;
};

#endif
