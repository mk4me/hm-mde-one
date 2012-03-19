/********************************************************************
	created:	2012/03/12
	created:	12:3:2012   11:17
	filename: 	Manipulators.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MANIPULATORS_H__
#define HEADER_GUARD_KINEMATIC__MANIPULATORS_H__

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/CoordinateSystemNode>
#include <osgText/Text>

#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/Translate2DDragger>
#include <osgManipulator/TranslateAxisDragger>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>

#include <iostream>

// The DraggerContainer node is used to fix the dragger's size on the screen
class DraggerContainer : public osg::Group
{
public:
    DraggerContainer() : _draggerSize(100.0f), _active(true) {}

    DraggerContainer( const DraggerContainer& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );

    META_Node( osgManipulator, DraggerContainer );

    void setDragger( osgManipulator::Dragger* dragger );
    

    osgManipulator::Dragger* getDragger() { return _dragger.get(); }
    const osgManipulator::Dragger* getDragger() const { return _dragger.get(); }

    void setDraggerSize( float size ) { _draggerSize = size; }
    float getDraggerSize() const { return _draggerSize; }

    void setActive( bool b ) { _active = b; }
    bool getActive() const { return _active; }

    void traverse( osg::NodeVisitor& nv );
    void setDraggerPivot( const osg::Vec3& pivot );
protected:
    osg::ref_ptr<osgManipulator::Dragger> _dragger;
    float _draggerSize;
    bool _active;
};
typedef osg::ref_ptr<DraggerContainer> DraggerContainerPtr;

class Manipulators
{
public:
    enum DraggerType 
    {
        TabPlane,
        TabPlaneTrackball,
        TabBoxTrackball,
        Trackball,
        Translate1D,
        Translate2D,
        TranslateAxis,
        TabBox
    };

    static void connect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer, const osg::Vec3& pivot );
    static void disconnect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer);
    static osgManipulator::Dragger* createDragger(DraggerType type);
    static DraggerContainer* creatreDraggerContainer(DraggerType type);
    static osg::Node* addDraggerToScene(osg::PositionAttitudeTransform* scene, DraggerType type, bool fixedSizeInScreen);
};
#endif
