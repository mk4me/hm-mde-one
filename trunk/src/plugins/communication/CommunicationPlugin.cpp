
#include "CommunicationPCH.h"
#include "CommunicationPlugin.h"

DEFINE_DEFAULT_LOGGER("edr.communication")
using namespace core;

CORE_PLUGIN_BEGIN("Communication", UniqueID('COMM', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(CommunicationService)
CORE_PLUGIN_ADD_PARSER(TextParser)
CORE_PLUGIN_END
