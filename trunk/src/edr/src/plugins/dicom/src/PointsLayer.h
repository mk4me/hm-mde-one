
/********************************************************************
	created:	2013/11/20
	created:	20:11:2013   18:49
	filename: 	PointsLayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__POINTSLAYER_H__
#define HEADER_GUARD_DICOM__POINTSLAYER_H__

#include <plugins/dicom/ILayer.h>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsPathItem>
#include <qwt_spline.h>


#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include "qstring_serialization.h"
#include "Serializers.h"
#include "Adnotations.h"

namespace dicom {

    

    class IPointsDrawer
    {
    public:
        virtual ~IPointsDrawer() {}
        virtual QPen getLinePen(bool editable) = 0;
        virtual QBrush getLineBrush(bool editable) = 0;
        //virtual QPen getPointPen() = 0;
        virtual QString methodName() = 0;
        virtual QPainterPath createPath(const QVector<QGraphicsItem*>& points) = 0;
        virtual QGraphicsItem* createPoint() = 0;
        virtual IPointsDrawer* clone() const = 0;
    };
    DEFINE_SMART_POINTERS(IPointsDrawer);

    class DrawersBuilder 
    {
    public:
        static IPointsDrawerPtr createDrawer(adnotations::annotationsIdx annotationIdx);
        static std::pair<QColor, QColor> getColors(adnotations::annotationsIdx annotationIdx);
        static bool isOpenLine(adnotations::annotationsIdx annotationIdx);
        static bool isCurved(adnotations::annotationsIdx annotationIdx);
        static QPixmap getColorPixmap(adnotations::annotationsIdx annotationIdx);
    };

    class CurveDrawer : public IPointsDrawer 
    {
    public:
        CurveDrawer(bool openLine, const QColor& color, const QColor& colorEdit);
        QPen getLinePen(bool editable);
        QString methodName();
        QPainterPath createPath(const QVector<QGraphicsItem*>& points);
        QGraphicsItem* createPoint();

        virtual QBrush getLineBrush( bool editable );
        virtual IPointsDrawer* clone() const;
    private:
        QColor color;
        QColor colorEdit;
        bool openLine;
    };

    class PolyDrawer : public IPointsDrawer
    {
    public:
        PolyDrawer(bool openLine, const QColor& color, const QColor& colorEdit);
        QPen getLinePen(bool editable);
        QString methodName();
        QPainterPath createPath(const QVector<QGraphicsItem*>& points);
        QGraphicsItem* createPoint();

        virtual QBrush getLineBrush( bool editable );
        virtual IPointsDrawer* clone() const;

    private:
        QColor color;
        QColor colorEdit;
        bool openLine;
    };

    class PointsLayer : public ILayerItem
    {
    public:
        PointsLayer();
	    virtual ~PointsLayer();

        virtual QGraphicsItem* getItem();
        virtual QString getName() const;
        virtual void setName(const QString& name);
        virtual QSize getSize() const;
    public:
        void addPoint(const QPointF& p);
        void addPoint(const QPointF& p, int idx);
        void addPoint(QGraphicsItem* itm, int idx);
        void addPoint(QGraphicsItem* itm);
        int getNumPoint() const;
        QGraphicsItem* getPoint(int idx) const;
        QGraphicsItem* removePoint(int idx);
        void refresh();

        bool hasPoint(QGraphicsItem* itm);
        int getPointIdx(QGraphicsItem* itm);

        dicom::IPointsDrawerPtr getPointsDrawer() const;
        void setPointsDrawer(dicom::IPointsDrawerPtr val);
        bool getEditable() const;
        void setEditable(bool val);


        virtual bool getSelected() const;

        virtual void setSelected( bool val );

        virtual ILayerItem* clone() const;
        
    private:
        utils::scoped_ptr<QGraphicsItemGroup> group;
        utils::scoped_ptr<QGraphicsPathItem> pathItem; 
        QVector<QGraphicsItem*> points;
        QString name;
        IPointsDrawerPtr pointsDrawer;

    private:
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            QVector<QPointF> rawPoints;
            for (auto it = points.begin(); it != points.end(); ++it) {
                rawPoints.push_back((*it)->scenePos());
            }
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ILayerItem);
            ar & BOOST_SERIALIZATION_NVP(rawPoints);
            ar & BOOST_SERIALIZATION_NVP(name);
            QString method = pointsDrawer->methodName();
            ar & BOOST_SERIALIZATION_NVP(method);
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            QVector<QPointF> rawPoints;

            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ILayerItem);
            ar & BOOST_SERIALIZATION_NVP(rawPoints);
            ar & BOOST_SERIALIZATION_NVP(name);
            QString method;
            ar & BOOST_SERIALIZATION_NVP(method);
            // TODO: jesli dojdzie drawerow to koniecznie trzeba wprowadzic fabryke.
            /*if (method == "Curve") {
                pointsDrawer = utils::make_shared<CurveDrawer>();
            } else if (method == "Polygon") {
                pointsDrawer = utils::make_shared<PolyDrawer>();
            }*/
            pointsDrawer = DrawersBuilder::createDrawer(static_cast<adnotations::annotationsIdx>(getAdnotationIdx()));
            pathItem->setPen(pointsDrawer->getLinePen(false));

            for (auto it = rawPoints.begin(); it != rawPoints.end(); ++it) {
                addPoint(*it);
            }
            setEditable(false);
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER();


    };
    DEFINE_SMART_POINTERS(PointsLayer);      


}

#endif
