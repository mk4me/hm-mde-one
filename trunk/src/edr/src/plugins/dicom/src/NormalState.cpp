#include "DicomPCH.h"
#include "NormalState.h"
#include "PointsState.h"
#include <coreui/AbstractStateMachine.h>
#include <QtGui/QGraphicsSceneMouseEvent>
#include "LayeredStateMachine.h"
#include <coreUI/MoveCommand.h>
#include "LayeredSerie.h"
#include <QtGui/QMenu>

using namespace dicom;

dicom::NormalState::NormalState( LayeredStateMachine* machine) :
    coreUI::AbstractState(machine),
    machine(machine)
{

}

void NormalState::selectionChanged(const QList<QGraphicsItem*>& list)
{
    if (item2layer.empty()) {
        begin();
    }
    for (auto it = list.begin(); it != list.end(); ++it) {
        QGraphicsItem* item = extractItem(*it);
        auto layerIt = item2layer.find(item);
        if (layerIt != item2layer.end()) {
            (layerIt->second)->setSelected(true);
        }
        machine->getSerie()->getLayersModel()->refresh();
    }
}

bool NormalState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
//    position.first = nullptr;
    if (e->button() == Qt::LeftButton) {
       // auto item = extractItem(e);
       // if (item) {
       //     position = std::make_pair(item, item->pos());
       // }
    } else if (e->button() == Qt::RightButton) {
        QMenu menu;

        int numSelected = getNumSelected();
        if (numSelected == 0) {
            QAction* curveAction = menu.addAction(tr("Add curve"));
            connect(curveAction, SIGNAL(triggered()), this, SLOT(addCurve()));

            QAction* polyAction = menu.addAction(tr("Add poly"));
            connect(polyAction, SIGNAL(triggered()), this, SLOT(addPoly()));
        } else if (numSelected == 1) {
            QAction* moveAction = menu.addAction(tr("Move"));
            connect(moveAction, SIGNAL(triggered()), this, SLOT(move()));

            QAction* editAction = menu.addAction(tr("Edit"));
            connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));
        }
        menu.exec(e->screenPos());
    }
    // celowo zwracany jest false, nawet gdy obs³u¿yliœmy event (domyœlna obs³uga jest w tym przypadku porz¹dana)
    return false;
}

bool NormalState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    /*if (e->button() == Qt::LeftButton) {
        if (position.first != nullptr) {
            QGraphicsItem* item = position.first;
            QPointF oldP = position.second;
            QPointF newP = item->pos();
            machine->getCommandStack()->addCommand(utils::make_shared<coreUI::MoveCommand>(item, newP, oldP));
        }
    }*/
    return false;
}

bool NormalState::keyReleaseEvent( QKeyEvent *event )
{
    return false;
}


void NormalState::end()
{
//    position.first = nullptr;
//    item2layer.clear();
}

QGraphicsItem* NormalState::extractItem( QGraphicsSceneMouseEvent* e )
{
    QGraphicsItem* itm = machine->getGraphicsScene()->itemAt(e->scenePos());
    return extractItem(itm);
}


QGraphicsItem* dicom::NormalState::extractItem( QGraphicsItem* itm )
{
    if (itm) {
        QGraphicsItemGroup* grp = dynamic_cast<QGraphicsItemGroup*>(itm->parentItem());
        if (grp) {
            return grp;
        }
    }

    return itm;
}

void dicom::NormalState::begin()
{
    createItem2LayerMap();

}

bool dicom::NormalState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    return true;
}

void dicom::NormalState::createItem2LayerMap()
{
    ILayeredImagePtr image = machine->getSerie()->getImage();
    int count = image->getNumLayers();
    for (int i = 0; i < count; ++i) {
        ILayerItemPtr vl = image->getLayer(i);
        if (vl) {
            item2layer[vl->getItem()] = vl;
        }
    }
}

int dicom::NormalState::getNumSelected()
{
    int selected = 0;
    ILayeredImagePtr image = machine->getSerie()->getImage();
    int count = image->getNumLayers();
    for (int i = 0; i < count; ++i) {
        ILayerItemPtr vl = image->getLayer(i);
        if (vl->getSelected()) {
            ++selected;
        }
    }

    return selected;
}

dicom::ILayerItemPtr dicom::NormalState::getFirstSelected()
{
    int selected = 0;
    ILayeredImagePtr image = machine->getSerie()->getImage();
    int count = image->getNumLayers();
    for (int i = 0; i < count; ++i) {
        ILayerItemPtr vl = image->getLayer(i);
        if (vl->getSelected()) {
            return vl;
        }
    }

    return ILayerItemPtr();
}

void dicom::NormalState::move()
{
    machine->setState(machine->getMoveState());
}

void dicom::NormalState::addCurve()
{
    machine->setState(machine->getCurveState());
}

void dicom::NormalState::addPoly()
{
    machine->setState(machine->getPolyState());
}

void dicom::NormalState::edit()
{
    PointsLayerPtr points = utils::dynamic_pointer_cast<PointsLayer>(getFirstSelected());
    if (points) {
        machine->getEditState()->setLayerToEdit(points);
        machine->setState(machine->getEditState());
    }
}
