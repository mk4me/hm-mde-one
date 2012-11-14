/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   7:55
    filename: VdfPCH.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___VDFPCH_H__
#define HEADER_GUARD___VDFPCH_H__

#ifndef DISABLE_PRECOMPILED_HEADERS

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/TextureCubeMap>
#include <osg/TexMat>
#include <osg/CullFace>
#include <osg/ImageStream>
#include <osg/io_utils>
#include <osg/Timer>
#include <osg/GLExtensions>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/WindowManager>
#include <osgText/Text>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/EventVisitor>
#include <osgDB/ReadFile>
UTILS_POP_WARNINGS

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include <utils/Debug.h>
#include <utils/Enum.h>

#endif  // DISABLE_PRECOMPILED_HEADERS
#endif  // HEADER_GUARD___VDFPCH_H__
