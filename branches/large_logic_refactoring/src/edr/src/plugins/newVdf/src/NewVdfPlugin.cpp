#include "NewVdfPCH.h"
#include <core/Plugin.h>
#include <plugins/newVdf/NewVdfService.h>
#include <core/ILog.h>
#include "HACK.h"

CORE_PLUGIN_BEGIN("newVdf", UID::GenerateUniqueID("{4A8E064B-BA20-40D0-91CD-8C6A87398BD4}"))
    CORE_PLUGIN_ADD_DATA_SOURCE(IntSource, UID::GenerateUniqueID("{EF393C1F-2202-47DA-A1B9-D5DE868FDFFA}"))
    CORE_PLUGIN_ADD_DATA_PROCESSOR(IntProcessor, UID::GenerateUniqueID("{511A32C7-E82D-42FF-9BAA-2A74F83A5103}"))
    CORE_PLUGIN_ADD_DATA_SINK(IntSink, UID::GenerateUniqueID("{B4F05E79-80E7-46E7-97AE-FD81212C9AF7}"))
    CORE_PLUGIN_ADD_SERVICE(NewVdfService)
CORE_PLUGIN_END
