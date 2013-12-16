#include "DicomPCH.h"
#include "LayeredImage.h"
#include "BackgroundLayer.h"
#include <QtGui/QPainter>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <QtGui/QGraphicsItemGroup>


dicom::LayeredImage::LayeredImage( const QPixmap& pixmap ) 
{
    //layers.push_back(utils::make_shared<BackgroundLayer>(pixmap));
    backgroundLayer = utils::make_shared<BackgroundLayer>(pixmap);
}

dicom::LayeredImage::LayeredImage( const std::string& pixmap )
{
    //layers.push_back(utils::make_shared<BackgroundLayer>(QString::fromStdString(pixmap)));
    backgroundLayer = utils::make_shared<BackgroundLayer>(QString::fromStdString(pixmap));
}

dicom::LayeredImage::LayeredImage()
{

}


dicom::LayeredImage::~LayeredImage()
{

}


void dicom::LayeredImage::addLayer( ILayerItemPtr layer )
{
    layers.push_back(layer);
}

void dicom::LayeredImage::removeLayer( ILayerItemConstPtr layer )
{
    auto it = std::find(layers.begin(), layers.end(), layer);
    if (it != layers.end()) {
        layers.erase(it);
    }
}

dicom::LayeredImage::const_range dicom::LayeredImage::getLayers() const
{
    return boost::make_iterator_range(layers.cbegin(), layers.cend());
}


QPixmap dicom::LayeredImage::getPixmap() const
{
    throw std::runtime_error("Not implemented");
    /*QSize size = getSize();
    QRect rect(QPoint(0, 0), size);
    QPixmap pix(size);
    QPainter paint(&pix);
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        IRasterLayerItemPtr raster = utils::dynamic_pointer_cast<IRasterLayerItem>(*it);
        if (raster) {
            raster->render(&paint, &rect);
        }
    }
    return pix;*/
}

QSize dicom::LayeredImage::getSize() const
{
    QSize maxSize = backgroundLayer->getSize();
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        QSize layerSize = (*it)->getSize();

        if (maxSize.width() < layerSize.width()) {
            maxSize.setWidth(layerSize.width());
        }

        if (maxSize.height() < layerSize.height()) {
            maxSize.setHeight(layerSize.height());
        }
    }

    return maxSize;
}

int dicom::LayeredImage::getNumLayers() const
{
    return layers.size();
}

dicom::ILayerItemConstPtr dicom::LayeredImage::getLayer( int idx ) const
{
    UTILS_ASSERT(idx >= 0 && idx < getNumLayers());
    return layers[idx];
}

dicom::ILayerItemPtr dicom::LayeredImage::getLayer( int idx )
{
    UTILS_ASSERT(idx >= 0 && idx < getNumLayers());
    return layers[idx];
}

std::vector<dicom::ILayerItemConstPtr> dicom::LayeredImage::getLayersToSerialize() const
{
    std::vector<ILayerItemConstPtr> ret;
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        //BackgroundLayerPtr b = utils::dynamic_pointer_cast<BackgroundLayer>(*it);
        //if (!b) {
            ret.push_back(*it);
        //}
    }

    return ret;
}

dicom::ILayerItemPtr dicom::LayeredImage::getBackgroundLayer() const
{
    return backgroundLayer;
}

void dicom::LayeredImage::setBackgroundLayer( dicom::BackgroundLayerPtr val )
{
    backgroundLayer = val;
}


