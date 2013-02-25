#include "NewVdfPCH.h"
#include "Command.h"
#include "VdfScene.h"

using namespace vdf;

void AddToSceneCommand::doIt()
{
	auto node = nodeWithPins.get<0>();
	auto vis = node->visualItem();
	vis->setPos(position);

	sceneModel->addItem(node);

	SceneBuilder::Pins pins = nodeWithPins.get<1>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		sceneModel->addItem(*it);
	}
	
	pins = nodeWithPins.get<2>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		sceneModel->addItem(*it);
	}
}

void AddToSceneCommand::undoIt()
{
	/*SceneBuilder::Pins pins = nodeWithPins.get<2>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		sceneModel->removeItem(*it);
	}

	pins = nodeWithPins.get<1>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		sceneModel->removeItem(*it);
	}*/
	sceneModel->removeItem(nodeWithPins.get<0>());
}

AddToSceneCommand::AddToSceneCommand(SceneModelPtr scene, const SceneBuilder::VisualNodeWithPins& node, const QPointF& scenePos) :
	sceneModel(scene),
	nodeWithPins(node),
	position(scenePos)
{

}

QString AddToSceneCommand::name()
{
	return QString(typeid(this).name());
}

void CommandStack::addCommand( ICommandPtr command )
{
	command->doIt();
	if (isRedoPossible()) {
		commands.erase(++currentCommand, commands.end());
	}
	commands.push_back(command);
	currentCommand = --commands.end();
	emit changed();
}

void CommandStack::undo()
{
	if (isUndoPossible()) {
		(*currentCommand)->undoIt();
		--currentCommand;
		//commands.back()->undoIt();
		//commands.pop_back();
		emit changed();
	}
}

void CommandStack::redo()
{
	if (isRedoPossible()) {
		currentCommand++;
		(*currentCommand)->doIt();
		emit changed();
	}
}

void CommandStack::clear()
{
    commands.clear();
    currentCommand = commands.end();
}


bool CommandStack::isRedoPossible() const
{
	return	!commands.empty() && 
			currentCommand != commands.end() && 
			currentCommand != (--commands.end());
}

CommandStack::CommandStack() 
{
	currentCommand = commands.end();
}

bool CommandStack::isUndoPossible() const
{
	return	!commands.empty() && 
		currentCommand != commands.begin();
}

AddConnectionCommand::AddConnectionCommand( SceneModelPtr scene, IVisualOutputPinPtr p1, IVisualInputPinPtr p2 ) :
	sceneModel(scene),
	outputPin(p1),
	inputPin(p2)
{

}

void AddConnectionCommand::doIt()
{
    if (!connection) {
        connection = sceneModel->addConnection(outputPin, inputPin);
    } else {
        UTILS_ASSERT(connection->getInputPin() && connection->getOutputPin());
        sceneModel->addItem(connection);
    }
}

void AddConnectionCommand::undoIt()
{
	sceneModel->removeItem(connection);
}

MultiCommand::MultiCommand( const std::vector<ICommandPtr>& c ) :
	commands(c) 
{

}

void MultiCommand::doIt()
{
	for (auto it = commands.begin(); it != commands.end(); ++it) {
		(*it)->doIt();
	}
}

void MultiCommand::undoIt()
{
	for (int i = commands.size() - 1; i >= 0; --i) {
		commands[i]->undoIt();
	}
}

MoveCommand::MoveCommand( QGraphicsItem* item, const QPointF& newP, const QPointF& oldP ) :
	item(item),
	newPosition(newP),
	oldPosition(oldP)
{

}

void MoveCommand::doIt()
{
	item->setPos(newPosition);
}

void MoveCommand::undoIt()
{
	item->setPos(oldPosition);
}

RemoveNodeCommand::RemoveNodeCommand(SceneModelPtr scene, IVisualNodePtr toRemove ) : 
	item(toRemove),
	sceneModel(scene)
{

}

void RemoveNodeCommand::doIt()
{
	removedConnections.clear();
	IVisualSourceNodePtr source = core::dynamic_pointer_cast<IVisualSourceNode>(item);
	if (source) {
		int count = source->getNumOutputPins();
		for (int i = 0; i < count; i++) {
			IVisualOutputPinPtr pin = source->getOutputPin(i);
			removeConnectionFromPin(pin);
			removedPins.push_back(pin);
		}
	}

	IVisualSinkNodePtr sink = core::dynamic_pointer_cast<IVisualSinkNode>(item);
	if (sink) {
		int count = sink->getNumInputPins();
		for (int i = 0; i < count; i++) {
			IVisualInputPinPtr pin = sink->getInputPin(i);
			removeConnectionFromPin(pin);
			removedPins.push_back(pin);
		}
	}

	sceneModel->removeItem(item);
}

void RemoveNodeCommand::undoIt()
{
	sceneModel->addItem(item);
	for (auto it = removedPins.begin(); it != removedPins.end(); ++it) {
		sceneModel->addItem(*it);
	}
	removedPins.clear();
	for (auto it = removedConnections.begin(); it != removedConnections.end(); ++it) {
		sceneModel->addConnection((*it)->getOutputPin(), (*it)->getInputPin());
	}
	removedConnections.clear();
}

void RemoveNodeCommand::removeConnection( IVisualConnectionPtr connection ) 
{
    if (connection) {
        sceneModel->removeItem(connection);
        removedConnections.push_back(connection);
    }
}

void RemoveNodeCommand::removeConnectionFromPin( IVisualInputPinPtr pin )
{
	IVisualConnectionPtr connection = pin->getConnection().lock();
    removeConnection(connection);

}

void vdf::RemoveNodeCommand::removeConnectionFromPin( IVisualOutputPinPtr pin )
{
    int count = pin->getNumConnections();
    for (int i = 0; i < count; ++i) {
        IVisualConnectionPtr connection = pin->getConnection(i).lock();
        removeConnection(connection);
    }
    
}

vdf::RemoveSelectedCommand::RemoveSelectedCommand( SceneModelPtr scene, const QList<QGraphicsItem*> selectedItems ) :
	items(selectedItems),
    sceneModel(scene)
{

}

void vdf::RemoveSelectedCommand::doIt()
{
	auto nodes = sceneModel->getVisualItems<IVisualNodePtr>(items);
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto command = ICommandPtr(new RemoveNodeCommand(sceneModel, *it));
        command->doIt();
		commands.push_back(command);
	}
    auto connections = sceneModel->getVisualItems<IVisualConnectionPtr>(items);
    for (auto it = connections.begin(); it != connections.end(); ++it) {
        auto command = ICommandPtr(new RemoveConnectionCommand(sceneModel, *it));
        command->doIt();
        commands.push_back(command);
    }
}

void vdf::RemoveSelectedCommand::undoIt()
{
    for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
        (*it)->undoIt();
    }
    commands.clear();
}

void vdf::RemoveConnectionCommand::doIt()
{
    sceneModel->removeItem(item);
}

void vdf::RemoveConnectionCommand::undoIt()
{
    //sceneModel->addConnection(item->getOutputPin(), item->getInputPin());
    sceneModel->addItem(item);
}

vdf::RemoveConnectionCommand::RemoveConnectionCommand( SceneModelPtr scene, IVisualConnectionPtr toRemove ) :
    item(toRemove),
    sceneModel(scene)
{

}




