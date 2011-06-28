
#include "CommunicationPCH.h"
#include "CommunicationPlugin.h"

CORE_PLUGIN_BEGIN("communication", UID::GenerateUniqueID("{A73693E1-A494-4064-84E6-BCD20DC6BD3E}"))
    CORE_PLUGIN_ADD_SERVICE(CommunicationService)
CORE_PLUGIN_END
