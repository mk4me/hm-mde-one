/********************************************************************
	created:	2011/10/12
	created:	12:10:2011   12:35
	filename: 	hmmPCH.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMPCH_H__
#define HEADER_GUARD_HMM__HMMPCH_H__

#include <vector>
#include <list>
#include <stack>
#include <map>

#include <boost/date_time.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/qglobal.h>
#include <QtCore/QVariant>
#include <QtCore/QElapsedTimer>
#include <QtCore/QThread>

#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QGridLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QSlider>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>

#include <utils/Utils.h>
//UTILS_PUSH_WARNINGS
//#include <osg/Node>
//#include <osg/Config>
//#include <osg/Quat>
//#include <osg/ref_ptr>
//#include <osg/Referenced>
//#include <osg/Vec3>
//UTILS_POP_WARNINGS

#include <utils/Push.h>
#include <timelinelib/IChannel.h>

#include <core/ObjectWrapper.h>
#include <core/ServiceManager.h>
#include <core/IService.h>
#include <core/PluginCommon.h>
#include <core/IDataManager.h>
#include <core/Filesystem.h>

#include <plugins/subject/Session.h>
#include <plugins/subject/DataFilter.h>
#include <plugins/subject/Motion.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/newTimeline/ITimelineService.h>
#include <plugins/video/Wrappers.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/chart/ChartVisualizer.h>


#endif
