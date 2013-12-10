/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:36
	filename: 	LayeredImage.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDIMAGE_H__
#define HEADER_GUARD_DICOM__LAYEREDIMAGE_H__


#include <plugins/dicom/ILayer.h>
#include <plugins/dicom/ILayeredImage.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include "BackgroundLayer.h"


namespace dicom {

class LayeredImage : public ILayeredImage
{
public:
    LayeredImage();
    LayeredImage(const QPixmap& pixmap);
    LayeredImage(const std::string& pixmap);
	virtual ~LayeredImage();

public:
    virtual void addLayer( ILayerItemPtr layer );
    virtual void removeLayer( ILayerItemConstPtr layer );
    virtual const_range getLayers() const;
    virtual QPixmap getPixmap() const;
    virtual int getNumLayers() const;
    virtual ILayerItemConstPtr getLayer(int idx) const;
    virtual ILayerItemPtr getLayer(int idx);
    QSize getSize() const;

    // workaround
    std::vector<ILayerItemConstPtr> getLayersToSerialize() const;

    dicom::IVectorLayerItemPtr getBackgroundLayer() const;
    void setBackgroundLayer(dicom::BackgroundLayerPtr val);

private:
    std::vector<ILayerItemPtr> layers;
    BackgroundLayerPtr backgroundLayer;

private:
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("layers", layers);
    }
};
DEFINE_SMART_POINTERS(LayeredImage);

}

DEFINE_WRAPPER_INHERITANCE(dicom::LayeredImage, dicom::ILayeredImage);
#endif
