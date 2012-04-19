/********************************************************************
	created:	2011/09/02
	created:	2:9:2011   9:28
	filename: 	MarkerSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MARKERSERIE_H__
#define HEADER_GUARD_KINEMATIC__MARKERSERIE_H__

#include <core/IVisualizer.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"

class MarkerSerie : public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	MarkerSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer),
	  markersDrawer(new SchemeDrawerContainer())
	  {
		  
	  }

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

    virtual osg::Matrix getInitialMatrix() const;


	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setLocalTime(double time);

private slots:
    void showGhost(bool visible);
	
private:
	KinematicVisualizer * visualizer;
	MarkersVisualizationSchemePtr scheme;
	SchemeDrawerContainerPtr markersDrawer;
	TrajectoryDrawerPtr trajectoryDrawer;
    osg::ref_ptr<osg::PositionAttitudeTransform> ghostNode;
    core::ObjectWrapperConstPtr data;
    std::string name;
};
typedef boost::shared_ptr<MarkerSerie> MarkerSeriePtr;
typedef boost::shared_ptr<const MarkerSerie> MarkerSerieConstPtr;


#endif
