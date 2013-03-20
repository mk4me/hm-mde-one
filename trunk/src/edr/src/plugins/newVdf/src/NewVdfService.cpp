#include "NewVdfPCH.h"
#include <corelib/IServiceManager.h>
#include <plugins/newVdf/NewVdfService.h>
#include "NewVdfWidget.h"
#include "TypesWindow.h"
#include "CanvasStyleEditorWidget.h"
#include "SceneModel.h"
#include "VdfScene.h"
#include "Command.h"
#include "CommandStackDebug.h"
#include <QtCore/QObject>
#include <corelib/PluginCommon.h>
#include "DataSinkManager.h"
#include "DataProcessorManager.h"
#include "DataSourceManager.h"

using namespace vdf;

NewVdfService::NewVdfService() :
	name("Data flow service"),
	commandStack(new CommandStack()),
	dataSinkManager(new DataSinkManager()),
	dataProcessorManager(new DataProcessorManager()),
	dataSourceManager(new DataSourceManager())
{
    CanvasStyleEditorPtr canvas(new CanvasStyleEditor());
	SceneModelPtr scene(new SceneModel(canvas));
    newVdfWidget = new NewVdfWidget(commandStack, scene);
//	commandStackDebug = new CommandStackDebug(commandStack);
//	QObject::connect(commandStack.get(), SIGNAL(changed()), commandStackDebug, SLOT(refresh()));
//	commandStack->addCommand(ICommandPtr(new NullCommand()));
    typesWindow = new TypesWindow(commandStack, canvas, newVdfWidget);
    QObject::connect(newVdfWidget, SIGNAL(singleNodeSelected(IVisualNodePtr)), typesWindow, SLOT(onNodeSelected(IVisualNodePtr)));
	dataSourceManager->attach(typesWindow);
	dataProcessorManager->attach(typesWindow);
	dataSinkManager->attach(typesWindow);
    canvasStyleEditorWidget = new CanvasStyleEditorWidget(canvas);
	QObject::connect(canvasStyleEditorWidget, SIGNAL(backgroundAccepted(IBackgroundStrategyPtr)), newVdfWidget->getScene() , SLOT(setBackgroundStrategy(IBackgroundStrategyPtr)));
}

NewVdfService::~NewVdfService()
{
}


QWidget* NewVdfService::getWidget() 
{
	return newVdfWidget;
}

QWidget* NewVdfService::getSettingsWidget()
{
    return canvasStyleEditorWidget;
}

QWidget* NewVdfService::getControlWidget()
{
    return typesWindow;
}

const bool NewVdfService::lateInit()
{
	std::list<IWorkflowItemBasePtr> items;

	core::IServiceManager* manager = plugin::getServiceManager();

	int count = manager->getNumServices();

	for (int i = 0; i < count; ++i) {
		IDataFlowProviderPtr provider = core::dynamic_pointer_cast<IDataFlowProvider>(manager->getService(i));
		if (provider) {
			provider->provideDataFlowItems(items);
		}
	}

	DataProcessorManagerPtr processorManager = core::dynamic_pointer_cast<DataProcessorManager>(dataProcessorManager);
	DataSinkManagerPtr sinkManager = core::dynamic_pointer_cast<DataSinkManager>(dataSinkManager);
	DataSourceManagerPtr sourceManager = core::dynamic_pointer_cast<DataSourceManager>(dataSourceManager);

	for (auto it = items.begin(); it != items.end(); ++it) {
		IDataProcessorPtr processor = core::dynamic_pointer_cast<IDataProcessor>(*it);
		if (processor) {
			processorManager->registerDataProcessor(processor);
		} else {
			IDataSourcePtr source = core::dynamic_pointer_cast<IDataSource>(*it);
			if (source) {
				sourceManager->registerDataSource(source);
			}
			IDataSinkPtr sink = core::dynamic_pointer_cast<IDataSink>(*it);
			if (sink) {
				sinkManager->registerDataSink(sink);
			}

		}
	}
	return true;
}

void NewVdfService::finalize()
{

}

void NewVdfService::update( double time )
{

}

void NewVdfService::init( core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager )
{

}


void vdf::NewVdfService::registerDataSource( const IDataSourcePtr & dataSource )
{
    DataSourceManagerPtr sourceManager = core::dynamic_pointer_cast<DataSourceManager>(dataSourceManager);
    sourceManager->registerDataSource(dataSource);
}

void vdf::NewVdfService::registerDataSink( const IDataSinkPtr & dataSink )
{
    DataSinkManagerPtr sinkManager = core::dynamic_pointer_cast<DataSinkManager>(dataSinkManager);
    sinkManager->registerDataSink(dataSink);
}

void vdf::NewVdfService::registerDataProcessor( const IDataProcessorPtr & dataProcessor )
{
    DataProcessorManagerPtr processorManager = core::dynamic_pointer_cast<DataProcessorManager>(dataProcessorManager);
    processorManager->registerDataProcessor(dataProcessor);
}
