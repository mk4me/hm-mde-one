#include "VdfPCH.h"
#include <core/IServiceManager.h>
#include <plugins/vdf/VdfService.h>
#include "VdfWidget.h"


VdfService::VdfService()
:   name("Vdf")
{
    widget = new VdfWidget(this);
}


VdfService::~VdfService()
{

}

QWidget* VdfService::getWidget(std::vector<QObject*>& actions)
{
    return widget;
}

void VdfService::init(core::IServiceManager* serviceManager, core::IDataManager* dataManager)
{

}

const std::string& VdfService::getName() const
{
    return name;
}

