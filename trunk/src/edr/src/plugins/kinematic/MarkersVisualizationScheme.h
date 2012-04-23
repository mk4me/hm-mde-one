/********************************************************************
	created:	2012/04/23
	created:	23:4:2012   12:47
	filename: 	MarkersVisualizationScheme.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MARKERSVISUALIZATIONSCHEME_H__
#define HEADER_GUARD_KINEMATIC__MARKERSVISUALIZATIONSCHEME_H__

#include "VisualizationScheme.h"

class MarkersVisualizationScheme : public VisualizationScheme
{
public:
    MarkerCollectionConstPtr getMarkers() const { return markers; }
    virtual double getDuration() const 
    {
        if (markers) {
            return markers->getLength();
        }
        UTILS_ASSERT(false);
        return 0.0;
    }

    virtual bool hasData() const { return markers.get(); }

    void setMarkers(MarkerCollectionConstPtr val);
    void setMarkersDataFromVsk(kinematic::VskParserConstPtr vsk);
    virtual osg::Vec3 getCurrentPosition() const { return currentPosition; }
    osg::Vec3 getRootPosition(double time) const;

protected:
    virtual void update( double time )
    {
        updateMarkers(time);
    }

private:
    //! odswiezenie informacji o markerach
    void updateMarkers(double time);


private:
    MarkerCollectionConstPtr markers;
    osg::Vec3 currentPosition;
};
typedef core::shared_ptr<MarkersVisualizationScheme> MarkersVisualizationSchemePtr;
typedef core::shared_ptr<const MarkersVisualizationScheme> MarkersVisualizationSchemeConstPtr;

#endif
