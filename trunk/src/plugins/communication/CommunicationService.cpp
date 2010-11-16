
#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>

CommunicationService::CommunicationService() : name("Communication")
{
	widget = new CommunicationWidget();
}

CommunicationService::~CommunicationService()
{
	delete widget;
}
