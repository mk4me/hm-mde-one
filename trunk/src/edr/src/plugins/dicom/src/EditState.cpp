#include "DicomPCH.h"
#include "EditState.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QMenu>
#include <coreui/AbstractStateMachine.h>
#include <coreui/MoveCommand.h>
#include <coreUi/MultiCommand.h>
#include "LayeredStateMachine.h"
#include "BackgroundLayer.h"
#include "LayeredSerie.h"
#include "PointsLayer.h"
#include "Adnotations.h"


using namespace dicom;


dicom::EditState::EditState( LayeredStateMachine* machine ) :
    coreUI::AbstractState(machine),
    machine(machine),
    possibleMove(false),
    delPointIdx(-1)
{

}


bool dicom::EditState::keyReleaseEvent( QKeyEvent *event )
{
    return true;
}

bool dicom::EditState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::RightButton) {
        QMenu menu;

        QAction* doneAction = menu.addAction(tr("Done"));
        connect(doneAction, SIGNAL(triggered()), this, SLOT(done()));
        auto item = machine->getGraphicsScene()->itemAt(e->scenePos());
        if (layer->hasPoint(item)) {
            delPointIdx = layer->getPointIdx(item);
            QAction* deleteAction = menu.addAction(tr("Delete point"));
            connect(deleteAction, SIGNAL(triggered()), this, SLOT(deletePoint()));
        }

		bool enable = machine->getSerie()->editionEnable();
		auto actions = menu.actions();
		for(auto it = actions.begin(); it != actions.end(); ++it){
			(*it)->setEnabled(enable);
		}

        menu.exec(e->screenPos());
    } else if (e->button() == Qt::LeftButton) {
        auto item = machine->getGraphicsScene()->itemAt(e->scenePos());
        if (layer->hasPoint(item)) {
            possibleMove = true;
            positionsToCheck.clear();
            int count = layer->getNumPoint();
            for (int i = 0; i < count; ++i) {
                auto pointItem = layer->getPoint(i);
                positionsToCheck.push_back(std::make_pair(pointItem, pointItem->scenePos()));
            }
            return false;
        } else {
            auto pos = e->scenePos();
            auto offs = layer->getItem()->pos();
            machine->getCommandStack()->addCommand(utils::make_shared<AddPointCommand>(layer, pos - offs, true));
        }
    }

    return true;
}

void dicom::EditState::begin( coreUI::AbstractStateConstPtr lastState )
{
    layer = getLayerToEdit();
    layer->setSelected(false);
    layer->setEditable(true);
}

void dicom::EditState::end()
{
    //done();
    layer->setEditable(false);
}

bool dicom::EditState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    possibleMove = false;

    std::vector<utils::ICommandPtr> commands;
    for (auto it = positionsToCheck.begin(); it != positionsToCheck.end(); ++it) {
        QGraphicsItem* item = it->first;
        QPointF oldP = it->second;
        QPointF newP = item->pos();
        if (newP != oldP) {
            commands.push_back(utils::ICommandPtr(new MovePathCommand(layer, item, newP, oldP)));
        }
    }
    if (!commands.empty()) {
        machine->getCommandStack()->addCommand(utils::ICommandPtr(new coreUI::MultiCommand(commands)));
    }
    return false;
}

bool dicom::EditState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    if (possibleMove) {
        layer->refresh();
    }
    return false;
}

void dicom::EditState::done()
{
    machine->setState(machine->getNormalState());
}

QString dicom::AddPointCommand::name()
{
    return QString(typeid(this).name());
}

void dicom::AddPointCommand::undoIt()
{
    auto point = layer->removePoint(idx);
    removedPoint = std::unique_ptr<QGraphicsItem>(point);
    removedPoint->setVisible(false);
    layer->refresh();
}

void dicom::AddPointCommand::doIt()
{
    if (removedPoint) {
        removedPoint->setVisible(true);
        layer->addPoint(removedPoint.release());
        removedPoint = std::unique_ptr<QGraphicsItem>();
    } else {
        idx = bestFit ? getBestIdx() : layer->getNumPoint();
        layer->addPoint(newP, idx);
    }

    layer->refresh();
}

dicom::AddPointCommand::AddPointCommand( PointsLayerPtr layer, const QPointF& newP, bool bestFit ) : 
layer(layer),
    newP(newP),
    idx(-1),
    removedPoint(nullptr),
    bestFit(bestFit)
{

}

