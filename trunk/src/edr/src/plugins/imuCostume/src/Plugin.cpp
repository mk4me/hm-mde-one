#include "PCH.h"
#include <corelib/IPlugin.h>
//#include <plugins/imuCostume/Wrappers.h>
#include "DataSource.h"
#include "IMUParser.h"
#include "IMUPerspective.h"
#include "IMUDatParser.h"


CORE_PLUGIN_BEGIN("imuCostume", core::UID::GenerateUniqueID("{3C0CD7AF-0000-46CC-A5FE-52AA182E1279}"));
CORE_PLUGIN_ADD_SOURCE(IMU::IMUCostumeDataSource);
CORE_PLUGIN_ADD_SERVICE(IMU::IMUPerspectiveService);
CORE_PLUGIN_ADD_PARSER(IMU::IMUDatParserCore);
CORE_PLUGIN_ADD_PARSER(IMU::IMUCFGParser);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::IMUCostumeDataSource::RawDataStream);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::IMUCostumeDataSource::CANopenFramesStream);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::IMUCostumeDataSource::CostumeStream);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::IMUStream);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::Vec3Stream);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::Frames);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(IMU::IMUConfig);
CORE_PLUGIN_END;