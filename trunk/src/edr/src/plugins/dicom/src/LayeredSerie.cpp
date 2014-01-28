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
#include <QtGui/QScrollBar>

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
        img->addLayer(layer, serie->getUserName());
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
    // TODO: jak zastosowac update danych?
    auto cloneWrp = data->clone();
    image = cloneWrp->get();
    if (image) {
        //image = utils::const_pointer_cast<ILayeredImage>(img);
        graphicsScene->addItem(image->getBackgroundLayer()->getItem());

        BOOST_FOREACH(std::string tag, image->getTags()) {
            int count = image->getNumLayerItems(tag);
            for (int i = 0; i < count; ++i) {
                ILayerItemPtr vec = image->getLayerItem(tag, i);
                if (vec) {
                    graphicsScene->addItem(vec->getItem());
                }
            }
        }
        auto it = data->find("DICOM_XML");
        if (it != data->end()) {
            core::Filesystem::Path xml = it->second;
            setName(xml.stem().string());
        }
        this->data = data;
        switchCrop(); //domyslny cropping
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
    initialized(false),
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
    BOOST_FOREACH(std::string tag, image->getTags()) {
        int count = image->getNumLayerItems(tag);
        for (int i = 0; i < count; ++i) {
            ILayerItemPtr vec = image->getLayerItem(tag, i);
            if (vec && graphicsScene != vec->getItem()->scene()) {
                graphicsScene->addItem(vec->getItem());
            }
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


void dicom::LayeredSerie::setNormalState()
{
    stateMachine->setState(stateMachine->getNormalState());
}

void dicom::LayeredSerie::save()
{
    std::ofstream ofs(getXmlOutputFilename());
    assert(ofs.good());
    boost::archive::xml_oarchive oa(ofs);
    oa.register_type<BackgroundLayer>();
    oa.register_type<PointsLayer>();
    LayeredImageConstPtr l = utils::dynamic_pointer_cast<const LayeredImage>(getImage());
    oa << boost::serialization::make_nvp("layers", l->getLayersToSerialize(visualizer->getUserName()));
    ofs.close();
}

void dicom::LayeredSerie::removeLayer( int tagIdx, int idx )
{
    auto img = this->getImage();
    auto tag = img->getTag(tagIdx);
    if (idx >= 0 && idx < img->getNumLayerItems(tag)) {
        auto layer = img->getLayerItem(tag, idx);
        commandStack->addCommand(utils::make_shared<RemoveLayerCommand>(this, img, layer));    
    }
}

void dicom::LayeredSerie::editLayer( int tagIdx, int idx )
{
    auto img = this->getImage();

    auto tag = img->getTag(tagIdx);
    if (idx >= 0 && idx < img->getNumLayerItems(tag)) {
        auto layer = img->getLayerItem(tag, idx);
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
    //int count = img->getNumLayerItems(TODO);
    BackgroundLayerPtr background = utils::dynamic_pointer_cast<BackgroundLayer>(img->getBackgroundLayer());
    if (background) {
        if (background->getPixmap().rect() == background->getCrop()) {
            //background->setCrop(QRect(56,135, 761, 493));
            background->setCrop(QRect(4,133, 850, 645));
        } else {
            background->setCrop(background->getPixmap().rect());
        }
    }
}

void dicom::LayeredSerie::setMoveState()
{
    stateMachine->setState(stateMachine->getMoveState());
}

std::string dicom::LayeredSerie::getUserName() const
{
    return visualizer->getUserName();
}

double dicom::LayeredSerie::getBegin() const
{
    return 0.0;
}

double dicom::LayeredSerie::getEnd() const
{
    return 0.0;
}

void dicom::LayeredSerie::setBoneState()
{
    stateMachine->setState(stateMachine->getBoneState());
}

void dicom::LayeredSerie::setSkinState()
{
    stateMachine->setState(stateMachine->getSkinState());
}

void dicom::LayeredSerie::setTendonState()
{
    stateMachine->setState(stateMachine->getTendonState());
}

void dicom::LayeredSerie::setJointState()
{
    stateMachine->setState(stateMachine->getJointState());
}

void dicom::LayeredSerie::setInflamatoryState()
{
    stateMachine->setState(stateMachine->getInflamatoryState());
}

void dicom::LayeredSerie::setNoiseState()
{
    stateMachine->setState(stateMachine->getNoiseState());
}

void dicom::LayeredSerie::fitToSize()
{
    ILayeredImagePtr img = this->getImage();
    BackgroundLayerPtr background = utils::dynamic_pointer_cast<BackgroundLayer>(img->getBackgroundLayer());
    if (background) {
        auto rect = background->getCrop();
        auto rectW = getGraphicsView()->rect();
        
        auto h = (float)rectW.height() / rect.height();
        auto w = (float)rectW.width() / rect.width();

        if (w > 1.0f) {
            w *= 0.99f;
            getGraphicsView()->scale(w, w);
            auto sb = getGraphicsView()->verticalScrollBar();
            sb->setValue(sb->maximum() - 1);
        }
    }
}

void dicom::LayeredSerie::update()
{
}

void dicom::LayeredSerie::init()
{
    initialized = true;
    fitToSize();
}

bool dicom::LayeredSerie::wasInitialized() const
{
    return initialized;
}




