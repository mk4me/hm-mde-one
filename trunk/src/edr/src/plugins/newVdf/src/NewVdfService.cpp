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
#include <boost/chrono/thread_clock.hpp>
#include <corelib/IMemoryDataManager.h>
#include "PresetsWidget.h"
#include "MergedWidget.h"

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

    mergedWidget = new MergedWidget();
    mergedWidget->setObjectName("Merged");
    presetsWidget = new PresetsWidget(newVdfWidget->getSceneModel(), typesWindow);
    presetsWidget->setObjectName("Presets");
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
    //list.push_back(mergedWidget);
    list.push_back(presetsWidget);
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
    using namespace boost::chrono;
    static thread_clock::time_point startTime = thread_clock::now();
    thread_clock::time_point currentTime = thread_clock::now();  
    if (duration_cast<milliseconds>(currentTime - startTime).count()  > 1000) {
        startTime = currentTime;

        // TODO : tutaj tylko tymczasowo

        auto sceneModel = newVdfWidget->getSceneModel();
        auto nodes = sceneModel->getVisualItems<IVisualNodePtr>();
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            IVisualNodePtr node = *it;
            auto modelNode = node->getModelNode();
            INodeValidation* valid = dynamic_cast<INodeValidation*>(modelNode);
            if (valid) {
                node->setValid(valid->isNodeValid());
            }
        }
        
    }
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
    for (int i = count - 1; i >= 0; --i) {
        core::IHierarchyItemPtr itm = utils::const_pointer_cast<core::IHierarchyItem>(resultsModel.getChild(i));
        root->appendChild(itm);
    }

    auto transaction = this->memoryManager->hierarchyTransaction();
    transaction->addRoot(root);

    emit transferResults();
    resultsModel.clear();
}

