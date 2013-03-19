/********************************************************************
    created:  2011/07/18
    created:  18:7:2011   14:07
    filename: TimelinePCH.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMELINEPCH_H__
#define HEADER_GUARD_TIMELINE__TIMELINEPCH_H__

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>

#include <corelib/IPlugin.h>
#include <corelib/SmartPtr.h>

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Enum.h>

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtGui/QFrame>
#include <QtGui/QAction>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QPainter>

#define QT_NO_QT_INCLUDE_WARN
#include <qwt_scale_draw.h>
#include <qwt_slider.h>
#undef QT_NO_QT_INCLUDE_WARN

#endif  //  HEADER_GUARD_TIMELINE__TIMELINEPCH_H__
