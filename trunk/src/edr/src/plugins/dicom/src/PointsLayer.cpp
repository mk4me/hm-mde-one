#include "DicomPCH.h"
#include "PointsLayer.h"
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPen>
#include "spline.h"
#include "boost/serialization/export.hpp"


dicom::PointsLayer::PointsLayer() :
    group (new QGraphicsItemGroup()),
    pathItem(new QGraphicsPathItem()),
    name("VPoints")
{
    pathItem->setParentItem(group.get());
    group->setHandlesChildEvents(false);
    //setEditable(false);
}

dicom::PointsLayer::~PointsLayer()
{
}

QGraphicsItem* dicom::PointsLayer::getItem()
{
    if (pathItem->path().isEmpty()) {
        refresh();
    }
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

void dicom::PointsLayer::addPoint( const QPointF& p, int idx )
{
    QGraphicsItem* rect = pointsDrawer->createPoint();
    
    rect->setPos(p.x(), p.y());
    rect->setZValue(1.0);
    rect->setParentItem(group.get());
    rect->setFlag(QGraphicsItem::ItemIsMovable);
    rect->setFlag(QGraphicsItem::ItemIsSelectable, false);

    addPoint(rect, idx);
    setSelected(false);
}

void dicom::PointsLayer::addPoint( const QPointF& p)
{
    addPoint(p, points.size());
}

void dicom::PointsLayer::addPoint( QGraphicsItem* itm )
{
    addPoint(itm, points.size());
}

void dicom::PointsLayer::addPoint( QGraphicsItem* itm, int idx )
{
    points.insert(idx, itm);
    setSelected(false);
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
    pathItem->setBrush(pointsDrawer->getLineBrush(getEditable()));
    pathItem->setPath(pointsDrawer->createPath(points));

    // TODO: Podmiana punktow
}

bool dicom::PointsLayer::getEditable() const
{
    return !group->handlesChildEvents();
}

void dicom::PointsLayer::setEditable( bool val )
{
    /*if (val) {
        setSelected(false);
        for (auto it = points.begin(); it != points.end(); ++it) {
            (*it)->setSelected(false);
            (*it)->setFlag(QGraphicsItem::ItemIsSelectable, !val);
        }

        pathItem->setSelected(false);

        pathItem->setFlag(QGraphicsItem::ItemIsMovable, !val);
        pathItem->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    }
    group->setHandlesChildEvents(!val);
    group->setFlag(QGraphicsItem::ItemIsMovable, !val);
    group->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    pathItem->setPen(pointsDrawer->getLinePen(val));*/

    setSelected(false);
    for (auto it = points.begin(); it != points.end(); ++it) {
        (*it)->setSelected(false);
        (*it)->setFlag(QGraphicsItem::ItemIsSelectable, true);
        (*it)->setFlag(QGraphicsItem::ItemIsMovable, val);
    }
    group->setHandlesChildEvents(!val);
    group->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    group->setFlag(QGraphicsItem::ItemIsMovable, !val);
    pathItem->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    pathItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    if (pointsDrawer) {
        pathItem->setPen(pointsDrawer->getLinePen(val));
        pathItem->setBrush(pointsDrawer->getLineBrush(val));
    }
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

int dicom::PointsLayer::getPointIdx( QGraphicsItem* itm )
{
    return points.indexOf(itm);
}

dicom::ILayerItem* dicom::PointsLayer::clone() const
{
    std::unique_ptr<PointsLayer> pl(new PointsLayer());
    pl->setAdnotationIdx(getAdnotationIdx());
    pl->setName(this->name);
    pl->setPointsDrawer(IPointsDrawerPtr(pointsDrawer->clone()));

    for (auto it = points.begin(); it != points.end(); ++it) {
        pl->addPoint((*it)->pos());
    }
    pl->setEditable(getEditable());

    return pl.release();
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
    if (count >= 2) {
        QPolygonF poly;
        for (auto it = points.begin(); it != points.end(); ++it) {
            poly << (*it)->pos();
        }
        if (!openLine) {
            poly << (*points.begin())->pos();
        }
        QPainterPath path;
        path.addPolygon(poly);
        return path;
    }

    return QPainterPath();
}

QBrush dicom::PolyDrawer::getLineBrush( bool editable )
{

    return openLine ? QBrush() : QBrush(editable ? 
        QColor(colorEdit.red(), colorEdit.green(), colorEdit.blue(), 30) : 
        QColor(color.red(), color.green(), color.blue(), 30));
}

QPen dicom::PolyDrawer::getLinePen(bool editable)
{
    QPen pen(editable ? colorEdit : color);
    pen.setWidth(2);
    return pen;
}

dicom::IPointsDrawer* dicom::PolyDrawer::clone() const
{
    //return QBrush(editable ? QColor(0, 100, 255, 30) : QColor(128, 212, 220, 30));
    return new PolyDrawer(openLine, color, colorEdit);
}

dicom::PolyDrawer::PolyDrawer( bool openLine, const QColor& color, const QColor& colorEdit) :
    color(color),
    colorEdit(colorEdit),
    openLine(openLine)
{

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
        QPolygonF poly;
        auto beginPos = (*points.begin())->pos();
        auto endPos = (*(points.end() - 1))->pos();
        for (auto it = points.begin(); it != points.end(); ++it) {
            poly << (*it)->pos();
        }
        if (openLine == false && poly.isClosed() == false) {
            poly << beginPos;
        }

        SplineCurveFitter fitter;
        poly = fitter.fitCurve(poly);
        QPainterPath path;
        path.addPolygon(poly);
        return path;
    }

    return QPainterPath();
}

QBrush dicom::CurveDrawer::getLineBrush( bool editable )
{
    return openLine ? QBrush() : QBrush(editable ? 
        QColor(colorEdit.red(), colorEdit.green(), colorEdit.blue(), 30) : 
        QColor(color.red(), color.green(), color.blue(), 30));
}

QPen dicom::CurveDrawer::getLinePen(bool editable)
{
    //QPen pen(editable ? QColor(255, 0, 0) : QColor(220, 128, 128));
    QPen pen(editable ? colorEdit : color);
    pen.setWidth(2);
    return pen;
}


dicom::IPointsDrawer* dicom::CurveDrawer::clone() const
{
    return new CurveDrawer(openLine, color, colorEdit);
}

dicom::CurveDrawer::CurveDrawer(bool openLine, const QColor& color, const QColor& colorEdit) :
    color(color),
    colorEdit(colorEdit),
    openLine(openLine)
{

}

std::pair<QColor, QColor> dicom::DrawersBuilder::getColors( adnotations::annotationsIdx annotationIdx )
{
    switch(annotationIdx) {
        case adnotations::unknownAnnotation:
            return std::make_pair(QColor(50, 50, 50), QColor(0, 0, 0));
        case adnotations::otherAnnotation:                           
            return std::make_pair(QColor(150, 75, 50), QColor(100, 50, 0));
        case adnotations::skin:                            
            return std::make_pair(QColor(220, 175, 128), QColor(255, 100, 0));
        case adnotations::bone:                            
            return std::make_pair(QColor(128, 220, 128), QColor(0, 255, 0));
        case adnotations::tendon:                          
            return std::make_pair(QColor(200, 200, 128), QColor(255, 255, 0));
        case adnotations::joint: 
            return std::make_pair(QColor(128, 212, 220), QColor(0, 100, 255));
        case adnotations::inflammatory:
            return std::make_pair(QColor(220, 128, 128), QColor(255, 0, 0));
        case adnotations::intensity:
            return std::make_pair(QColor(50, 75, 150), QColor(0, 50, 100));
        case adnotations::noise:                           
            return std::make_pair(QColor(200, 200, 200), QColor(150, 150, 150));
        default:
            throw std::runtime_error("Unknown annotation");
    }
}

dicom::IPointsDrawerPtr dicom::DrawersBuilder::createDrawer( adnotations::annotationsIdx annotationIdx )
{
    auto color = getColors(annotationIdx);
    auto openLine = isOpenLine(annotationIdx);
    if (isCurved(annotationIdx)) {
        return utils::make_shared<CurveDrawer>(openLine, color.first, color.second);
    } else {
        return utils::make_shared<PolyDrawer>(openLine, color.first, color.second);
    }
}

bool dicom::DrawersBuilder::isOpenLine( adnotations::annotationsIdx annotationIdx )
{
    switch(annotationIdx) {
    case adnotations::unknownAnnotation:
        return false;
    case adnotations::otherAnnotation:                           
        return false;
    case adnotations::skin:                            
        return true;
    case adnotations::bone:                            
        return true;
    case adnotations::tendon:                          
        return true;
    case adnotations::joint: 
        return false;
    case adnotations::inflammatory:
        return false;
    case adnotations::intensity:
        return false;
    case adnotations::noise:                           
        return false;
    default:
        throw std::runtime_error("Unknown annotation");
    }
}

bool dicom::DrawersBuilder::isCurved( adnotations::annotationsIdx annotationIdx )
{
    switch(annotationIdx) {
    case adnotations::unknownAnnotation:
        return false;
    case adnotations::otherAnnotation:                           
        return false;
    case adnotations::skin:                            
        return true;
    case adnotations::bone:                            
        return false;
    case adnotations::tendon:                          
        return false;
    case adnotations::joint: 
        return false;
    case adnotations::inflammatory:
        return true;
    case adnotations::intensity:
        return false;
    case adnotations::noise:                           
        return false;
    default:
        throw std::runtime_error("Unknown annotation");
    }
}

QPixmap dicom::DrawersBuilder::getColorPixmap( adnotations::annotationsIdx annotationIdx )
{
    static std::map<adnotations::annotationsIdx, QPixmap> pixmaps;
    if (pixmaps.empty()) {
        auto makePix = [&](adnotations::annotationsIdx idx) -> QPixmap {
            QPixmap pix(8, 8);
            pix.fill(getColors(idx).second);
            return pix;
        };
        pixmaps[adnotations::unknownAnnotation     ] = makePix(adnotations::unknownAnnotation     );
        pixmaps[adnotations::otherAnnotation       ] = makePix(adnotations::otherAnnotation       );
        pixmaps[adnotations::skin        ] = makePix(adnotations::skin        );
        pixmaps[adnotations::bone        ] = makePix(adnotations::bone        );
        pixmaps[adnotations::tendon      ] = makePix(adnotations::tendon      );
        pixmaps[adnotations::joint       ] = makePix(adnotations::joint       );
        pixmaps[adnotations::inflammatory] = makePix(adnotations::inflammatory);
        pixmaps[adnotations::intensity   ] = makePix(adnotations::intensity   );
        pixmaps[adnotations::noise       ] = makePix(adnotations::noise       );
    }
    return pixmaps[annotationIdx];
}
