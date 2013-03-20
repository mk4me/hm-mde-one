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

//! podstawowy interfejs reprezentuj�cy pojedyncze polecenie (zgodnie ze wzorcem)
class ICommand
{
public:
	virtual ~ICommand() {}

public:
    //! metoda wykonuje polecenie
    virtual void doIt() = 0;
    //! cofa wykonane ju� polecenie
    virtual void undoIt() = 0;
	//! \return nazwa polecenia (dla ewentualnej reprezentacji stosu polece�)
	virtual QString name() = 0; 
};
typedef core::shared_ptr<ICommand> ICommandPtr;
typedef core::shared_ptr<const ICommand> ICommandConstPtr;
}

#endif
