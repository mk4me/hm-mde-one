/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   20:14
	filename: 	LayeredSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDSERIE_H__
#define HEADER_GUARD_DICOM__LAYEREDSERIE_H__

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <corelib/IVisualizer.h>
#include <corelib/AbstractSerie.h>
#include <plugins/dicom/ILayeredImage.h>
#include <coreui/WheelGraphicsView.h>
#include <coreui/GraphicSceneWithStateMachine.h>
#include "LayeredStateMachine.h"
#include "LayeredModelView.h"
#include <utils/CommandStack.h>

class QLabel;
class QScrollArea;
class QPushButton;
class QSlider;
class QGraphicsPixmapItem;

namespace dicom {

    class LayeredImageVisualizer;

    class LayeredSerie : public plugin::AbstractSerie, public plugin::IVisualizer::ITimeAvareSerieFeatures
    {
    public:
        LayeredSerie(LayeredImageVisualizer* visualizer);
        virtual ~LayeredSerie() {}

    public:
        virtual void setupData( const core::ObjectWrapperConstPtr & data );
        virtual void update() {}
        virtual void setTime( double time );
        virtual double getLength() const;
        

    public:
        QPixmap getPixmap() const;
        QSize getSize() const;
        //void onClick(const QPoint& p);
        std::string getXmlOutputFilename() const;

        ILayeredImagePtr getImage();
        ILayeredImageConstPtr getImage() const;
        coreUI::WheelGraphicsView* getGraphicsView() const;
        coreUI::GraphicSceneWithStateMachine* getGraphicsScene() const;
        dicom::LayeredModelView* getLayersModel();
        void refresh();
        void save();

        void removeLayer(int idx);
        void editLayer(int idx);
        void undo();
        void redo();
        void setPolyState();
        void setCurveState();
        void setNormalState();
    private:
        ILayeredImagePtr image;
        coreUI::WheelGraphicsView* graphicsView;
        coreUI::GraphicSceneWithStateMachine* graphicsScene;
        LayeredStateMachinePtr stateMachine;
        LayeredImageVisualizer* visualizer;
        QGraphicsPixmapItem* pixmapItem;
        LayeredModelView layersModel;
        utils::ObjectWrapperConstPtr data;
        utils::CommandStackPtr commandStack;
    };
    DEFINE_SMART_POINTERS(LayeredSerie)

}
#endif
