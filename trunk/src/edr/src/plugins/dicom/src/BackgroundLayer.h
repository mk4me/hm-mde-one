/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:32
	filename: 	BackgroundLayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__BACKGROUNDLAYER_H__
#define HEADER_GUARD_DICOM__BACKGROUNDLAYER_H__

#include <plugins/dicom/ILayer.h>

#include <boost/serialization/nvp.hpp>
#include "qstring_serialization.h"
#include "Serializers.h"

namespace dicom {

class BackgroundLayer : public IRasterLayerItem
{
public:
    BackgroundLayer(const QPixmap& p, const QString& name = QString("Background"));
    BackgroundLayer(const QString& pixmapPath, const QString& name = QString("Background"));
	virtual ~BackgroundLayer() {}

public:
    virtual void render(QPainter* painter, const QRect* rect) const;
    virtual QString getName() const;

    virtual QSize getSize() const;

private:
    BackgroundLayer() {}
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        //ar & boost::serialization::base_object<ILayer>(*this);
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IRasterLayerItem);
        ar & BOOST_SERIALIZATION_NVP(name);
        ar & BOOST_SERIALIZATION_NVP(pixmapPath);
        //ar & BOOST_SERIALIZATION_NVP(pixmap);
    }

    virtual void setName( const QString& name );

private:
    mutable QPixmap pixmap;
    QString pixmapPath;
    QString name;
};
DEFINE_SMART_POINTERS(BackgroundLayer);

class CircleLayer : public IRasterLayerItem
{
public:
    CircleLayer(const QPoint& p, float r = 50);
    virtual ~CircleLayer() {}

public:
    virtual void render(QPainter* painter, const QRect* rect) const;
    virtual QString getName() const;
    virtual QSize getSize() const;

private:
    QPoint point;
    float r;
    QString name;

private:
    CircleLayer() {}
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        //ar & boost::serialization::base_object<ILayer>(*this);
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IRasterLayerItem);
        ar & BOOST_SERIALIZATION_NVP(point);
        ar & BOOST_SERIALIZATION_NVP(r);
    }

    virtual void setName( const QString& name );

};

//class PointsLayer : public IRasterLayerItem
//{
//public:
//    PointsLayer();
//    virtual ~PointsLayer() {}
//    virtual void render( QPainter* painter, const QRect* rect ) const;
//    virtual QString getName() const;
//    virtual QSize getSize() const;
//    virtual void setName( const QString& name );
//
//public:
//    void addPoint(const QPoint& p);
//    int getNumPoint() const;
//    void removePoint(int idx);
//
//
//private:
//    QVector<QPoint> points;
//    QString name;
//
//private:
//    friend class boost::serialization::access;
//    template <typename Archive>
//    void serialize(Archive& ar, const unsigned int version)
//    {
//        //ar & boost::serialization::base_object<ILayer>(*this);
//        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IRasterLayerItem);
//        ar & BOOST_SERIALIZATION_NVP(points);
//        ar & BOOST_SERIALIZATION_NVP(name);
//    }
//};
//DEFINE_SMART_POINTERS(PointsLayer);

}

#endif
