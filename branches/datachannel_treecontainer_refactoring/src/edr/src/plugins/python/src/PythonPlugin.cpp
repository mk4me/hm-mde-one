#include "PythonPCH.h"
#include "PythonService.h"
#include <corelib/IPlugin.h>
#include <corelib/BaseDataTypes.h>
#include "plugins/newVdf/IDataFlowProvider.h"
#include "PythonProcessingBlock.h"


CORE_PLUGIN_BEGIN("python", core::UID::GenerateUniqueID("{5915FB7D-7FEF-4543-B153-08EA4485E1AB}"));
	VDF_SERVICE_BEGIN(PythonElements, "FCBF1441-360B-4B69-B193-6008A7EAB6FC")
		VDF_ADD_DATA_PROCESSOR(
			python::PythonProcessingBlock,
			"PythonVector",
			"C2E59CDA-BD3C-43DB-AA1A-95FF304C97EB",
			QIcon(":/newVdf/icons/processor.png"));
	VDF_SERVICE_END(PythonElements)

    CORE_PLUGIN_ADD_SERVICE(python::PythonService);
CORE_PLUGIN_END;
