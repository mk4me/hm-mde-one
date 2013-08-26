#include "NewVdfPCH.h"
#include "TypesWindow.h"
#include "CanvasStyleEditor.h"
#include "NewVdfWidget.h"
#include <plugins/newVdf/IDataSourceManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSource.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <corelib/PluginCommon.h>

using namespace vdf;

TypesWindow::TypesWindow(utils::ICommandStackPtr stack, CanvasStyleEditorPtr canvas, SceneModelPtr sceneModel, QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    canvas(canvas),
    sceneModel(sceneModel),
	commmandStack(stack)
{
    setupUi(this);
    
    processorsList->setSelectionMode(QAbstractItemView::SingleSelection);
    processorsList->setDragEnabled(true);

    sinksList->setSelectionMode(QAbstractItemView::SingleSelection);
    sinksList->setDragEnabled(true);

    sourcesList->setSelectionMode(QAbstractItemView::SingleSelection);
    sourcesList->setDragEnabled(true);
}

void TypesWindow::insert( const QString& name, const QPointF& scenePos )
{
    auto item = createItemByEntry(name);
	commmandStack->addCommand(utils::ICommandPtr(new AddToSceneCommand(sceneModel, item, scenePos)));
}

void TypesWindow::addEntry( const QString& entry, const QIcon& icon, IVisualItem::Type type )
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(entry);

    item->setSizeHint(QSize(64, 64));

    switch (type)  {
    case IVisualItem::ProcessingNode:
        item->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/processor.png"): icon);
        processorsList->addItem(item);
        break;

    case IVisualItem::SourceNode:
        item->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/source.png") : icon);
        sourcesList->addItem(item);
        break;

    case IVisualItem::SinkNode:
        item->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/sink.png") : icon);
        sinksList->addItem(item);
        break;

    default:
        UTILS_ASSERT(false);
    }
}

SceneBuilder::VisualNodeWithPins TypesWindow::createItemByEntry( const QString& entry )
{
    SceneBuilder::VisualNodeWithPins item;
    
    auto it = name2node.find(entry);
    if (it != name2node.end()) {
        auto node = it->second;
        IOutputDescriptionConstPtr output = utils::dynamic_pointer_cast<const IOutputDescription>(node);
		IInputDescriptionConstPtr input = utils::dynamic_pointer_cast<const IInputDescription>(node);
        
        int out = 0;
		if (output) {
			std::vector<IOutputDescription::OutputInfo> outputInfo;
			output->getOutputInfo(outputInfo);
			out = outputInfo.size();
		}
        int in = 0;
		if (input) {
			std::vector<IInputDescription::InputInfo> inputInfo;
			input->getInputInfo(inputInfo);
			in = inputInfo.size();
		}
        item = sceneModel->getBuilder().createType(it->first, node->getIcon(), node->create(), in, out);
    }
    UTILS_ASSERT(item.get<0>() != nullptr);
    return item;
}

void TypesWindow::update( const IDataProcessorManager* pm )
{
    auto rProcessors = pm->enumPrototypes();
    for(auto it = rProcessors.begin(); it != rProcessors.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
            name2node[name] = *it;
            addEntry(name, (*it)->getIcon(), IVisualItem::ProcessingNode);
        }
    }
}

void TypesWindow::update( const IDataSourceManager* sm )
{
    auto rSources = sm->enumPrototypes();
    for(auto it = rSources.begin(); it != rSources.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
			IWorkflowItemBasePtr base = *it;
            name2node[name] = *it;
            addEntry(name, (*it)->getIcon(),IVisualItem::SourceNode);
        }
    }
}

void TypesWindow::update( const IDataSinkManager* sm )
{
    auto rSinks = sm->enumPrototypes();
    for(auto it = rSinks.begin(); it != rSinks.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
            name2node[name] = *it;
            addEntry(name, (*it)->getIcon(), IVisualItem::SinkNode);
        }
    }
}

