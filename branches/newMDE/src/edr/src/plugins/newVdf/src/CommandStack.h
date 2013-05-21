/********************************************************************
	created:	2013/03/12
	created:	12:3:2013   21:14
	filename: 	CommandStack.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__COMMANDSTACK_H__
#define HEADER_GUARD_NEWVDF__COMMANDSTACK_H__

#include <plugins/newVdf/ICommandStack.h>

namespace vdf {
			
//! Przechowuje polecenia i zarz�dza nimi. Umo�liwia standardowe operacje undo/redo.
class CommandStack : public QObject, public ICommandStack
{
    Q_OBJECT;
    friend class CommandStackDebug;
public:
    typedef std::list<ICommandPtr> Commands;
public:
    CommandStack();
    virtual ~CommandStack() {}

public:
	//! Dodaje polecenie do stosu i automatycznie wykonuje je.
	//! \param command dodawane polecenie
    void addCommand(ICommandPtr command);
	//! Cofa ostatnio wykonane polecenie.
    void undo();
	//! Ponownie wykonuje ostatnio cofni�te polecenie
    void redo();
	//! Sprawdza, czy na stosie jest cho� jedno wykonane polecenie
	//! \return Czy mo�na cofn�� efekt dzia�ania polecenia.
	bool isUndoPossible() const;
	//! Sprawdza, czy na stosie znajduje si� polecenie kt�re zosta�o cofni�te.
	//! \return Czy mo�na ponownie wykona� polecenie?
    bool isRedoPossible() const;
	//! Usuwa wszystkie polecenia ze stosu
    void clear();

signals:
	//! Stan Stosu zmieni� si�
    void changed();

private:
	//! Wszystkie agregowane polecenia
    Commands commands;
	//! Aktualne polecenie (ostatnio wykonane, za nim mog� znale�� si� polecenia cofni�te)
    Commands::iterator currentCommand;
};

DEFINE_SMART_POINTERS(CommandStack);
}

#endif
