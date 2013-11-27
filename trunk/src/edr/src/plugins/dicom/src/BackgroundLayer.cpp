#include "DicomPCH.h"
#include "BackgroundLayer.h"
#include <QtGui/QPainter>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

//
//BOOST_CLASS_EXPORT(dicom::BackgroundLayer);
//BOOST_CLASS_EXPORT(dicom::CircleLayer);

void dicom::BackgroundLayer::render(QPainter* painter, const QRect* rect) const
{
    if (pixmap.isNull()) {
        pixmap = QPixmap(pixmapPath);
    }
    painter->drawPixmap(*rect, pixmap);
}

QString dicom::BackgroundLayer::getName() const
{
    return name;
}

dicom::BackgroundLayer::BackgroundLayer( const QPixmap& p, const QString& name /*= QString("Background")*/ ) :
    pixmap(p),
    name(name)
{
    setAdnotationIdx(0);
}

dicom::BackgroundLayer::BackgroundLayer( const QString& pixmapPath, const QString& name /*= QString("Background")*/ ) :
    pixmapPath(pixmapPath),
    name(name)
{

}

QSize dicom::BackgroundLayer::getSize() const
{
    return pixmap.size();
}

void dicom::BackgroundLayer::setName( const QString& name )
{
    this->name = name;
}


void dicom::CircleLayer::render(QPainter* painter, const QRect* rect) const
{
    painter->setPen(QColor(255, 80, 0, 255));
    painter->drawEllipse(point.x(), point.y(), r, r);
}

QString dicom::CircleLayer::getName() const
{
    return name;
}

QSize dicom::CircleLayer::getSize() const
{
    return QSize(point.x() + r, point.y() + r);
}

dicom::CircleLayer::CircleLayer( const QPoint& p, float r) :
    point(p),
    r(r),
    name("Circle")
{

}

void dicom::CircleLayer::setName( const QString& name )
{
    this->name = name;
}


//void dicom::PointsLayer::render( QPainter* painter, const QRect* rect ) const
//{
//    painter->setPen(QColor(150, 25, 0));
//    const int halfPointSize = 5;
//    if (!points.empty()) {
//        //auto last = points.begin();
//        //painter->drawRect(last->x() - halfPointSize, last->y() - halfPointSize, 2 * halfPointSize, 2 * halfPointSize);
//        for (auto it = points.begin(); it != points.end(); ++it) {
//            painter->drawRect(it->x() - halfPointSize, it->y() - halfPointSize, 2 * halfPointSize, 2 * halfPointSize);
//            //painter->drawLine(*last, *it);
//            //last = it;
//        }
//
//        painter->drawPolyline(points);
//        //painter->drawLine(*points.begin(), *points.rbegin());
//    }
//}
//
//QString dicom::PointsLayer::getName() const
//{
//    return name;
//}
//
//QSize dicom::PointsLayer::getSize() const
//{
//    QSize s(0, 0);
//    for (auto it = points.begin(); it != points.end(); ++it) {
//        int w = it->x();
//        int h = it->y();
//        if (w > s.width()) {
//            s.setWidth(w);
//        }
//
//        if (h > s.height()) {
//            s.setHeight(h);
//        }
//    }
//
//    return s;
//}
//
//void dicom::PointsLayer::addPoint( const QPoint& p )
//{
//    points.push_back(p);
//}
//
//int dicom::PointsLayer::getNumPoint() const
//{
//    return points.size();
//}
//
//void dicom::PointsLayer::removePoint( int idx )
//{
//    points.erase(points.begin() + idx);
//}
//
//void dicom::PointsLayer::setName( const QString& name )
//{
//    this->name = name;
//}
//
//dicom::PointsLayer::PointsLayer() : 
//    name("Points")
//{
//
//}
