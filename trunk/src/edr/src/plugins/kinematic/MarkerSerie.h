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
#include <timelinelib/IChannel.h>
#include "KinematicVisualizer.h"
#include "TrajectoriesDialog.h"

class MarkerSerie : public QObject, public core::IVisualizer::SerieBase, public timeline::IChannel
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
		  TrajectoryDrawerPtr t(new TrajectoryDrawer(osg::Vec4(1, 1, 1, 0.5f), 300));
		  trajectoryDrawer = t;
		  dialog = new TrajectoriesDialog(visualizer->widget, trajectoryDrawer);
	  }

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
		UTILS_ASSERT(scheme);
		scheme->setTime(time);
		markersDrawer->update();
	}

private slots:
	void trajectoriesDialog();
	
private:
	KinematicVisualizer * visualizer;
	SkeletalVisualizationSchemePtr scheme;
	SchemeDrawerContainerPtr markersDrawer;
	TrajectoryDrawerPtr trajectoryDrawer;
	TrajectoriesDialog* dialog;
};
typedef boost::shared_ptr<MarkerSerie> MarkerSeriePtr;
typedef boost::shared_ptr<const MarkerSerie> MarkerSerieConstPtr;


#endif
