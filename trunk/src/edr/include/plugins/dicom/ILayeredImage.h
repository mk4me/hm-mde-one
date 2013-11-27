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
#include <vector>
#include <utils/ObjectWrapper.h>
#include <boost/range/iterator_range_core.hpp>
#include <boost/serialization/access.hpp>
#include "boost/serialization/assume_abstract.hpp"

namespace dicom {

class ILayeredImage
{
public:
    typedef boost::iterator_range<std::vector<ILayerItemPtr>::const_iterator> const_range;

public:
    ILayeredImage() {}
	virtual ~ILayeredImage() {}
    virtual void addLayer(ILayerItemPtr layer) = 0;
    virtual void removeLayer(ILayerItemConstPtr layer) = 0;
    virtual const_range getLayers() const = 0;
    virtual QPixmap getPixmap() const = 0;
    virtual QSize getSize() const = 0;
    virtual int getNumLayers() const = 0;
    virtual ILayerItemConstPtr getLayer(int idx) const = 0;
    virtual ILayerItemPtr getLayer(int idx) = 0;

};
DEFINE_SMART_POINTERS(ILayeredImage);
}

DEFINE_WRAPPER(dicom::ILayeredImage, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);

#endif
