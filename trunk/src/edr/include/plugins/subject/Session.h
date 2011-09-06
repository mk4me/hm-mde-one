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
	typedef boost::iterator_range<MotionCollection::iterator> MotionIterator;
	typedef boost::iterator_range<MotionCollection::const_iterator> MotionConstIterator;
	
private:
	MotionCollection motions;
	std::string name;
	kinematic::SkeletalModelConstPtr skeletalModel;
	
public:
	virtual ~Session() {}

public:
	MotionIterator getMotions() { return boost::make_iterator_range(motions.begin(), motions.end()); }
	MotionConstIterator getMotions() const { return boost::make_iterator_range(motions.cbegin(), motions.cend()); }
	void addMotion(MotionPtr motion)
	{
		motions.push_back(motion);
	}
	std::string getName() const { return name; }
	void setName(std::string val) { name = val; }
	kinematic::SkeletalModelConstPtr getSkeletalModel() const { return skeletalModel; }
	void setSkeletalModel(kinematic::SkeletalModelConstPtr val) { skeletalModel = val; }

	bool hasGrf() const {
		BOOST_FOREACH(const MotionPtr& motion, getMotions()) {
			if (motion->getGrf()) {
				return true;
			}
		}
		return false;
	}
	bool hasEmg() const {
		BOOST_FOREACH(const MotionPtr& motion, getMotions()) {
			if (motion->getEmg()) {
				return true;
			}
		}
		return false;
	}
	bool hasMarkers() const {
		BOOST_FOREACH(const MotionPtr& motion, getMotions()) {
			if (motion->getMarkers()) {
				return true;
			}
		}
		return false;
	}
	bool hasJoints() const {
		BOOST_FOREACH(const MotionPtr& motion, getMotions()) {
			if (motion->getJoints()) {
				return true;
			}
		}
		return false;
	}

	bool hasVideo() const {
		BOOST_FOREACH(const MotionPtr& motion, getMotions()) {
			if (motion->getVideos().size()) {
				return true;
			}
		}
		return false;
	}

};


typedef core::shared_ptr<Session> SessionPtr;
typedef core::shared_ptr<const Session> SessionConstPtr;

CORE_DEFINE_WRAPPER(Session, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

#endif
