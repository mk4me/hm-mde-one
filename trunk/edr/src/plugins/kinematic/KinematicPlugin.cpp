#include "PCH.h"
#include <core/Plugin.h>
#include "mainwindow.h"
#include <plugins/kinematic/KinematicService.h>
#include <plugins/kinematic/KinematicParser.h>
#include <core/Log.h>

CORE_PLUGIN_BEGIN("kinematic", UniqueID('KINE', 'PLUG'));
CORE_PLUGIN_ADD_PARSER(KinematicParser);
CORE_PLUGIN_ADD_VISUALIZER(KinematicVisualizer);
//CORE_PLUGIN_ADD_SERVICE(KinematicService);
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
            UTILS_ASSERT(false, "Nieznany poziom logowania. Wiadomoœæ: %s", msg);
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