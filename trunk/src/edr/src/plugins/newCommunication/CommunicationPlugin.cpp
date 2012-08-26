#include "CommunicationPCH.h"
#include "CommunicationPlugin.h"
#include <plugins/newCommunication/Patient.h>
#include "DataSource.h"


CORE_DEFINE_WRAPPER(Patient, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

CORE_PLUGIN_BEGIN("newCommunication", UID::GenerateUniqueID("{B73693E1-A494-4064-84E6-BCD20DC6BD3E}"))
    //CORE_PLUGIN_ADD_SERVICE(CommunicationService)
    CORE_PLUGIN_ADD_SOURCE(CommunicationDataSource)
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(communication::IPatient)
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(communication::AntropometricData)
CORE_PLUGIN_END
