#include "DicomPCH.h"
#include "LayeredImageVisualizerView.h"
#include "ui_layeredImageVisualizer.h"
#include "LayeredImageVisualizer.h"
#include <QtGui/QGraphicsScene>
#include <coreui/WheelGraphicsView.h>
#include <QtGui/QGraphicsItem>
#include <QtGui/QMouseEvent>
#include <coreui/CoreAction.h>
#include "AdnotationsDelegate.h"

using namespace dicom;

//class LayeredGraphicsView : public coreUI::WheelGraphicsView
//{
//public:
//    LayeredGraphicsView(LayeredImageVisualizerView* view, LayeredImageVisualizer* model, QGraphicsScene* scene, Qt::KeyboardModifier modifier = Qt::NoModifier) : 
//      coreUI::WheelGraphicsView(scene, modifier), model(model), view(view) {}
//
//    //virtual void mousePressEvent( QMouseEvent * e)
//    //{
//    //    auto serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
//    //    if (serie) {
//    //        QSize size = serie->getSize();
//    //        QPoint p;
//    //        p.setX(e->pos().x());
//    //        p.setY(e->pos().y());
//    //        //serie->onClick(p);
//    //        view->refresh();
//    //    } 
//    //}
//
//private:
//    LayeredImageVisualizer* model;
//    LayeredImageVisualizerView* view;
//};

LayeredImageVisualizerView::LayeredImageVisualizerView(LayeredImageVisualizer* model, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
    model(model),
    QWidget(parent, f),
    ui(new Ui::LayeredImageVisualizer()),
    //pixmapItem(nullptr),
    //layersModel(this),
    //stateMachine(new LayeredStateMachine),
    lastView(nullptr)
{
    ui->setupUi(this);
    connect(ui->prevButton, SIGNAL(clicked()), model, SLOT(setPrevSerie()));
    connect(ui->nextButton, SIGNAL(clicked()), model, SLOT(setNextSerie()));
    connect(ui->sliderBar, SIGNAL(valueChanged(int)), model, SLOT(trySetSerie(int)));
    connect(model, SIGNAL(serieChanged()), this, SLOT(refresh()));
    connect(ui->saveButton, SIGNAL(clicked()), model, SLOT(saveSerie()));

    ui->tableView->setItemDelegateForColumn(1, new AdnotationsDelegate());

    coreUI::CoreAction*  und = new coreUI::CoreAction(tr("Edit")  , QIcon(":/coreUI/icons/textedit/editundo.png"), tr("Undo"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  red = new coreUI::CoreAction(tr("Edit")  , QIcon(":/coreUI/icons/textedit/editredo.png"), tr("Redo"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  nrm = new coreUI::CoreAction(tr("State")  , QIcon(":/dicom/arrowIcon.png"), tr("Normal"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  cur = new coreUI::CoreAction(tr("State")  , QIcon(":/dicom/curveIcon.png"), tr("Add curve"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  pol = new coreUI::CoreAction(tr("State")  , QIcon(":/dicom/pathIcon.png"), tr("Add polygon"), this, coreUI::CoreTitleBar::Left);
    

    
    connect(und, SIGNAL(triggered()), this, SLOT(undo()));
    connect(red, SIGNAL(triggered()), this, SLOT(redo()));

    connect(nrm, SIGNAL(triggered()), this, SLOT(normalState()));
    connect(cur, SIGNAL(triggered()), this, SLOT(curveState()));
    connect(pol, SIGNAL(triggered()), this, SLOT(polyState()));

    this->addAction(und);
    this->addAction(red);

    this->addAction(nrm);
    this->addAction(cur);
    this->addAction(pol);
    
    ui->graphicsHolder->setLayout(new QHBoxLayout());
    ui->graphicsHolder->setContentsMargins(0, 0, 0, 0);
}

void dicom::LayeredImageVisualizerView::refresh()
{
    int currentSerieNo = model->getCurrentSerieNo();
    int numSeries = model->getNumSeries();

    ui->sliderBar->blockSignals(true);
    if (currentSerieNo >= 0) {
        ui->sliderBar->setValue(currentSerieNo);
    }
    if (numSeries > 0) {
        ui->sliderBar->setRange(0, numSeries - 1);
        ui->sliderBar->setEnabled(true);
    } else {
        ui->sliderBar->setEnabled(false);
    }
    ui->sliderBar->blockSignals(false);

    /*if (pixmapItem) {
    graphicsScene->removeItem(dynamic_cast<QGraphicsItem*>(pixmapItem));
    }*/

    auto serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    
    if (serie) {
        if (serie->getGraphicsView() != lastView) {
        if (lastView) {
            lastView->hide();
        }
        lastView = serie->getGraphicsView();
        if (lastView->parent() != ui->graphicsHolder) {
            ui->graphicsHolder->layout()->addWidget(lastView);
        }
        lastView->show();
    }
        serie->refresh();
        //pixmapItem = graphicsScene->addPixmap(serie->getPixmap());
        /*serie->getGraphicsView()->setSceneRect(0, 0, serie->getSize().width(), serie->getSize().height());
        serie->getGraphicsScene()->setSceneRect( serie->getGraphicsView()->rect() );*/
        //layersModel.setImage(serie->getImage());
        ui->tableView->setModel(serie->getLayersModel());
    } else {
        //ui->pictureHolder->setPixmap(QPixmap());
    }

}

void dicom::LayeredImageVisualizerView::undo()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->undo();
    }
}

void dicom::LayeredImageVisualizerView::redo()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->redo();
    }
}

void dicom::LayeredImageVisualizerView::normalState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setNormalState();
    }
}

void dicom::LayeredImageVisualizerView::curveState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setCurveState();
    }
}

void dicom::LayeredImageVisualizerView::polyState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setPolyState();
    }
}
