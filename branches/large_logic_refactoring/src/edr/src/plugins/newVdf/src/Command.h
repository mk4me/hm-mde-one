/********************************************************************
	created:	2012/12/26
	created:	26:12:2012   22:37
	filename: 	Command.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__COMMAND_H__
#define HEADER_GUARD_NEWVDF__COMMAND_H__

#include <core/SmartPtr.h>
#include <QtCore/QObject>
#include <QtCore/QPointF>

#include "SceneBuilder.h"
#include "SceneModel.h"

class ICommand
{
public:
	virtual ~ICommand() {}
    virtual void doIt() = 0;
    virtual void undoIt() = 0;
	virtual QString name() = 0; 
};
typedef core::shared_ptr<ICommand> ICommandPtr;
typedef core::shared_ptr<const ICommand> ICommandConstPtr;

class CommandStack : public QObject
{
	Q_OBJECT;
	friend class CommandStackDebug;
public:
	typedef std::list<ICommandPtr> Commands;
public:
	CommandStack();
    virtual ~CommandStack() {}

public:
    void addCommand(ICommandPtr command);
	void undo();
	void redo();
	bool isUndoPossible() const;
	bool isRedoPossible() const;

signals:
	void changed();

private:
    Commands commands;
	Commands::iterator currentCommand;
};
typedef core::shared_ptr<CommandStack> CommandStackPtr;
typedef core::shared_ptr<const CommandStack> CommandStackConstPtr;

class NullCommand : public ICommand
{

public:
	virtual void doIt() {}
	virtual void undoIt() {}
	virtual QString name() { return QString(typeid(this).name()); }
};

class IVisualItem;

class AddToSceneCommand : public ICommand
{
public:
	AddToSceneCommand(SceneModelPtr scene, const SceneBuilder::VisualNodeWithPins& node, const QPointF& scenePos);
public:
	virtual void doIt();
	virtual void undoIt();
	virtual QString name();
private:
	SceneModelPtr sceneModel;
	SceneBuilder::VisualNodeWithPins nodeWithPins;
	QPointF position;
};

class AddConnectionCommand : public ICommand
{
public:
	AddConnectionCommand(SceneModelPtr scene, IVisualPinPtr p1, IVisualPinPtr p2);
public:
	virtual void doIt();
	virtual void undoIt();
	virtual QString name() { return QString(typeid(this).name()); }
private:
	SceneModelPtr sceneModel;
	IVisualPinPtr pin1;
	IVisualPinPtr pin2;
};

class MoveCommand : public ICommand
{
public:
	MoveCommand(QGraphicsItem* item, const QPointF& newP, const QPointF& oldP);

public:
	virtual void doIt();
	virtual void undoIt();
	virtual QString name() { return QString(typeid(this).name()); }

private:
	QGraphicsItem* item;
	QPointF newPosition;
	QPointF oldPosition;
};

class MultiCommand : public ICommand
{
public:
	MultiCommand(const std::vector<ICommandPtr>& c);
public:
	virtual void doIt();
	virtual void undoIt();
	virtual QString name() { return QString(typeid(this).name()); }

private:
	std::vector<ICommandPtr> commands;
};


class RemoveCommand : public ICommand
{
public:
	RemoveCommand(SceneModelPtr scene, IVisualItemPtr toRemove);
public:
	virtual void doIt();
	virtual void undoIt();
	virtual QString name() { return QString(typeid(this).name()); }

private:
	void removeConnectionFromPin( IVisualPinPtr pin );

private:
	IVisualItemPtr item;
	SceneModelPtr sceneModel;
	std::list<IVisualConnectionPtr> removedConnections;
};


#endif
