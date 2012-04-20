/********************************************************************
	created:	2011/09/04
	created:	4:9:2011   14:24
	filename: 	SkeletonSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETONSERIE_H__
#define HEADER_GUARD_KINEMATIC__SKELETONSERIE_H__

#include <core/IVisualizer.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"

class SkeletonSerie :  public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	SkeletonSerie(KinematicVisualizer * visualizer);
	virtual ~SkeletonSerie() {}

private slots:
	void setAxis(bool);

public:
	virtual void setName(const std::string & name)
    {
        this->name = name;
    }

    virtual const std::string & getName() const
    {
        return name;
    }

	virtual void setData(const core::ObjectWrapperConstPtr & data);

    virtual const core::ObjectWrapperConstPtr & getData() const
    {
        return data;
    }

	//! \return Dlugosc kanalu w sekundach
	virtual double getLength() const
	{
		UTILS_ASSERT(scheme);
		return scheme->getDuration();
	}

	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setLocalTime(double time);

    osg::Matrix getInitialMatrix() const;


private:
    MarkerCollectionConstPtr createTrajectories(kinematic::JointAnglesCollectionConstPtr joints);
    osg::Matrix getXYZMatrix() const;

    virtual void resetTransform();

private:
	KinematicVisualizer * visualizer;
	SkeletalVisualizationSchemePtr scheme;
	SchemeDrawerContainerPtr skeletonDrawers;
    core::ObjectWrapperConstPtr data;
    std::string name;
    TransformPtr skeletonNode;
    bool xyzAxis;
    TrajectoryDrawerPtr trajectoryDrawer;
};
typedef boost::shared_ptr<SkeletonSerie> SkeletonSeriePtr;
typedef boost::shared_ptr<const SkeletonSerie> SkeletonSerieConstPtr;

#endif

