#include "SubjectPCH.h"
#include <core/Plugin.h>
#include <core/ILog.h>
#include <plugins/subject/ISubject.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include "SubjectService.h"
#include "SubjectVisualizer.h"

CORE_PLUGIN_BEGIN("subject", UID::GenerateUniqueID("{F1B8F7D1-1611-4768-A01C-9F54084FFF51}"));
CORE_PLUGIN_ADD_VISUALIZER(SubjectVisualizer);
CORE_PLUGIN_ADD_SERVICE(SubjectService);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(ISubject);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(ISession);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMotion);
CORE_PLUGIN_END;
