#include "VdfPCH.h"
#include <core/Plugin.h>
#include <plugins/vdf/VdfService.h>
#include <core/Log.h>

CORE_PLUGIN_BEGIN("vdf", UID::GenerateUniqueID("{650D6C7D-1D43-4D4E-A727-4C25BD72C5CE}"))
CORE_PLUGIN_ADD_SERVICE(VdfService)
CORE_PLUGIN_END
