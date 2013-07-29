#include "NewVdfPCH.h"
#include <corelib/IServiceManager.h>
#include <plugins/newVdf/NewVdfService.h>
#include "NewVdfWidget.h"
#include "TypesWindow.h"
#include "PropertiesWindow.h"
#include "CanvasStyleEditorWidget.h"
#include "SceneModel.h"
#include "VdfScene.h"
#include "Command.h"
#include <utils/CommandStack.h>
//#include "CommandStackDebug.h"
#include <QtCore/QObject>
#include <corelib/PluginCommon.h>
#include "DataSinkManager.h"
#include "DataProcessorManager.h"
#include <corelib/IMemoryDataManager.h>
#include "DataSourceManager.h"
#include <plugins/c3d/C3DCollections.h>
#include <corelib/IMemoryDataManager.h>

using namespace vdf;

NewVdfService::NewVdfService() :
	name("Data flow service"),
	commandStack(new utils::CommandStack()),
	dataSinkManager(new DataSinkManager()),
	dataProcessorManager(new DataProcessorManager()),
	dataSourceManager(new DataSourceManager())
{
    CanvasStyleEditorPtr canvas(new CanvasStyleEditor());
	SceneModelPtr scene(new SceneModel(canvas));
    newVdfWidget = new NewVdfWidget(commandStack, scene, &resultsModel);
//	commandStackDebug = new CommandStackDebug(commandStack);
//	QObject::connect(commandStack.get(), SIGNAL(changed()), commandStackDebug, SLOT(refresh()));
//	commandStack->addCommand(ICommandPtr(new NullCommand()));
    typesWindow = new TypesWindow(commandStack, canvas, scene);
    propertiesWindow = new PropertiesWindow(commandStack);
    QObject::connect(newVdfWidget, SIGNAL(singleNodeSelected(IVisualNodePtr)), propertiesWindow, SLOT(onNodeSelected(IVisualNodePtr)));
	dataSourceManager->attach(typesWindow);
	dataProcessorManager->attach(typesWindow);
	dataSinkManager->attach(typesWindow);
    canvasStyleEditorWidget = new CanvasStyleEditorWidget(canvas);
	QObject::connect(canvasStyleEditorWidget, SIGNAL(backgroundAccepted(IBackgroundStrategyPtr)), newVdfWidget->getScene() , SLOT(setBackgroundStrategy(IBackgroundStrategyPtr)));

    resultsView = new QTreeView();
    resultsView->setModel(&resultsModel);
    resultProperty = new QWidget();
    resultButton = new QPushButton();
    resultButton->setText(tr("Transfer results"));
    resultProperty->setLayout(new QVBoxLayout());
    resultProperty->layout()->addWidget(resultsView);
    resultProperty->layout()->addWidget(resultButton);
    connect(resultButton, SIGNAL(clicked()), this, SLOT(onTransferResults()));
    resultProperty->setObjectName("Result view");
}

NewVdfService::~NewVdfService()
{
}


QWidget* NewVdfService::getWidget() 
{
	return newVdfWidget;
}

QWidgetList vdf::NewVdfService::getPropertiesWidgets()
{
    QWidgetList list;
    list.push_back(canvasStyleEditorWidget);
    list.push_back(typesWindow);
    list.push_back(propertiesWindow);
    list.push_back(resultProperty);
    return list;
}

const bool NewVdfService::lateInit()
{
	std::list<IWorkflowItemBaseWeakPtr> items;

	core::IServiceManager* manager = plugin::getServiceManager();

	int count = manager->getNumServices();

	for (int i = 0; i < count; ++i) {
		IDataFlowProviderPtr provider = utils::dynamic_pointer_cast<IDataFlowProvider>(manager->getService(i));
		if (provider) {
			provider->provideDataFlowItems(items);
		}
	}

	DataProcessorManagerPtr processorManager = utils::dynamic_pointer_cast<DataProcessorManager>(dataProcessorManager);
	DataSinkManagerPtr sinkManager = utils::dynamic_pointer_cast<DataSinkManager>(dataSinkManager);
	DataSourceManagerPtr sourceManager = utils::dynamic_pointer_cast<DataSourceManager>(dataSourceManager);

	for (auto it = items.begin(); it != items.end(); ++it) {
		IDataProcessorPtr processor = utils::dynamic_pointer_cast<IDataProcessor>(it->lock());
		if (processor) {
			processorManager->registerDataProcessor(processor);
		} else {
			IDataSourcePtr source = utils::dynamic_pointer_cast<IDataSource>(it->lock());
			if (source) {
				sourceManager->registerDataSource(source);
			}
			IDataSinkPtr sink = utils::dynamic_pointer_cast<IDataSink>(it->lock());
			if (sink) {
				sinkManager->registerDataSink(sink);
			}

		}
	}
	return true;
}

void NewVdfService::finalize()
{
    dataProcessorManager = IDataProcessorManagerPtr(); 
    dataSourceManager = IDataSourceManagerPtr(); 
    dataSinkManager = IDataSinkManagerPtr(); 
}                                                  

void NewVdfService::update( double time )
{

}

void NewVdfService::init( core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager )
{
    this->memoryManager = memoryDataManager;
    auto memoryManager = plugin::getHierarchyManagerReader();
    memoryManager->addObserver(shared_from_this());
}


void vdf::NewVdfService::registerDataSource( const IDataSourcePtr & dataSource )
{
    DataSourceManagerPtr sourceManager = utils::dynamic_pointer_cast<DataSourceManager>(dataSourceManager);
    sourceManager->registerDataSource(dataSource);
}

void vdf::NewVdfService::registerDataSink( const IDataSinkPtr & dataSink )
{
    DataSinkManagerPtr sinkManager = utils::dynamic_pointer_cast<DataSinkManager>(dataSinkManager);
    sinkManager->registerDataSink(dataSink);
}

void vdf::NewVdfService::registerDataProcessor( const IDataProcessorPtr & dataProcessor )
{
    DataProcessorManagerPtr processorManager = utils::dynamic_pointer_cast<DataProcessorManager>(dataProcessorManager);
    processorManager->registerDataProcessor(dataProcessor);
}

void vdf::NewVdfService::observe( const core::IMemoryDataManagerHierarchy::HierarchyChangeList & changes )
{
    auto memoryManager = plugin::getHierarchyManagerReader();
    auto model = newVdfWidget->getSceneModel();
    auto items = model->getVisualItems<IVisualSourceNodePtr>();
    for (auto it = items.begin(); it != items.end(); ++it) {
        auto observer = dynamic_cast<vdf::INodeHierarchyObserver*>((*it)->getModelNode());
        if (observer) {
            observer->refresh(memoryManager, changes);
        }
    }
}

void vdf::NewVdfService::onTransferResults()
{
    core::HierarchyItemPtr root(new core::HierarchyItem(tr("Processed"), QString()));
    auto count = resultsModel.getNumChildren();
    for (int i = 0; i < count; ++i) {
        root->appendChild(resultsModel.getChild(i));
    }

    auto transaction = this->memoryManager->hierarchyTransaction();
    transaction->addRoot(root);

    emit transferResults();
}

