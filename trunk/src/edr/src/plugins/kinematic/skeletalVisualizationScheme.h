/********************************************************************
	created:	2012/04/23
	created:	23:4:2012   12:45
	filename: 	skeletalVisualizationScheme.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETALVISUALIZATIONSCHEME_H__
#define HEADER_GUARD_KINEMATIC__SKELETALVISUALIZATIONSCHEME_H__

#include "VisualizationScheme.h"

class SkeletalVisualizationScheme : public VisualizationScheme
{
public:
    SkeletalVisualizationScheme() : 
		counterHelper(-1),
        dotColor(1,1,0,1),
        connectionColor(1, 1, 1, 0.5f)
	{}
	
public:
    kinematic::JointAnglesCollectionConstPtr getJoints() const { return joints; }
    void setJoints(kinematic::JointAnglesCollectionConstPtr val);

    virtual int getNumFrames() const { 
        UTILS_ASSERT(joints);  
        return joints->getNumPointsPerChannel(); 
    }
    double getFrameTime() const {
        UTILS_ASSERT(joints); 
        return static_cast<double>(joints->getLength()) / joints->getNumPointsPerChannel(); 
    }

    virtual double getDuration() const  { 
        if (joints) {
            return static_cast<double>(joints->getLength()); 
        } 
        UTILS_ASSERT(false);
        return 0.0;
    }

    virtual bool hasData() const { return joints.get(); }

    virtual osg::Vec3 getCurrentPosition() const { return currentPosition; }
    osg::Vec3 getRootPosition(double time) 
    {
        return joints->getRootPosition(time);
    }

protected:
    virtual void update( double time )
    {
        updateJointTransforms(time);
        currentPosition = getRootPosition(time);

        //int count = states.size();
        //for (int i = 0; i < count; i++) {
        //    states[i].position -= currentPosition;
        //}
    }

private:
    //! odswiezenie informacji o jointach
    void updateJoints();
    //! obliczenie poczatkowych transformacji
    //void computeBindPoses(KinematicModelPtr model);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(double time);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(const std::vector<osg::Quat>& rotations, kinematic::hAnimJointPtr joint, 
        osg::Quat parentRot, osg::Vec3 parentPos);

    void createSkeletonConnections(kinematic::hAnimJointPtr joint);

private:
    //! ulatwia ineksowanie jointow
    std::map<kinematic::hAnimJointPtr, int> visJoints;
	kinematic::JointAnglesCollectionConstPtr joints;
    //! pomocne przy zapelnianiu kolekcji jointow
    int counterHelper;
    osg::Vec3 currentPosition;
    osg::Vec4 connectionColor;
    osg::Vec4 dotColor;
};
typedef core::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemePtr;
typedef core::shared_ptr<const SkeletalVisualizationScheme> SkeletalVisualizationSchemeConstPtr;

#endif
