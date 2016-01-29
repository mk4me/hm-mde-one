#include <corelib/IPlugin.h>
#include <corelib/BaseDataTypes.h>
#include "plugins/newVdf/IDataFlowProvider.h"
#include "PythonProcessingBlock.h"


CORE_PLUGIN_BEGIN("mdePython", core::UID::GenerateUniqueID("{6EC28252-E4CD-4D0C-BD6A-ABC7FB691990}"));
VDF_SERVICE_BEGIN(PythonElements, "FCBF1441-360B-4B69-B193-6008A7EAB6FC")
VDF_ADD_DATA_PROCESSOR(
	python::PythonProcessingBlock,
	"PythonVector",
	"C2E59CDA-BD3C-43DB-AA1A-95FF304C97EB",
	QIcon(":/newVdf/icons/processor.png"));
VDF_SERVICE_END(PythonElements)
CORE_PLUGIN_END;
