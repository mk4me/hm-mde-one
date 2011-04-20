#include "TimelinePCH.h"
#include <core/Plugin.h>
#include <plugins/timeline/TimelineService.h>
#include <core/Log.h>

CORE_PLUGIN_BEGIN("timeline", UID::GenerateUniqueID("{DF1BD93D-1C14-4461-8E2B-1934486B98A3}"))
CORE_PLUGIN_ADD_SERVICE(TimelineService)
CORE_PLUGIN_END