float magnitude(const QPointF &l1,  const QPointF &l2)
{
    QPointF ofs = l2 - l1;
    return sqrt(ofs.x() * ofs.x() + ofs.y() * ofs.y());
}


float distancePointLine( const QPointF &p, const QPointF &l1,  const QPointF &l2)
{
    float lineMag;
    float u;
    QPoint intersectPnt;

    lineMag = magnitude( l2, l1 );

    u = ( ( ( p.x() - l1.x() ) * ( l2.x() - l1.x() ) ) +
        ( ( p.y() - l1.y() ) * ( l2.y() - l1.y() ) ) ) /
        ( lineMag * lineMag );

    if ( u < 0.0f) {
        return magnitude(p, l1);
    }
    if ( u > 1.0f ) {
        return magnitude(p, l2);
    }

    intersectPnt.setX( int( l1.x() + u * ( l2.x() - l1.x() ) ) );
    intersectPnt.setY( int( l1.y() + u * ( l2.y() - l1.y() ) ) );

    return magnitude( p, intersectPnt );
}

int dicom::AddPointCommand::getBestIdx()
{
    int count = layer->getNumPoint();
    if (count < 2) {
        return count;
    } 
    float minDist2 = std::numeric_limits<float>::max();
    int minIdx = -1;
    for (int i =  0; i < count - 1; ++i) {

        float dist2 = distancePointLine(newP, layer->getPoint(i)->pos(), layer->getPoint(i + 1)->pos());
        if (dist2 < minDist2) {
            minDist2 = dist2;
            minIdx = i + 1;
        }
    }
    float endD = sqrt(getDistance2(layer->getPoint(count - 1)->pos()));
    float startD = sqrt(getDistance2(layer->getPoint(0)->pos()));
    if (endD - minDist2 < 0.001f ) {
        minIdx = count;
        minDist2 = endD;
    } 
    
    if (startD - minDist2 < 0.001f ) {
        minIdx = 0;
        minDist2 = startD;
    }
    return minIdx;
}

float dicom::AddPointCommand::getDistance2( const QPointF& p )
{
    QPointF ofs = p - newP;
    return ofs.x() * ofs.x() + ofs.y() * ofs.y();
}

//////////////////////////////////////////////////////////////////////////

QString dicom::RemovePointCommand::name()
{
    return QString(typeid(this).name());
}

void dicom::RemovePointCommand::undoIt()
{
    removedPoint->setVisible(true);
    layer->addPoint(removedPoint.release(), idx);
}

void dicom::RemovePointCommand::doIt()
{
    auto point = layer->removePoint(idx);
    removedPoint = std::unique_ptr<QGraphicsItem>(point);
    removedPoint->setVisible(false);
    layer->refresh();
}

dicom::RemovePointCommand::RemovePointCommand( PointsLayerPtr layer, int idx ) : 
    layer(layer),
    idx(idx),
    removedPoint(nullptr)
{

}

void dicom::MovePathCommand::undoIt()
{
    MoveCommand::undoIt();
    layer->refresh();
}

void dicom::MovePathCommand::doIt()
{
    MoveCommand::doIt();
    layer->refresh();
}

dicom::MovePathCommand::MovePathCommand( PointsLayerPtr layer, QGraphicsItem* item, const QPointF& newP, const QPointF& oldP ) : 
MoveCommand(item, newP, oldP),
    layer(layer)
{

}



bool dicom::EditState::keyPressEvent( QKeyEvent* event )
{
    if (event->key() == Qt::Key_Delete) {
        delPointIdx = getSelectedPointIdx();
        deletePoint();
    }

    return false;
}

int dicom::EditState::getSelectedPointIdx() const
{
    int count = layer->getNumPoint();
    int idx = -1;
    for (int i = 0; i < count; i++) {
        if (layer->getPoint(i)->isSelected()) {
            if (idx == -1) {
                idx = i;
            } else {
                return -1;
            }
        }
    }

    return idx;
}

void dicom::EditState::deletePoint()
{
    if (delPointIdx != -1) {
        machine->getCommandStack()->addCommand(utils::make_shared<RemovePointCommand>(layer, delPointIdx));
        delPointIdx = -1;
    } else {
        UTILS_ASSERT(false);
    }
}

bool dicom::EditState::focusOutEvent( QFocusEvent * event )
{
    machine->setState(machine->getNormalState());
    return true;
}

