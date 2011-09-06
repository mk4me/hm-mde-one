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
#include <timelinelib/IChannel.h>
#include "KinematicVisualizer.h"

class SkeletonSerie :  public QObject, public core::IVisualizer::SerieBase, public timeline::IChannel
{
	Q_OBJECT;
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	SkeletonSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer),
	  skeletonDrawers(new SchemeDrawerContainer())
	  {

	  }
	virtual ~SkeletonSerie() {}

private slots:
	void setAxis(bool);

protected:
	virtual void setSerieName(const std::string & name){}			

	virtual void setSerieData(const core::ObjectWrapperConstPtr & data);

	//! \return Sklonowane dane w kanale
	virtual timeline::IChannelPtr clone() const
	{
		//! NIE WSPIERAMY TUTAJ KLONOWANIA!!
		return timeline::IChannelPtr();
	}

	//! \return Dlugosc kanalu w sekundach
	virtual double getLength() const
	{
		UTILS_ASSERT(scheme);
		return scheme->getDuration();
	}

	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setTime(double time)
	{
		UTILS_ASSERT(scheme && skeletonDrawers);
		scheme->setTime(time);
		skeletonDrawers->update();
	}

private:
	KinematicVisualizer * visualizer;
	SkeletalVisualizationSchemePtr scheme;
	SchemeDrawerContainerPtr skeletonDrawers;
	osg::ref_ptr<osg::PositionAttitudeTransform> transformNode;
};
typedef boost::shared_ptr<SkeletonSerie> SkeletonSeriePtr;
typedef boost::shared_ptr<const SkeletonSerie> SkeletonSerieConstPtr;

#endif

