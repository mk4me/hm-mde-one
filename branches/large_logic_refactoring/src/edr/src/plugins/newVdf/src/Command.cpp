#include "NewVdfPCH.h"
#include "Command.h"
#include "VdfScene.h"

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

AddConnectionCommand::AddConnectionCommand( SceneModelPtr scene, IVisualPinPtr p1, IVisualPinPtr p2 ) :
	sceneModel(scene),
	pin1(p1),
	pin2(p2)
{

}

void AddConnectionCommand::doIt()
{
	sceneModel->addConnection(pin1, pin2);
}

void AddConnectionCommand::undoIt()
{
	sceneModel->removeConnection(pin1, pin2);
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

RemoveCommand::RemoveCommand(SceneModelPtr scene, IVisualItemPtr toRemove ) : 
	item(toRemove),
	sceneModel(scene)
{

}

void RemoveCommand::doIt()
{
	removedConnections.clear();
	IVisualSourceNodePtr source = core::dynamic_pointer_cast<IVisualSourceNode>(item);
	if (source) {
		int count = source->getNumOutputPins();
		for (int i = 0; i < count; i++) {
			IVisualPinPtr pin = source->getOutputPin(i);
			removeConnectionFromPin(pin);
		}
	}

	IVisualSinkNodePtr sink = core::dynamic_pointer_cast<IVisualSinkNode>(item);
	if (sink) {
		int count = sink->getNumInputPins();
		for (int i = 0; i < count; i++) {
			IVisualPinPtr pin = sink->getInputPin(i);
			removeConnectionFromPin(pin);
		}
	}

	sceneModel->removeItem(item);
}

void RemoveCommand::undoIt()
{
	sceneModel->addItem(item);
	for (auto it = removedConnections.begin(); it != removedConnections.end(); ++it) {
		sceneModel->addConnection((*it)->getBegin(), (*it)->getEnd());
	}
	removedConnections.clear();
}

void RemoveCommand::removeConnectionFromPin( IVisualPinPtr pin )
{
	IVisualConnectionPtr connection = pin->getConnection().lock();
	if (connection) {
		sceneModel->removeItem(connection);
		removedConnections.push_back(connection);
	}
}
