#include "DicomPCH.h"
#include "LayeredImageVisualizer.h"
#include <plugins/dicom/ILayeredImage.h>
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include "LayeredImageVisualizerView.h"
#include <QtGui/QApplication>
#include "BackgroundLayer.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "LayeredImage.h"
#include "PointsLayer.h"
#include <corelib/PluginCommon.h>
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include "corelib/ISourceManager.h"
#include "plugins/newCommunication/IDownloadRequest.h"
#include "coreui/CorePopup.h"

using namespace dicom;

LayeredImageVisualizer::LayeredImageVisualizer()
{
    mainWidget = new LayeredImageVisualizerView(this);
    currentSerie = -1;
}

LayeredImageVisualizer::~LayeredImageVisualizer()
{

}

plugin::IVisualizer* LayeredImageVisualizer::create() const
{
    return new LayeredImageVisualizer();
}

plugin::IVisualizer::ISerie * LayeredImageVisualizer::createSerie( const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr& data )
{
    auto serie = new LayeredSerie(this);
    serie->setData(requestedType, data);

    series.push_back(serie);
    
    if (series.size() == 1) {
        setActiveSerie(series[0]);
    }

    Q_EMIT serieChanged();

    return serie; 
}

plugin::IVisualizer::ISerie * LayeredImageVisualizer::createSerie( const plugin::IVisualizer::ISerie * serie )
{
    return nullptr;
}

plugin::IVisualizer::ISerie* dicom::LayeredImageVisualizer::createSerie( const ISerie* serie, const core::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data )
{
    return nullptr;
}


void LayeredImageVisualizer::removeSerie( plugin::IVisualizer::ISerie *serie )
{
    auto active = getActiveSerie();
    auto it = std::find(series.begin(), series.end(), serie);
    if (it != series.end()) {
        series.erase(it);
    }

    if (active == serie) {
        currentSerie = series.empty() ? 0 : 1;
    }

    Q_EMIT serieChanged();
}

void LayeredImageVisualizer::setActiveSerie( plugin::IVisualizer::ISerie * serie )
{
    auto prevSerie = getActiveSerie();
    int count = static_cast<int>(series.size());
    for (int i = 0; i < count; ++i) {
        if (series[i] == serie) {
            currentSerie = i;
            if (prevSerie != series[i]) {
                Q_EMIT changeLabel(QString::fromStdString(series[i]->getName()));
                Q_EMIT serieChanged();
            }
            return;
        }
    }   
}

const plugin::IVisualizer::ISerie * LayeredImageVisualizer::getActiveSerie() const
{
    return correctIndex(currentSerie) ? series[currentSerie] : nullptr;
}

plugin::IVisualizer::ISerie * dicom::LayeredImageVisualizer::getActiveSerie()
{
    return correctIndex(currentSerie) ? series[currentSerie] : nullptr;
}


QWidget* LayeredImageVisualizer::createWidget()
{
    return mainWidget;
}

QIcon* LayeredImageVisualizer::createIcon()
{
    return new QIcon();
}

int LayeredImageVisualizer::getMaxDataSeries() const
{
    return -1;
}

void LayeredImageVisualizer::getSupportedTypes( core::TypeInfoList & supportedTypes ) const
{
    supportedTypes.push_back(typeid(ILayeredImage));
}

void LayeredImageVisualizer::update( double deltaTime )
{
    if (correctIndex(currentSerie)) {
        auto serie = series[currentSerie];
        // nie jest to najszczesliwszy mechanizm, ale inicjalizacja jest mozliwa dopiero, gdy zna sie geometrie widgetow
        if (!serie->wasInitialized()) {
            serie->init();
        }
    }
}

QPixmap LayeredImageVisualizer::takeScreenshot() const
{
    if (correctIndex(currentSerie)) {
        return series[currentSerie]->getPixmap();
    }
    return QPixmap();
}

void dicom::LayeredImageVisualizer::setPrevSerie()
{
    if (correctIndex(currentSerie - 1)) {
        setActiveSerie(series[currentSerie - 1]);
    } 
}

