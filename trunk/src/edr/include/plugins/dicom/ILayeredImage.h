/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:28
	filename: 	ILayeredImage.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ILAYEREDIMAGE_H__
#define HEADER_GUARD_DICOM__ILAYEREDIMAGE_H__

#include <plugins/dicom/ILayer.h>

namespace dicom {

class ILayeredImage
{
public:
    typedef boost::iterator_range<std::list<ILayerConstPtr>::const_iterator> const_range;

public:
	virtual ~ILayeredImage() {}
    virtual void addLayer(ILayerConstPtr layer) = 0;
    virtual void removeLayer(ILayerConstPtr layer) = 0;
    virtual const_range getLayers() const = 0;
    virtual QPixmap getPixmap() const = 0;
};
DEFINE_SMART_POINTERS(ILayeredImage);
}


DEFINE_WRAPPER(dicom::ILayeredImage, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);

#endif
