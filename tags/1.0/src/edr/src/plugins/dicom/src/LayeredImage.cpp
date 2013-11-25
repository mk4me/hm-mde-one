#include "DicomPCH.h"
#include "LayeredImage.h"
#include "BackgroundLayer.h"

void dicom::LayeredImage::addLayer( ILayerConstPtr layer )
{
    layers.push_back(layer);
}

void dicom::LayeredImage::removeLayer( ILayerConstPtr layer )
{
    layers.remove(layer);
}

dicom::LayeredImage::const_range dicom::LayeredImage::getLayers() const
{
    return boost::make_iterator_range(layers.cbegin(), layers.cend());
}

dicom::LayeredImage::LayeredImage( const QPixmap& pixmap )
{
    layers.push_back(utils::make_shared<BackgroundLayer>(pixmap));
}

QPixmap dicom::LayeredImage::getPixmap() const
{
    // TODO : Wprowadzic faktyczna obsluge warstw
    UTILS_ASSERT(layers.size() == 1);
    return (*layers.begin())->render();
}
