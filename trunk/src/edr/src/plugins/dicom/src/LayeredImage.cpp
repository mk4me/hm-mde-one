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


void dicom::LayeredImage::addLayer( ILayerItemPtr layer, const std::string& layerName )
{
    layers.insert(std::make_pair(layerName, layer));
    tags.insert(layerName);
    auto vis = tagsVisibility.find(layerName);
    if (vis == tagsVisibility.end()) {
        tagsVisibility[layerName] = true;
    }
}

void dicom::LayeredImage::removeLayer( ILayerItemConstPtr layer )
{
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        if (it->second == layer) {
            layers.erase(it);
            break;
        }
    }
}

dicom::LayeredImage::const_range dicom::LayeredImage::getLayerItems( const std::string& layerName ) const
{
    return boost::make_iterator_range(layers.equal_range(layerName));
    //return boost::make_iterator_range(layers.cbegin(), layers.cend());
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
        QSize layerSize = (it->second)->getSize();
        //QSize layerSize = (*it)->getSize();
        if (maxSize.width() < layerSize.width()) {
            maxSize.setWidth(layerSize.width());
        }

        if (maxSize.height() < layerSize.height()) {
            maxSize.setHeight(layerSize.height());
        }
    }

    return maxSize;
}

int dicom::LayeredImage::getNumLayerItems( const std::string& layerName ) const
{
    return layers.count(layerName);
}

dicom::ILayerItemConstPtr dicom::LayeredImage::getLayerItem(const std::string& layerName, int idx ) const
{
    UTILS_ASSERT(idx >= 0 && idx < getNumLayerItems(layerName));
    auto it = layers.equal_range(layerName).first;
    std::advance(it, idx);
    return (it->second);
}

dicom::ILayerItemPtr dicom::LayeredImage::getLayerItem(const std::string& layerName,  int idx )
{
    UTILS_ASSERT(idx >= 0 && idx < getNumLayerItems(layerName));
    auto it = layers.equal_range(layerName).first;
    std::advance(it, idx);
    return (it->second);
}

std::vector<dicom::ILayerItemConstPtr> dicom::LayeredImage::getLayersToSerialize( const std::string& tag ) const
{
    std::vector<ILayerItemConstPtr> ret;
    BOOST_FOREACH(auto layer, layers.equal_range(tag)) {
        ret.push_back(layer.second);
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

int dicom::LayeredImage::getNumTags() const
{
    return tags.size();
}

std::string dicom::LayeredImage::getTag( int idx ) const
{
    auto it = tags.begin();
    std::advance(it, idx);
    return *it;
}

dicom::LayeredImage::tags_range dicom::LayeredImage::getTags() const
{
    return boost::make_iterator_range(tags.cbegin(), tags.cend());
}

bool dicom::LayeredImage::getTagVisible( const std::string& tag ) const
{
    auto vis = tagsVisibility.find(tag);
    if (vis != tagsVisibility.end()) {
        return vis->second;
    } else {
        throw std::runtime_error("Unknown tag");
    }
}

void dicom::LayeredImage::setTagVisible( const std::string& tag, bool val )
{
    tagsVisibility[tag] = val;
    for (int i = getNumLayerItems(tag) - 1; i >= 0; --i ) {
        getLayerItem(tag, i)->getItem()->setVisible(val);
    }
}

dicom::ILayeredImage* dicom::LayeredImage::clone() const
{
    std::unique_ptr<LayeredImage> img(new LayeredImage());
    img->backgroundLayer = utils::dynamic_pointer_cast<BackgroundLayer>(ILayerItemPtr(backgroundLayer->clone()));
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        img->addLayer(ILayerItemPtr(it->second->clone()), it->first);
    }

    for (auto it = tagsVisibility.begin(); it != tagsVisibility.end(); ++it) {
        img->setTagVisible(it->first, it->second);
    }

    return img.release();
}


