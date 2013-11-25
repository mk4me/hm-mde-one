/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:36
	filename: 	LayeredImage.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDIMAGE_H__
#define HEADER_GUARD_DICOM__LAYEREDIMAGE_H__

#include <plugins/dicom/ILayeredImage.h>

namespace dicom {

class LayeredImage : public ILayeredImage
{
public:
    LayeredImage() {}
    LayeredImage(const QPixmap& pixmap);
	virtual ~LayeredImage() {}

public:
    virtual void addLayer( ILayerConstPtr layer );
    virtual void removeLayer( ILayerConstPtr layer );
    virtual const_range getLayers() const;
    virtual QPixmap getPixmap() const;

private:
    std::list<ILayerConstPtr> layers;
};
DEFINE_SMART_POINTERS(LayeredImage);

}

DEFINE_WRAPPER_INHERITANCE(dicom::LayeredImage, dicom::ILayeredImage);
#endif
