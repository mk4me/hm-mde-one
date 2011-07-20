#include <core/Plugin.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/Subject.h>
#include "SubjectParser.h"
#include "EventVisualizer.h"
#include "C3DParser.h"


CORE_PLUGIN_BEGIN("c3d", UID::GenerateUniqueID("{15AC5928-AAE3-4AB3-BFDB-447D4171A4E5}"))
	CORE_PLUGIN_ADD_PARSER(C3DParser);
    CORE_PLUGIN_ADD_PARSER(SubjectParser);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(Subject)
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(ScalarChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(C3DAnalogChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(EMGChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(GRFChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(MarkerChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(MarkerCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(EventsCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(GRFCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(EMGCollection);
	CORE_PLUGIN_ADD_VISUALIZER(EventVisualizer);
CORE_PLUGIN_END