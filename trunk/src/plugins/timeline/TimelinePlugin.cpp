#include "TimelinePCH.h"
#include <core/Plugin.h>
#include <plugins/timeline/TimelineService.h>
#include <core/Log.h>

CORE_PLUGIN_BEGIN("timeline", UniqueID('TIME', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(TimelineService)
CORE_PLUGIN_END