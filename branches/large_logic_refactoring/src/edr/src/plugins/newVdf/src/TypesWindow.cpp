#include "newVdfPCH.h"
#include "TypesWindow.h"
#include "CanvasStyleEditor.h"
#include "NewVdfWidget.h"
#include "HACK.h"
#include <core/IDataSourceManager.h>
#include <core/IDataSinkManager.h>
#include <core/IDataProcessorManager.h>
#include <core/IDataSource.h>
#include <core/PluginCommon.h>

TypesWindow::TypesWindow(CommandStackPtr stack, CanvasStyleEditorPtr canvas, NewVdfWidget* newVdf, QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    canvas(canvas),
    newVdf(newVdf),
	commmandStack(stack)
{
    setupUi(this);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget->setDragEnabled(true);
    treeWidget->viewport()->setAcceptDrops(true);
    treeWidget->setDropIndicatorShown(true);
    connect(this->insertButton, SIGNAL(clicked()), this, SLOT(insert()));
	connect(this->processButton, SIGNAL(clicked()), this, SLOT(run()));
    //addEntry("Source 3");
    //addEntry("Processing 3:6");
    //addEntry("Sink 5");

    core::getDataSourceManager()->attach(this);
    core::getDataProcessorManager()->attach(this);
    core::getDataSinkManager()->attach(this);
    
    //addEntry(new IntSource());
    //addEntry(new IntProcessor());
    //addEntry(new IntSink());
}

void TypesWindow::insert()
{
    QTreeWidgetItem* current = treeWidget->currentItem();
    if (current) {
        insert(current->text(0), QPointF());
    }
}

void TypesWindow::insert( const QString& name, const QPointF& scenePos )
{
    auto item = createItemByEntry(name);
	commmandStack->addCommand(ICommandPtr(new AddToSceneCommand(newVdf->getSceneModel(), item, scenePos)));
    /*auto* node = item.get<0>();
    auto* vis = node->visualItem();
    vis->setPos(scenePos);

    newVdf->getSceneModel()->addItem(node);

    SceneBuilder::Pins pins = item.get<1>();
    for (auto it = pins.begin(); it != pins.end(); ++it) {
        newVdf->getSceneModel()->addItem(*it);
    }

    pins = item.get<2>();
    for (auto it = pins.begin(); it != pins.end(); ++it) {
        newVdf->getSceneModel()->addItem(*it);
    }*/
}

void TypesWindow::addEntry( const QString& entry )
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, entry);
    treeWidget->addTopLevelItem(item);
}

SceneBuilder::VisualNodeWithPins TypesWindow::createItemByEntry( const QString& entry )
{
    SceneBuilder::VisualNodeWithPins item;
    SceneModelPtr scene = newVdf->getSceneModel();

    auto it = name2node.find(entry);
    if (it != name2node.end()) {
        auto node = it->second;
        core::IOutputDescriptionConstPtr output = core::dynamic_pointer_cast<const core::IOutputDescription>(node);
		core::IInputDescriptionConstPtr input = core::dynamic_pointer_cast<const core::IInputDescription>(node);
        
        int out = 0;
		if (output) {
			std::vector<core::IOutputDescription::OutputInfo> outputInfo;
			output->getOutputInfo(outputInfo);
			out = outputInfo.size();
		}
        int in = 0;
		if (input) {
			std::vector<core::IInputDescription::InputInfo> inputInfo;
			input->getInputInfo(inputInfo);
			in = inputInfo.size();
		}
        item = scene->getBuilder().createType(it->first, node->create(), in, out);
    }
    UTILS_ASSERT(item.get<0>() != nullptr);
    return item;
}

void TypesWindow::update( const core::IDataProcessorManager* pm )
{
    auto rProcessors = pm->enumPrototypes();
    for(auto it = rProcessors.begin(); it != rProcessors.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
            name2node[name] = *it;
            addEntry(name);
        }
    }
}

void TypesWindow::update( const core::IDataSourceManager* sm )
{
    auto rSources = sm->enumPrototypes();
    for(auto it = rSources.begin(); it != rSources.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
			core::IWorkflowItemBasePtr base = *it;
            name2node[name] = *it;
            addEntry(name);
        }
    }
}

void TypesWindow::update( const core::IDataSinkManager* sm )
{
    auto rSinks = sm->enumPrototypes();
    for(auto it = rSinks.begin(); it != rSinks.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
            name2node[name] = *it;
            addEntry(name);
        }
    }
}

void TypesWindow::run()
{
	this->newVdf->getSceneModel()->run();
}

