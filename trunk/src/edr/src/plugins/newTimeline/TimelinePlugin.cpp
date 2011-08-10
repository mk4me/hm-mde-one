/********************************************************************
    created:  2011/08/08
    created:  8:8:2011   8:42
    filename: TimelinePlugin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMELINEPLUGIN_H__
#define HEADER_GUARD_TIMELINE__TIMELINEPLUGIN_H__

#include "TimelinePCH.h"
#include <core/Plugin.h>
#include <plugins/newTimeline/TimelineService.h>
#include <core/ILog.h>

CORE_PLUGIN_BEGIN("newTimeline", UID::GenerateUniqueID("{AF1BD93D-1C14-4461-8E2B-1934486B98A3}"))
CORE_PLUGIN_ADD_SERVICE(TimelineService)
CORE_PLUGIN_END

#endif  //  HEADER_GUARD_TIMELINE__TIMELINEPLUGIN_H__