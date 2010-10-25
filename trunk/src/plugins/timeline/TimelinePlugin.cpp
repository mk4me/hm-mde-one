#include "TimelinePCH.h"
#include <core/Plugin.h>
#include <plugins/timeline/TimelineService.h>

CORE_PLUGIN_BEGIN("Timeline", UniqueID('TIME', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(TimelineService)
CORE_PLUGIN_END