void dicom::LayeredImageVisualizer::setNextSerie()
{
    if (correctIndex(currentSerie + 1)) {
        setActiveSerie(series[currentSerie + 1]);
    } 
}
//
void dicom::LayeredImageVisualizer::trySetSerie( int val )
{
    if (correctIndex(val)) {
        setActiveSerie(series[val]);
    }
}

bool dicom::LayeredImageVisualizer::correctIndex( int i ) const
{
    return i >= 0 && i < series.size();
}

int dicom::LayeredImageVisualizer::getNumSeries() const
{
    return series.size();
}

int dicom::LayeredImageVisualizer::getCurrentSerieNo() const
{
    return currentSerie;
}

void dicom::LayeredImageVisualizer::saveSerie()
{
    if (correctIndex(currentSerie)) {
        series[currentSerie]->save();
    }
}

void dicom::LayeredImageVisualizer::editSerie(int tagIdx, int idx)
{
    if (correctIndex(currentSerie)) {
        series[currentSerie]->editLayer(tagIdx,idx);
    }
}

void dicom::LayeredImageVisualizer::removeLayer(int tagIdx, int idx)
{
    if (correctIndex(currentSerie)) {
        series[currentSerie]->removeLayer(tagIdx,idx);
    }
}

void dicom::LayeredImageVisualizer::selectLayer(int tagIdx, int idx )
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(getActiveSerie());
    if (serie) {
        auto selected = selectedLayer();
        ILayeredImagePtr img = serie->getImage();
        std::string tag = img->getTag(tagIdx);
        if (tag != selected.first || idx != selected.second) {
            serie->getGraphicsScene()->blockSignals(true);
            BOOST_FOREACH(std::string t, img->getTags()) {
                int count = img->getNumLayerItems(t);
                for (int i = 0; i < count; ++i) {
                    img->getLayerItem(t, i)->setSelected(tag == t && i == idx);
                }
            }
            serie->getGraphicsScene()->blockSignals(false);
            serie->getLayersModel()->refreshSelected();
        }
    }
}

std::pair<std::string, int> dicom::LayeredImageVisualizer::selectedLayer() const
{
    int idx = -1;
    std::string selectedTag;
    const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());
    if (serie) {
        ILayeredImageConstPtr img = serie->getImage();
        BOOST_FOREACH(std::string tag, img->getTags()) {
            int count = img->getNumLayerItems(tag);
            for (int i = 0; i < count; ++i) {
                if (img->getLayerItem(tag, i)->getSelected()) {
                    if (idx == -1) {
                        idx = i;
                        selectedTag = tag;
                    } else {
                        // zaznaczono wiecej niz jedna serie danych !?
                        UTILS_ASSERT(false);
                        return std::make_pair(std::string(), -1);
                    }
                }
            }
        }
    }

    return std::make_pair(selectedTag, idx);
}

void dicom::LayeredImageVisualizer::uploadSerie()
{
    saveSerie();
    if (correctIndex(currentSerie)) {
        communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
        core::Filesystem::Path p(series[currentSerie]->getXmlOutputFilename());
        /// TODO : pobrac dane z OW
        
        comm->uploadMotionFile(p, "");
    }
}

std::string dicom::LayeredImageVisualizer::getUserName() const
{
    communication::ICommunicationDataSourcePtr comm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
    return comm->currentUser()->name();
}

void dicom::LayeredImageVisualizer::removeSelectedLayers()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(getActiveSerie());
    if (serie) {
        auto image = serie->getImage();
        int tagsNo = image->getNumTags();
        for (int iTag = 0; iTag < tagsNo; ++iTag) {
            auto tag = image->getTag(iTag);
            for (int itm = image->getNumLayerItems(tag) - 1; itm >= 0; --itm) {
                if (image->getLayerItem(tag, itm)->getSelected()) {
                    serie->removeLayer(iTag, itm);
                }
            }
        }
    }
}



