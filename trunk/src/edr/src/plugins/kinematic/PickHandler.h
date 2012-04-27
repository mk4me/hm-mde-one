/********************************************************************
	created:	2012/03/09
	created:	9:3:2012   12:10
	filename: 	PickHandler.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__PICKHANDLER_H__
#define HEADER_GUARD_KINEMATIC__PICKHANDLER_H__


#include <osg/Geode>
#include <osg/Drawable>
#include <osgGa/GUIEventHandler>
#include <osgGa/GUIEventAdapter>
#include <osgViewer/View>

#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <list>



class PickHandler : public osgGA::GUIEventHandler 
{
public: 
    //typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::Geode* GeodePtr;
    typedef osg::ref_ptr<osg::Drawable> DrawablePtr;
    //typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    typedef boost::tuple<GeodePtr, DrawablePtr, double> PickerTuple;
    typedef std::list<PickerTuple> PickerList;
    typedef boost::function<void (const PickerList& )> HandlerFunction;

public:
    PickHandler(HandlerFunction callback);

    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
    virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
    
protected:
    HandlerFunction handlerFunction;
};



#endif
