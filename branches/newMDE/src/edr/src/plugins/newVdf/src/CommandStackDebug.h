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
#include "CommandStack.h"

namespace vdf {

//! Klasa przedstawia stos wywo³añ poleceñ
class CommandStackDebug : public QListWidget
{
	Q_OBJECT;
public:
	//! Konstruktor
	//! \param stack Stos z poleceniami
	CommandStackDebug(CommandStackPtr stack, QWidget* parent = nullptr);
	virtual ~CommandStackDebug() {}

public slots:
	//! Odrysowanie stosu
	void refresh();

private:
	//! Stos z poleceniami
	CommandStackPtr commandStack;
};
DEFINE_SMART_POINTERS(CommandStackDebug);
}

#endif
