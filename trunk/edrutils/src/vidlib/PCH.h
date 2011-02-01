#ifndef __HEADER_GUARD_VIDLIB__PCH_H__
#define __HEADER_GUARD_VIDLIB__PCH_H__
#ifndef DISABLE_PRECOMPILED_HEADERS

#include <vidlib/Config.h>

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <boost/lexical_cast.hpp>

#ifdef VIDLIB_ENABLE_OSG
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
#endif // VIDLIB_ENABLE_OSG

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Enum.h>

#endif // DISABLE_PRECOMPILED_HEADERS
#endif // __HEADER_GUARD_VIDLIB__PCH_H__