/********************************************************************
	created:	2013/11/23
	created:	23:11:2013   21:44
	filename: 	MoveCommand.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__MOVECOMMAND_H__
#define HEADER_GUARD_COREUI__MOVECOMMAND_H__

#include <QtCore/QPointF>
#include <utils/ICommand.h>
#include <coreui/Export.h>

class QGraphicsItem;

namespace coreUI {
    
    //! Polecenie przesuwa elementy na scenie
    class COREUI_EXPORT MoveCommand : public utils::ICommand
    {
    public:
        //! Konstruktor
        //! \param item Element graficzny sceny, kt�re zostanie przesuni�ty 
        //! \param newP Nowa pozycja elementu 
        //! \param oldP Stara pozycja elementu
        MoveCommand(QGraphicsItem* item, const QPointF& newP, const QPointF& oldP);

    public:
        //! Przesuwa element
        virtual void doIt();
        //! Cofa przesuni�cie
        virtual void undoIt();
        //! Nazwa polecenia 
        virtual QString name() { return QString(typeid(this).name()); }

    private:
        //! Element graficzny sceny, kt�re zostanie przesuni�ty 
        QGraphicsItem* item;
        //! Nowa pozycja elementu graficznego
        QPointF newPosition;
        //! Stara pozycja elementu graficznego
        QPointF oldPosition;
    };
    DEFINE_SMART_POINTERS(MoveCommand);
}

#endif
