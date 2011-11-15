#include "NewChartPCH.h"
#include <core/Plugin.h>
#include "NewChartVisualizer.h"

CORE_PLUGIN_BEGIN("newChart", UID::GenerateUniqueID("{FB092C90-DC6E-4E5A-B33C-EAF84F325A99}"))
CORE_PLUGIN_ADD_VISUALIZER(NewChartVisualizer)
CORE_PLUGIN_END