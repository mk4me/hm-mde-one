/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   10:52
	filename: 	Motion.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__MOTION_H__
#define HEADER_GUARD_SUBJECT__MOTION_H__

#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/video/Wrappers.h>
#include <core/SmartPtr.h>

class Motion
{
public:
	virtual ~Motion() {}

public:
	GRFCollectionConstPtr getGrf() const { return grf; }
	void setGrf(GRFCollectionConstPtr val) { grf = val; }

	EMGCollectionConstPtr getEmg() const { return emg; }
	void setEmg(EMGCollectionConstPtr val) { emg = val; }

	kinematic::JointAnglesCollectionConstPtr getJoints() const { return joints; }
	void setJoints(kinematic::JointAnglesCollectionConstPtr val) { joints = val; }

	MarkerCollectionConstPtr getMarkers() const { return markers; }
	void setMarkers(MarkerCollectionConstPtr val) { markers = val; }

	VideoCollectionConstPtr getVideo() const { return videos; }
	void setVideo(VideoCollectionConstPtr val) { videos = val; }

	std::string getName() const { return name; }
	void setName(std::string val) { name = val; }

	ForceCollectionConstPtr getForces() const { return forces; }
	void setForces(ForceCollectionConstPtr val) { forces = val; }

	AngleCollectionConstPtr getAngles() const { return angles; }
	void setAngles(AngleCollectionConstPtr val) { angles = val; }

	PowerCollectionConstPtr getPowers() const { return powers; }
	void setPowers(PowerCollectionConstPtr val) { powers = val; }

	MomentCollectionConstPtr getMoments() const { return moments; }
	void setMoments(MomentCollectionConstPtr val) { moments = val; }


private:
	GRFCollectionConstPtr grf;
	EMGCollectionConstPtr emg;
	kinematic::JointAnglesCollectionConstPtr joints;
	MarkerCollectionConstPtr markers;
	ForceCollectionConstPtr forces;
	AngleCollectionConstPtr angles;
	PowerCollectionConstPtr powers;
	MomentCollectionConstPtr moments;

	VideoCollectionConstPtr videos;
	std::string name;
};
typedef core::shared_ptr<Motion> MotionPtr;
typedef core::shared_ptr<const Motion> MotionConstPtr;


#endif
