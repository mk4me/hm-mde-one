/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:27
	filename: 	ILayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ILAYER_H__
#define HEADER_GUARD_DICOM__ILAYER_H__

#include <utils/SmartPtr.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

class QPainter;
class QRect;
class QGraphicsItem;

namespace dicom {



class ILayerItem
{
public:
    ILayerItem() : adnotationIdx(-1) {}
	virtual ~ILayerItem() {}

    
    virtual QString getName() const = 0;
    virtual void setName(const QString& name) = 0;
    virtual QSize getSize() const = 0;

    virtual bool getSelected() const = 0;
    virtual void setSelected(bool val) = 0;


    // TODO : dobrze byloby przeniest to do jakiegos dekoratora...
    int getAdnotationIdx() const { return adnotationIdx; }
    void setAdnotationIdx(int val) { adnotationIdx = val; }

private:
    int adnotationIdx;
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    { 
        ar & BOOST_SERIALIZATION_NVP(adnotationIdx);
    }
};
DEFINE_SMART_POINTERS(ILayerItem);

//class IRasterLayerItem : public ILayerItem
//{
//public:
//    virtual ~IRasterLayerItem() {}
//
//    virtual void render(QPainter* painter, const QRect* rect) const = 0;
//};
//DEFINE_SMART_POINTERS(IRasterLayerItem);

class IVectorLayerItem : public ILayerItem
{
public:
    virtual ~IVectorLayerItem() {}
    virtual QGraphicsItem* getItem() = 0;
};
DEFINE_SMART_POINTERS(IVectorLayerItem);

class ILayer
{
public:
    virtual ~ILayer() {}

    virtual void addItem(ILayerItemPtr) = 0;
    virtual void removeItem(ILayerItemPtr) = 0;
    virtual void setVisible(bool) = 0;
};

}

#endif
