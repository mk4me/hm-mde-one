#include "PythonPCH.h"
#include "PythonService.h"
#include <corelib/IPlugin.h>
#include <corelib/BaseDataTypes.h>

CORE_PLUGIN_BEGIN("python", core::UID::GenerateUniqueID("{5915FB7D-7FEF-4543-B153-08EA4485E1AB}"));
    CORE_PLUGIN_ADD_SERVICE(PythonService);
CORE_PLUGIN_END;
