/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   13:02
	filename: 	Session.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SESSION_H__
#define HEADER_GUARD_SUBJECT__SESSION_H__

#include <vector>
#include <core/SmartPtr.h>
#include <plugins/Subject/Motion.h>
#include <boost/foreach.hpp>

struct AntropometricData
{
	double width;
};
typedef core::shared_ptr<AntropometricData> AntropometricDataPtr;
typedef core::shared_ptr<const AntropometricData> AntropometricDataConstPtr;

struct ExaminationData
{

};
typedef core::shared_ptr<ExaminationData> ExaminationDataPtr;
typedef core::shared_ptr<const ExaminationData> ExaminationDataConstPtr;

struct PersonalData
{
	std::string name;
	std::string surname;
};
typedef core::shared_ptr<PersonalData> PersonalDataPtr;
typedef core::shared_ptr<const PersonalData> PersonalDataConstPtr;


class Session
{
public:
	typedef std::vector<MotionPtr> MotionCollection;
    typedef std::vector<core::ObjectWrapperPtr>::iterator WrapperIterator;
    typedef boost::iterator_range<WrapperIterator> WrapperRange;
	typedef boost::iterator_range<MotionCollection::iterator> MotionRange;
	typedef boost::iterator_range<MotionCollection::const_iterator> MotionConstRange;
	
private:
	MotionCollection motions;
	std::string name;
    std::vector<core::ObjectWrapperPtr> wrappers;
    core::ObjectWrapperPtr skeletalModelWrapper;
	
public:
	virtual ~Session() {}

public:
	MotionRange getMotions() { return boost::make_iterator_range(motions.begin(), motions.end()); }
	MotionConstRange getMotions() const { return boost::make_iterator_range(motions.cbegin(), motions.cend()); }

    void addWrapper(core::ObjectWrapperPtr wrapper)
    {
        if (wrapper->getTypeInfo() == typeid(kinematic::SkeletalModel)) {
            setSkeletalModel(wrapper);
        }
        wrappers.push_back(wrapper);
    }

    WrapperRange getWrappers() { return boost::make_iterator_range(wrappers.begin(), wrappers.end()); }

	void addMotion(MotionPtr motion)
	{
        if (this->skeletalModelWrapper) {
            motion->addWrapper(skeletalModelWrapper);
        }
		motions.push_back(motion);
	}


	std::string getName() const { return name; }
	void setName(std::string val) { name = val; }

    bool hasObjectOfType(const core::TypeInfo& type) const 
    {
        BOOST_FOREACH(const MotionPtr& motion, getMotions()) {
            if (motion->hasObjectOfType(type)) {
                return true;
            }
        }
        return false;
    }

private:
    void setSkeletalModel(const core::ObjectWrapperPtr& model) 
    {
        this->skeletalModelWrapper = model;
        BOOST_FOREACH(MotionPtr motion, getMotions()) {
            motion->addWrapper(model);
        }
    }
};


typedef core::shared_ptr<Session> SessionPtr;
typedef core::shared_ptr<const Session> SessionConstPtr;

CORE_DEFINE_WRAPPER(Session, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif
