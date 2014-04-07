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
	class LayeredImageVisualizerView;

    // TODO : wywalic to time avare...
    class LayeredSerie : public plugin::AbstractSerie
    {
    public:
        LayeredSerie(LayeredImageVisualizer* visualizer, LayeredImageVisualizerView * view);
        virtual ~LayeredSerie() {}

		virtual void update() {}

    public:
        virtual void setupData( const core::ObjectWrapperConstPtr & data );  

    public:
		const bool editionEnable() const;
        QPixmap getPixmap() const;
        QSize getSize() const;
        std::string getXmlOutputFilename() const;
        std::string getLoggedUserName() const;
        ILayeredImagePtr getImage();
        ILayeredImageConstPtr getImage() const;
        coreUI::WheelGraphicsView* getGraphicsView() const;
        coreUI::GraphicSceneWithStateMachine* getGraphicsScene() const;
        dicom::LayeredModelView* getLayersModel();
        void refresh();
        void save();
        void removeLayer(int tagIdx, int idx);
        void editLayer(int tagIdx, int idx);
        void undo();
        void redo();
        //void setPolyState();
        //void setCurveState();
        void setNormalState();
        void switchCrop();
        void setMoveState();

        void setBoneState();
        void setSkinState();
        void setTendonState();
        void setJointState();
        void setInflamatoryState();
        void setNoiseState();
        void fitToSize();

        void init();
        bool wasInitialized() const;

		void markAsEdited(const bool edited);

		const bool isEdited()const;

    private:
        bool initialized;
        ILayeredImagePtr image;
        coreUI::WheelGraphicsView* graphicsView;
        coreUI::GraphicSceneWithStateMachine* graphicsScene;
        LayeredStateMachinePtr stateMachine;
        LayeredImageVisualizer* visualizer;
		LayeredImageVisualizerView * view;
        LayeredModelView layersModel;
        utils::ObjectWrapperConstPtr data;
        utils::CommandStackPtr commandStack;
    };
    DEFINE_SMART_POINTERS(LayeredSerie)

}
#endif
