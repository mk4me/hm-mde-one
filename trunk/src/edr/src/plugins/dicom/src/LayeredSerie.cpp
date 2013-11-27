#include "DicomPCH.h"
#include "LayeredImageVisualizer.h"
#include <plugins/dicom/ILayeredImage.h>
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QGraphicsPixmapItem>
#include "LayeredImageVisualizerView.h"
#include <QtGui/QApplication>
#include "BackgroundLayer.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "LayeredImage.h"

using namespace dicom;


void LayeredSerie::setupData( const core::ObjectWrapperConstPtr & data )
{
    // TODO: const_cast, jak zastosowac update danych?
    ILayeredImageConstPtr img = data->get();
    if (img) {
        image = utils::const_pointer_cast<ILayeredImage>(img);
        int count = image->getNumLayers();
        for (int i = 0; i < count; ++i) {
            IVectorLayerItemPtr vec = utils::dynamic_pointer_cast<IVectorLayerItem>(image->getLayer(i));
            if (vec) {
                graphicsScene->addItem(vec->getItem());
            }
        }
        pixmapItem = graphicsScene->addPixmap(getPixmap());
        pixmapItem->setZValue(-100);
        this->data = data;
    }
}

void LayeredSerie::setTime( double time )
{

}

double LayeredSerie::getLength() const
{
    return 1.0;
}

//void dicom::LayeredSerie::onClick( const QPoint& p )
//{
//    image->addLayer(utils::make_shared<CircleLayer>(p));
//}

QPixmap dicom::LayeredSerie::getPixmap() const
{
    return image->getPixmap();
}

QSize dicom::LayeredSerie::getSize() const
{
    return image->getSize();
}

dicom::ILayeredImageConstPtr dicom::LayeredSerie::getImage() const
{
    return image;
}

dicom::ILayeredImagePtr dicom::LayeredSerie::getImage()
{
    return image;
}

dicom::LayeredSerie::LayeredSerie(LayeredImageVisualizer* visualizer) :
    visualizer(visualizer),
    pixmapItem(nullptr),
    layersModel(visualizer),
    commandStack(new utils::CommandStack)
{
    stateMachine = utils::make_shared<LayeredStateMachine>(this, commandStack);
    graphicsScene = new coreUI::GraphicSceneWithStateMachine(stateMachine);
    graphicsView = new coreUI::WheelGraphicsView(graphicsScene);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
    graphicsScene->setSceneRect( graphicsView->rect() );

}

coreUI::WheelGraphicsView* dicom::LayeredSerie::getGraphicsView() const
{
    return graphicsView;
}

coreUI::GraphicSceneWithStateMachine* dicom::LayeredSerie::getGraphicsScene() const
{
    return graphicsScene;
}

void dicom::LayeredSerie::refresh()
{
    getGraphicsView()->setSceneRect(0, 0, getSize().width(), getSize().height());
    getGraphicsScene()->setSceneRect( getGraphicsView()->rect() );
    pixmapItem->setPixmap(getPixmap());
    int count = image->getNumLayers();
    for (int i = 0; i < count; ++i) {
        IVectorLayerItemPtr vec = utils::dynamic_pointer_cast<IVectorLayerItem>(image->getLayer(i));
        if (vec) {
            graphicsScene->addItem(vec->getItem());
        }
    }
    layersModel.setImage(getImage());
}

std::string dicom::LayeredSerie::getXmlOutputFilename() const
{
    return data->at("DICOM_XML");
}

dicom::LayeredModelView* dicom::LayeredSerie::getLayersModel()
{
    return &layersModel;
}

void dicom::LayeredSerie::undo()
{
    if (commandStack->isUndoPossible()) {
        commandStack->undo();
    }
}

void dicom::LayeredSerie::redo()
{
    if (commandStack->isRedoPossible()) {
        commandStack->redo();
    }
}

void dicom::LayeredSerie::setPolyState()
{
    stateMachine->setState(stateMachine->getPolyState());
}

void dicom::LayeredSerie::setCurveState()
{
    stateMachine->setState(stateMachine->getCurveState());
}

void dicom::LayeredSerie::setNormalState()
{
    stateMachine->setState(stateMachine->getNormalState());
}

void dicom::LayeredSerie::save()
{
    std::ofstream ofs(getXmlOutputFilename());
    assert(ofs.good());
    boost::archive::xml_oarchive oa(ofs);
    /*oa.register_type<LayeredImage>();*/
    oa.register_type<BackgroundLayer>();
    oa.register_type<CircleLayer>();
    oa.register_type<PointsLayer>();
    LayeredImageConstPtr l = utils::dynamic_pointer_cast<const LayeredImage>(getImage());
    oa << boost::serialization::make_nvp("layers", l->getLayersToSerialize());
    ofs.close();
}

