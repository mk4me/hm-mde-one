/********************************************************************
	created:  2010/11/22
	created:  22:11:2010   10:43
	filename: ChartPCH.h
	author:	  Micha³ Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTPCH_H__
#define __HEADER_GUARD__CHARTPCH_H__
#ifndef DISABLE_PRECOMPILED_HEADERS


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <bitset>
#include <limits>
#include <vector>
#include <stdio.h>

#include <boost/lexical_cast.hpp>

#include <utils/Utils.h>

#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QWidget>

UTILS_PUSH_WARNINGS
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/WindowManager>
#include <osgGA/StateSetManipulator>
#include <osg/MatrixTransform>
#include <osgText/Text>
UTILS_POP_WARNINGS

#include <osgui/EventCallback.h>
#include <osgui/QOsgWidgets.h>
#include <core/IServiceManager.h>
#include <plugins/timeline/ITimeline.h>
#include <plugins/timeline/Stream.h>

#include <core/Plugin.h>

#endif  // DISABLE_PRECOMPILED_HEADERS
#endif  // __HEADER_GUARD__VIDEOPCH_H__