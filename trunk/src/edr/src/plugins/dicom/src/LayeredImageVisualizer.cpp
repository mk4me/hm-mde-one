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
        //std::ofstream ofs(series[currentSerie]->getXmlOutputFilename());
        //assert(ofs.good());
        //boost::archive::xml_oarchive oa(ofs);
        ///*oa.register_type<LayeredImage>();*/
        //oa.register_type<BackgroundLayer>();
        //oa.register_type<CircleLayer>();
        //oa.register_type<PointsLayer>();
        //LayeredImageConstPtr l = utils::dynamic_pointer_cast<const LayeredImage>(series[currentSerie]->getImage());
        //oa << boost::serialization::make_nvp("layers", l->getLayersToSerialize());
        //ofs.close();
    }
}


