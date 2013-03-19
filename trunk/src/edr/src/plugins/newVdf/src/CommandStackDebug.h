/********************************************************************
	created:	2013/01/23
	created:	23:1:2013   20:41
	filename: 	CommandStackDebug.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__COMMANDSTACKDEBUG_H__
#define HEADER_GUARD_NEWVDF__COMMANDSTACKDEBUG_H__

#include <QtGui/QListWidget>
#include "Command.h"

namespace vdf {

class CommandStackDebug : public QListWidget
{
	Q_OBJECT;
public:
	CommandStackDebug(CommandStackPtr stack, QWidget* parent = nullptr);
	virtual ~CommandStackDebug() {}

public slots:
	void refresh();

private:
	CommandStackPtr commandStack;
};
typedef core::shared_ptr<CommandStackDebug> CommandStackDebugPtr;
typedef core::shared_ptr<const CommandStackDebug> CommandStackDebugConstPtr;
}

#endif
