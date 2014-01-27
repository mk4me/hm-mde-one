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


LayeredImageVisualizerView::LayeredImageVisualizerView(LayeredImageVisualizer* model, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
    model(model),
    QWidget(parent, f),
    ui(new Ui::LayeredImageVisualizer()),
    lastView(nullptr)
{
    ui->setupUi(this);
    connect(ui->prevButton, SIGNAL(clicked()), model, SLOT(setPrevSerie()));
    connect(ui->nextButton, SIGNAL(clicked()), model, SLOT(setNextSerie()));
    connect(ui->sliderBar, SIGNAL(valueChanged(int)), model, SLOT(trySetSerie(int)));
    connect(model, SIGNAL(serieChanged()), this, SLOT(refresh()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeSelectedLayers()));
    ui->removeButton->setToolTip(tr("Remove selected tag"));
        
    //ui->treeView->setItemDelegateForColumn(1, new AdnotationsDelegate());
    ui->treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
    connect(ui->treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &)));

    coreUI::CoreAction*  undo = new coreUI::CoreAction(tr("Edit")  , QIcon(":/dicom/undo.png"), tr("Undo"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  redo = new coreUI::CoreAction(tr("Edit")  , QIcon(":/dicom/redo.png"), tr("Redo"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  nrml = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/arrowIcon.png"), tr("Normal"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  move = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/moveIcon.png"), tr("Move"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  edit = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/moveIcon.png"), tr("Edit"), this, coreUI::CoreTitleBar::Left);
/*                                                        
    coreUI::CoreAction*  curv = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/curveIcon.png"), tr("Add curve"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  poly = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/pathIcon.png"), tr("Add polygon"), this, coreUI::CoreTitleBar::Left);*/
                                                          
    coreUI::CoreAction*  bone = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/BoneIcon.png"), tr("Tag bone"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  skin = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/SkinIcon.png"), tr("Tag skin"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  tend = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/TendonIcon.png"), tr("Tag tendon"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  join = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/JointIcon.png"), tr("Tag joint"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  infl = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/InflammatoryIcon.png"), tr("Tag region inflamatory synovitis"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  nois = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/NoiseIcon.png"), tr("Tag noise"), this, coreUI::CoreTitleBar::Left);
     
    coreUI::CoreAction*  save = new coreUI::CoreAction(tr("I/O")  , QIcon(":/dicom/save.png"), tr("Save"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  upld = new coreUI::CoreAction(tr("I/O")  , QIcon(":/dicom/upload.png"), tr("Upload"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  crop = new coreUI::CoreAction(tr("Other")  , QIcon(":/dicom/crop.png"), tr("Crop"), this, coreUI::CoreTitleBar::Left);
    
    connect(undo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(redo, SIGNAL(triggered()), this, SLOT(redo()));

    connect(nrml, SIGNAL(triggered()), this, SLOT(normalState()));
    connect(move, SIGNAL(triggered()), this, SLOT(moveState()));

    connect(bone, SIGNAL(triggered()), this, SLOT(boneState()));
    connect(skin, SIGNAL(triggered()), this, SLOT(skinState()));
    connect(tend, SIGNAL(triggered()), this, SLOT(tendonState()));
    connect(join, SIGNAL(triggered()), this, SLOT(jointState()));
    connect(infl, SIGNAL(triggered()), this, SLOT(inflamatoryState()));
    connect(nois, SIGNAL(triggered()), this, SLOT(noiseState()));

    connect(save, SIGNAL(triggered()), model, SLOT(saveSerie()));
    connect(upld, SIGNAL(triggered()), model, SLOT(uploadSerie()));
    connect(crop, SIGNAL(triggered()), this, SLOT(crop()));

    // tymczasowo wylaczone
    //this->addAction(undo);
    //this->addAction(redo);

    this->addAction(nrml);
    //this->addAction(move);

    this->addAction(bone);
    this->addAction(skin);
    this->addAction(tend);
    this->addAction(join);
    this->addAction(infl);
    this->addAction(nois);

    // hack - tymczasowo ukryte
    //this->addAction(save);
    this->addAction(upld);
    this->addAction(crop);
    
    ui->graphicsHolder->setLayout(new QHBoxLayout());
    ui->graphicsHolder->setContentsMargins(0, 0, 0, 0);
}

void dicom::LayeredImageVisualizerView::refresh()
{
    int currentSerieNo = model->getCurrentSerieNo();
    int numSeries = model->getNumSeries();

    if (numSeries > 1) {
        ui->sliderContainer->show();
    } else {
        ui->sliderContainer->hide();
    }

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

    auto serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    
    if (serie) {
        if (serie->getGraphicsView() != lastView) {
            if (lastView) {
                lastView->hide();
            }
            lastView = serie->getGraphicsView();
            serie->fitToSize();
            if (lastView->parent() != ui->graphicsHolder) {
                ui->graphicsHolder->layout()->addWidget(lastView);
            }
            lastView->show();
        }
        serie->refresh();
        auto treeModel = serie->getLayersModel();
        ui->treeView->setModel(treeModel);
        auto expands = treeModel->getExpands();
        for (auto it = expands.begin(); it != expands.end(); ++it) {
            ui->treeView->setExpanded(it->first, it->second);
        }
    } else {

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


void dicom::LayeredImageVisualizerView::removeSelectedLayers()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    std::set<std::pair<int,int>> rows;
    for (auto it = indexes.begin(); it != indexes.end(); ++it) {
        rows.insert(LayeredModelView::getTagAndIndex(*it));
    }
    if (rows.size() == 1) {
        auto ti = rows.begin();
        model->removeLayer(ti->first, ti->second);
    }
}

void dicom::LayeredImageVisualizerView::editSelectedSerie()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    std::set<std::pair<int,int>> rows;
    for (auto it = indexes.begin(); it != indexes.end(); ++it) {
        rows.insert(LayeredModelView::getTagAndIndex(*it));
    }
    if (rows.size() == 1) {
        auto ti = rows.begin();
        model->editSerie(ti->first, ti->second);
    }
}

void dicom::LayeredImageVisualizerView::selectionChanged(const QModelIndex & )
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    std::set<std::pair<int,int>> rows;
    for (auto it = indexes.begin(); it != indexes.end(); ++it) {
        rows.insert(LayeredModelView::getTagAndIndex(*it));
    }
    if (rows.size() == 1) {
        auto ti = rows.begin();
        model->selectLayer(ti->first, ti->second);
    }

    ui->treeView->blockSignals(true);
    ui->treeView->clearSelection();
    ui->treeView->blockSignals(false);
}

void dicom::LayeredImageVisualizerView::crop()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->switchCrop();
    }
}

void dicom::LayeredImageVisualizerView::moveState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setMoveState();
    }
}

void dicom::LayeredImageVisualizerView::boneState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setBoneState();
    }
}

void dicom::LayeredImageVisualizerView::skinState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setSkinState();
    }
}

void dicom::LayeredImageVisualizerView::tendonState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setTendonState();
    }
}

void dicom::LayeredImageVisualizerView::jointState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setJointState();
    }
}

void dicom::LayeredImageVisualizerView::inflamatoryState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setInflamatoryState();
    }
}

void dicom::LayeredImageVisualizerView::noiseState()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->setNoiseState();
    }
}

QRect dicom::LayeredImageVisualizerView::getSceneRect() const
{
    return ui->graphicsHolderContainer->geometry();
}
