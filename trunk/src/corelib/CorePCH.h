/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   17:40
	filename: CorePCH.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__COREPCH_H__
#define __HEADER_GUARD__COREPCH_H__

#ifndef DISABLE_PRECOMPILED_HEADERS
// do momentu obs³u¿enia nag³ówków prekompilowanych na innych platformach
// mechanizm ten musi byæ wy³¹czony, inaczej mo¿e negatywnie wp³yn¹æ na
// czas kompilacji

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <osg/Node>
#include <osg/AnimationPath>
#include <osg/Config>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/ImageStream>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/Notify>
#include <osg/observer_ptr>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>
#include <osg/Quat>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/ShapeDrawable>
#include <osg/Timer>
#include <osg/Vec3>
#include <osg/Vec3d>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osgDB/ReadFile>

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Enum.h>


#endif  // DISABLE_PRECOMPILED_HEADERS
#endif  // __HEADER_GUARD__COREPCH_H__