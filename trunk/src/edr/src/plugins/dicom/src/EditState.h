/********************************************************************
	created:	2013/11/29
	created:	29:11:2013   10:12
	filename: 	EditState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__EDITSTATE_H__
#define HEADER_GUARD_DICOM__EDITSTATE_H__

#include <coreui/AbstractState.h>
#include "PointsLayer.h"
#include <utils/ICommand.h>
#include <coreUI/MoveCommand.h>
namespace dicom {

    //! Polecenie dodaje punkt do obszaru
    class AddPointCommand : public utils::ICommand
    {
    public:
        //! Konstruktor
        //! \param item Element graficzny sceny, które zostanie przesuniêty 
        //! \param newP Nowa pozycja elementu 
        //! \param oldP Stara pozycja elementu
        AddPointCommand(PointsLayerPtr layer, const QPointF& newP, bool bestFit);

    public:
        //! Przesuwa element
        virtual void doIt();

        //! Cofa przesuniêcie
        virtual void undoIt();
        //! Nazwa polecenia 
        virtual QString name();
        int getBestIdx();
        float getDistance2( const QPointF& p );
    private:
        //! Modyfikowany obszar
        PointsLayerPtr layer;
        //! Nowododany punkt
        QPointF newP;
        //! indeks punktu do usuniêcia
        int idx;
        /// usuniêty punkt, który mo¿e zostaæ przywrócony. Przywracaj¹c nie mo¿na stworzyæ nowej instancji
        /// gdy¿ mog³oby to spowodowaæ b³êdy np. przy MoveCommand
        std::unique_ptr<QGraphicsItem> removedPoint;
        bool bestFit;
    };

    //! Polecenie usuwa punkt z obszaru
    class RemovePointCommand : public utils::ICommand
    {
    public:
        //! Konstruktor
        //! \param item Element graficzny sceny, które zostanie przesuniêty 
        //! \param newP Nowa pozycja elementu 
        //! \param oldP Stara pozycja elementu
        RemovePointCommand(PointsLayerPtr layer, int idx);

    public:
        //! Przesuwa element
        virtual void doIt();

        //! Cofa przesuniêcie
        virtual void undoIt();
        //! Nazwa polecenia 
        virtual QString name();

    private:
        //! Modyfikowany obszar
        PointsLayerPtr layer;
        //! indeks punktu do usuniêcia
        int idx;
        /// usuniêty punkt, który mo¿e zostaæ przywrócony. Przywracaj¹c nie mo¿na stworzyæ nowej instancji
        /// gdy¿ mog³oby to spowodowaæ b³êdy np. przy MoveCommand
        std::unique_ptr<QGraphicsItem> removedPoint;
    };

    class MovePathCommand : public coreUI::MoveCommand 
    {
    public:
        MovePathCommand(PointsLayerPtr layer, QGraphicsItem* item, const QPointF& newP, const QPointF& oldP);

          virtual void doIt();

          virtual void undoIt();

    private:
        PointsLayerPtr layer;
    };


class LayeredStateMachine;

// TODO - EditState i PointsState maja duzo wspolnego kodu, 
// PointsState powinno dziedziczyc po EditState
class EditState : public QObject, public coreUI::AbstractState
{
    Q_OBJECT
public:
    EditState(LayeredStateMachine* machine);
	virtual ~EditState() {}
    friend class AddLayerCommand;
public:
    virtual bool keyReleaseEvent( QKeyEvent *event );
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual bool mouseReleaseEvent( QGraphicsSceneMouseEvent* e );
    virtual bool mousePressEvent( QGraphicsSceneMouseEvent* e );

    virtual bool focusOutEvent(QFocusEvent * event);
    virtual bool keyPressEvent( QKeyEvent* event );

    virtual void begin( coreUI::AbstractStateConstPtr lastState );
    virtual void end();

    dicom::PointsLayerPtr getLayerToEdit() const { return layerToEdit; }
    void setLayerToEdit(dicom::PointsLayerPtr val) { layerToEdit = val; }


    int getSelectedPointIdx() const;

private Q_SLOTS:
    void done();
    void deletePoint();

private:
    PointsLayerPtr layerToEdit;
    std::vector<std::pair<QGraphicsItem*, QPointF>> positionsToCheck;
    PointsLayerPtr layer;
    LayeredStateMachine* machine;
    bool possibleMove;
    int delPointIdx;
};
DEFINE_SMART_POINTERS(EditState);

}
#endif
