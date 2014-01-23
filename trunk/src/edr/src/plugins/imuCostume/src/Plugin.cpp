#include "PCH.h"
#include <corelib/IPlugin.h>
#include <plugins/imuCostume/Wrappers.h>
#include "DataSource.h"

CORE_PLUGIN_BEGIN("imuCostume", core::UID::GenerateUniqueID("{3C0CD7AF-0000-46CC-A5FE-52AA182E1279}"));
CORE_PLUGIN_ADD_SOURCE(IMU::IMUCostumeDataSource);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::IMUStream);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::Vec3Stream);
CORE_PLUGIN_END;