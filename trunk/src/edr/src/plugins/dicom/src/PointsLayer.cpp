#include "DicomPCH.h"
#include "PointsLayer.h"
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPen>
#include <qwt_curve_fitter.h>
#include "boost/serialization/export.hpp"


dicom::PointsLayer::PointsLayer() :
    group (new QGraphicsItemGroup()),
    pathItem(new QGraphicsPathItem()),
    name("VPoints")
{
    pathItem->setParentItem(group.get());
    group->setHandlesChildEvents(false);
}

dicom::PointsLayer::~PointsLayer()
{
}

QGraphicsItem* dicom::PointsLayer::getItem()
{
    return group.get();
}

QString dicom::PointsLayer::getName() const
{
    return name;
}


QSize dicom::PointsLayer::getSize() const
{
    QSize s(0, 0);
    for (auto it = points.begin(); it != points.end(); ++it) {
        QPointF pos = (*it)->scenePos();
        int w = pos.x();
        int h = pos.y();
        if (w > s.width()) {
            s.setWidth(w);
        }

        if (h > s.height()) {
            s.setHeight(h);
        }
    }

    return s;
}

void dicom::PointsLayer::addPoint( const QPointF& p )
{
    const int size = 5;
    QGraphicsItem* rect = pointsDrawer->createPoint();
    
    rect->setPos(p.x(), p.y());
    rect->setZValue(1.0);
    rect->setParentItem(group.get());
    rect->setFlag(QGraphicsItem::ItemIsMovable);
    rect->setFlag(QGraphicsItem::ItemIsSelectable);

    addPoint(rect);
}

void dicom::PointsLayer::addPoint( QGraphicsItem* itm )
{
    points.push_back(itm);
    pathItem->setPath(pointsDrawer->createPath(points));
}

int dicom::PointsLayer::getNumPoint() const
{
    return points.size();
}

QGraphicsItem* dicom::PointsLayer::removePoint( int idx )
{
    QGraphicsItem* toRemove = points[idx];
    group->removeFromGroup(toRemove);
    points.erase(points.begin() + idx);
    return toRemove;

}

void dicom::PointsLayer::setName( const QString& name )
{
    this->name = name;
}


void dicom::PointsLayer::refresh()
{
    pathItem->setPath(pointsDrawer->createPath(points));
}

bool dicom::PointsLayer::hasPoint( QGraphicsItem* itm )
{
    return points.contains(itm);
}

QGraphicsItem* dicom::PointsLayer::getPoint( int idx ) const
{
    return points[idx];
}

dicom::IPointsDrawerPtr dicom::PointsLayer::getPointsDrawer() const
{
    return pointsDrawer;
}

void dicom::PointsLayer::setPointsDrawer( dicom::IPointsDrawerPtr val )
{
    pointsDrawer = val;

    pathItem->setPen(pointsDrawer->getLinePen(getEditable()));
    pathItem->setPath(pointsDrawer->createPath(points));

    // TODO: Podmiana punktow
}

bool dicom::PointsLayer::getEditable() const
{
    return !group->handlesChildEvents();
}

void dicom::PointsLayer::setEditable( bool val )
{
    group->setHandlesChildEvents(!val);
    group->setFlag(QGraphicsItem::ItemIsMovable, !val);
    group->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    pathItem->setPen(pointsDrawer->getLinePen(val));
}

bool dicom::PointsLayer::getSelected() const
{
    return group->isSelected();
}

void dicom::PointsLayer::setSelected( bool val )
{
    group->setSelected(val);
    auto childs = group->childItems();
    for (auto it2 = childs.begin(); it2 != childs.end(); ++it2) {
        (*it2)->setSelected(val);
    }
}



QPen dicom::PolyDrawer::getLinePen(bool editable)
{
    QPen pen(editable ? QColor(0, 100, 255) : QColor(128, 212, 220));
    pen.setWidth(2);
    return pen;
}

QGraphicsItem* dicom::PolyDrawer::createPoint()
{
    const int size = 5;
    auto rect = new QGraphicsRectItem(-size, -size, size * 2, size * 2);
    rect->setPen(getLinePen(true));
    return rect;
}

QString dicom::PolyDrawer::methodName()
{
    return "Polygon";
}

QPainterPath dicom::PolyDrawer::createPath(const QVector<QGraphicsItem*>& points)
{
    int count = points.size();
    if (count > 2) {
        QwtSplineCurveFitter fitter;
        QPolygonF poly;
        for (auto it = points.begin(); it != points.end(); ++it) {
            poly << (*it)->scenePos();
        }
        poly << (*points.begin())->scenePos();
        QPainterPath path;
        path.addPolygon(poly);
        return path;
    }

    return QPainterPath();
}


QPen dicom::CurveDrawer::getLinePen(bool editable)
{
    QPen pen(editable ? QColor(255, 0, 0) : QColor(220, 128, 128));
    pen.setWidth(2);
    return pen;
}


QGraphicsItem* dicom::CurveDrawer::createPoint()
{
    int size = 5;
    auto itm = new QGraphicsEllipseItem(-size, -size, 2 * size, 2 * size);
    itm->setPen(getLinePen(true));
    return itm;
}

QString dicom::CurveDrawer::methodName()
{
    return "Curve";
}

QPainterPath dicom::CurveDrawer::createPath(const QVector<QGraphicsItem*>& points)
{
    int count = points.size();
    if (count > 2) {
        QwtSplineCurveFitter fitter;
        QPolygonF poly;
        //poly << (*(points.end() - 1))->scenePos();
        for (auto it = points.begin(); it != points.end(); ++it) {
            poly << (*it)->scenePos();
        }
        poly << (*points.begin())->scenePos();
        //poly << (*(points.begin() + 1))->scenePos();

        fitter.setFitMode(QwtSplineCurveFitter::ParametricSpline);
        poly = fitter.fitCurve(poly);
        QPainterPath path;
        path.addPolygon(poly);
        return path;
    }

    return QPainterPath();
}
