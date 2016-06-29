#include "CommunicationPCH.h"
#include <corelib/IPlugin.h>
#include <corelib/PluginCommon.h>

#include "HMDBService.h"
#include "HMDBSource.h"

#include <plugins/medusaDatabase/IPatient.h>

CORE_PLUGIN_BEGIN("medusaDatabase", core::UID::GenerateUniqueID("{26AC5928-AAE3-4AB3-BFDB-447D4171A4E5}"))
	CORE_PLUGIN_ADD_SERVICE(medusaDatabase::HMDBService);
	CORE_PLUGIN_ADD_SOURCE(medusaDatabase::HMDBSource);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(medusaDatabase::IPatient);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(medusaDatabase::AntropometricData);
CORE_PLUGIN_END
