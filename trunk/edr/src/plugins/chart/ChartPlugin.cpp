#include <core/Plugin.h>
#include "EMGService.h"
#include "GRFService.h"
#include "ChartVisualizer.h"

CORE_PLUGIN_BEGIN("chart", UID::GenerateUniqueID("{DE42DFF1-2B7A-427D-9B2C-2F47B18039AF}"))
//CORE_PLUGIN_ADD_SERVICE(EMGService)
//CORE_PLUGIN_ADD_SERVICE(GRFService)
CORE_PLUGIN_ADD_SERVICE(ChartService)
CORE_PLUGIN_ADD_VISUALIZER(ChartVisualizer)
CORE_PLUGIN_END