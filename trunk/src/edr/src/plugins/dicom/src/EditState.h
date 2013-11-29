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
        //! \param item Element graficzny sceny, kt�re zostanie przesuni�ty 
        //! \param newP Nowa pozycja elementu 
        //! \param oldP Stara pozycja elementu
        AddPointCommand(PointsLayerPtr layer, const QPointF& newP);

    public:
        //! Przesuwa element
        virtual void doIt();

        //! Cofa przesuni�cie
        virtual void undoIt();
        //! Nazwa polecenia 
        virtual QString name();

    private:
        //! Modyfikowany obszar
        PointsLayerPtr layer;
        //! Nowododany punkt
        const QPointF& newP;
        //! indeks punktu do usuni�cia
        int idx;
        /// usuni�ty punkt, kt�ry mo�e zosta� przywr�cony. Przywracaj�c nie mo�na stworzy� nowej instancji
        /// gdy� mog�oby to spowodowa� b��dy np. przy MoveCommand
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

   /* class AddLayerCommand : public utils::ICommand
    {
    public:
        AddLayerCommand(LayeredStateMachine* machine, ILayeredImagePtr image, PointsState* pointsState, PointsLayerPtr layer, int adnotationIdx);

    public:
        virtual void doIt();

        virtual void undoIt();

        virtual QString name();

    private:
        PointsLayerPtr layer;
        int adnotationIdx;
        ILayeredImagePtr image;
        LayeredStateMachine* machine;
        PointsState* pointsState;
    };*/


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


    virtual void begin( coreUI::AbstractStateConstPtr lastState );
    virtual void end();

    dicom::PointsLayerPtr getLayerToEdit() const { return layerToEdit; }
    void setLayerToEdit(dicom::PointsLayerPtr val) { layerToEdit = val; }

private Q_SLOTS:
    //void clear();
    void done();

    //void resetLayer();

    //void addLayer();

    

private:
    PointsLayerPtr layerToEdit;
    /*std::vector<QGraphicsItem*> points;
    std::vector<QGraphicsItem*> connections;*/
    std::vector<std::pair<QGraphicsItem*, QPointF>> positionsToCheck;
    PointsLayerPtr layer;
    LayeredStateMachine* machine;
    bool possibleMove;
};
DEFINE_SMART_POINTERS(EditState);

}
#endif
