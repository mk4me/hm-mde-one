#include "PCH.h"
#include "KinematicVisualizer.h"
#include <utils/DataChannel.h>
#include <core/Plugin.h>
#include "OsgSchemeDrawer.h"
#include "PointSchemeDrawer.h"
#include "LineSchemeDrawer.h"
#include <plugins/kinematic/KinematicService.h>
#include <plugins/kinematic/KinematicParser.h>
#include <core/ILog.h>
#include <plugins/kinematic/Wrappers.h>

CORE_PLUGIN_BEGIN("kinematic", UID::GenerateUniqueID("{3C0CD7AF-9351-46CC-A5FE-52AA182E1279}"));
    CORE_PLUGIN_ADD_PARSER(KinematicParser);
	CORE_PLUGIN_ADD_PARSER(AsfParser);
    CORE_PLUGIN_ADD_VISUALIZER(KinematicVisualizer);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(kinematic::JointAnglesCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(kinematic::SkeletalData);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(kinematic::SkeletalModel);
	/*
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(KinematicModel);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(SkeletalVisualizationScheme);*/
CORE_PLUGIN_END;

using namespace kinematic;

void logCallback(Logger::LogSeverity severity, const std::string& msg)
{
    switch ( severity ) {
        case Logger::Error:
            LOG_ERROR_STATIC_NAMED("kinematic", msg);
            break;
        case Logger::Warning:    
            LOG_WARNING_STATIC_NAMED("kinematic", msg);
            break;
        case Logger::Info:
            LOG_INFO_STATIC_NAMED("kinematic", msg);
            break;
        case Logger::Debug:
            LOG_DEBUG_STATIC_NAMED("kinematic", msg);
            break;
        default:
            UTILS_ASSERT(false, "Nieznany poziom logowania. Wiadomo��: %s", msg);
            break;
    }
}

static struct Initializer
{
    Initializer()
    {
        Logger::getInstance().setLogCallback(logCallback);
    }
} initializer;