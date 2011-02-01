
#include "CommunicationPCH.h"
#include "CommunicationPlugin.h"

CORE_PLUGIN_BEGIN("communication", UniqueID('COMM', 'PLUG'))
CORE_PLUGIN_ADD_SERVICE(CommunicationService)
CORE_PLUGIN_ADD_PARSER(TextParser)
CORE_PLUGIN_END
