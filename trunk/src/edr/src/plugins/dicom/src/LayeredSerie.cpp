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
#include <utils/ICommand.h>

using namespace dicom;

class RemoveLayerCommand : public utils::ICommand
{
public:
    RemoveLayerCommand(LayeredSerie* serie, ILayeredImagePtr img, ILayerItemPtr layer) :
      img(img),
      layer(layer),
      serie(serie)
    {
    }

public:
    virtual void doIt() 
    {
        img->removeLayer(layer);
        if (layer) {
            layer->getItem()->setVisible(false);//setParentItem(nullptr);
        }
        serie->refresh();
    }
    //! cofa wykonane ju¿ polecenie
    virtual void undoIt() 
    {
        img->addLayer(layer);
        if (layer) {
            layer->getItem()->setVisible(true);
        }
        serie->refresh();
    }

    //! \return nazwa polecenia (dla ewentualnej reprezentacji stosu poleceñ)
    virtual QString name() { return QString(typeid(this).name()); }

private:
    ILayeredImagePtr img;
    ILayerItemPtr layer;
    LayeredSerie* serie;
};

void LayeredSerie::setupData( const core::ObjectWrapperConstPtr & data )
{
    // TODO: const_cast, jak zastosowac update danych?
    ILayeredImageConstPtr img = data->get();
    if (img) {
        image = utils::const_pointer_cast<ILayeredImage>(img);
        graphicsScene->addItem(image->getBackgroundLayer()->getItem());
        int count = image->getNumLayers();
        for (int i = 0; i < count; ++i) {
            ILayerItemPtr vec = image->getLayer(i);
            if (vec) {
                graphicsScene->addItem(vec->getItem());
            }
        }
        core::Filesystem::Path xml = data->at("DICOM_XML");
        setName(xml.stem().string());
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
    //pixmapItem(nullptr),
    layersModel(visualizer),
    commandStack(new utils::CommandStack)
{
    stateMachine = utils::make_shared<LayeredStateMachine>(this, commandStack);
    graphicsScene = new coreUI::GraphicSceneWithStateMachine(stateMachine);
    graphicsView = new coreUI::WheelGraphicsView(graphicsScene);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
    graphicsScene->setSceneRect( graphicsView->rect() );

    QObject::connect(graphicsScene, SIGNAL(selectionChanged()), stateMachine.get(), SLOT(selectionChanged()));
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
    //pixmapItem->setPixmap(getPixmap());
    int count = image->getNumLayers();
    for (int i = 0; i < count; ++i) {
        ILayerItemPtr vec = image->getLayer(i);
        if (vec && graphicsScene != vec->getItem()->scene()) {
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
//    oa.register_type<CircleLayer>();
    oa.register_type<PointsLayer>();
    LayeredImageConstPtr l = utils::dynamic_pointer_cast<const LayeredImage>(getImage());
    oa << boost::serialization::make_nvp("layers", l->getLayersToSerialize());
    ofs.close();
}

void dicom::LayeredSerie::removeLayer( int idx )
{
    auto img = this->getImage();
    if (idx >= 0 && idx < img->getNumLayers()) {
        auto layer = img->getLayer(idx);
        commandStack->addCommand(utils::make_shared<RemoveLayerCommand>(this, img, layer));    
    }
}

void dicom::LayeredSerie::editLayer( int idx )
{
    auto img = this->getImage();
    if (idx >= 0 && idx < img->getNumLayers()) {
        auto layer = img->getLayer(idx);
        auto pointsLayer = utils::dynamic_pointer_cast<PointsLayer>(layer);
        if (pointsLayer) {
            stateMachine->getEditState()->setLayerToEdit(pointsLayer);
            stateMachine->setState(stateMachine->getEditState());
        }
    }
}

void dicom::LayeredSerie::switchCrop()
{
    ILayeredImagePtr img = this->getImage();
    int count = img->getNumLayers();
    BackgroundLayerPtr background = utils::dynamic_pointer_cast<BackgroundLayer>(img->getBackgroundLayer());
    if (background) {
        if (background->getPixmap().rect() == background->getCrop()) {
            background->setCrop(QRect(56,135, 761, 493));
        } else {
            background->setCrop(background->getPixmap().rect());
        }
    }
}

void dicom::LayeredSerie::setMoveState()
{
    stateMachine->setState(stateMachine->getMoveState());
}

