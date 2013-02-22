/********************************************************************
	created:	2012/12/26
	created:	26:12:2012   22:37
	filename: 	Command.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__ICOMMAND_H__
#define HEADER_GUARD_NEWVDF__ICOMMAND_H__

#include <corelib/SmartPtr.h>
#include <QtCore/QObject>
#include <QtCore/QPointF>

namespace vdf {

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

}

#endif
