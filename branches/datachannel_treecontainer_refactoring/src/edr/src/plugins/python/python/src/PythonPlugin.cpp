#include "PythonPCH.h"
#include "PythonService.h"
#include <corelib/IPlugin.h>
#include <corelib/BaseDataTypes.h>
#include "plugins/newVdf/IDataFlowProvider.h"


CORE_PLUGIN_BEGIN("python", core::UID::GenerateUniqueID("{CA2603B6-AFF0-4865-A4C9-56F2C55D9CC4}"));
    CORE_PLUGIN_ADD_SERVICE(python::PythonService);
CORE_PLUGIN_END;
