#include "MedusaExporterPCH.h"
#include "MedusaExporterService.h"
#include "MedusaExporterServiceWidget.h"
#include <corelib/Filesystem.h>
#include <corelib/IServiceManager.h>
#include <corelib/PluginCommon.h>
#include <corelib/IMemoryDataManager.h>
#include <corelib/IMemoryDataManager.h>

using namespace medusaExporter;

MedusaExporterService::MedusaExporterService()
{
	exportWidget = new MedusaExporterServiceWidget(this);
}

MedusaExporterService::~MedusaExporterService()
{
}


QWidget* MedusaExporterService::getWidget()
{
	return exportWidget;
}

QWidgetList MedusaExporterService::getPropertiesWidgets()
{
	QWidgetList list;
	return list;
}

const bool MedusaExporterService::lateInit()
{
	core::IServiceManager* manager = plugin::getServiceManager();

	
	return true;
}

void MedusaExporterService::finalize()
{
	
}

void MedusaExporterService::update(double time)
{
	
}

void MedusaExporterService::init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager)
{
	this->memoryManager = memoryDataManager;
}

