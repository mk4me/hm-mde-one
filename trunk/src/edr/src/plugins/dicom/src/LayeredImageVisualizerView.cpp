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
    //connect(ui->saveButton, SIGNAL(clicked()), model, SLOT(saveSerie()));
    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editSelectedSerie()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeSelectedLayers()));

    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView->setItemDelegateForColumn(1, new AdnotationsDelegate());
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &)));

    coreUI::CoreAction*  undo = new coreUI::CoreAction(tr("Edit")  , QIcon(":/dicom/undo.png"), tr("Undo"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  redo = new coreUI::CoreAction(tr("Edit")  , QIcon(":/dicom/redo.png"), tr("Redo"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  nrml = new coreUI::CoreAction(tr("State")  , QIcon(":/dicom/arrowIcon.png"), tr("Normal"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  curv = new coreUI::CoreAction(tr("State")  , QIcon(":/dicom/curveIcon.png"), tr("Add curve"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  poly = new coreUI::CoreAction(tr("State")  , QIcon(":/dicom/pathIcon.png"), tr("Add polygon"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  save = new coreUI::CoreAction(tr("File")  , QIcon(":/dicom/save.png"), tr("Save"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  crop = new coreUI::CoreAction(tr("Other")  , QIcon(":/dicom/crop.png"), tr("Crop"), this, coreUI::CoreTitleBar::Left);
    
    connect(undo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(redo, SIGNAL(triggered()), this, SLOT(redo()));

    connect(nrml, SIGNAL(triggered()), this, SLOT(normalState()));
    connect(curv, SIGNAL(triggered()), this, SLOT(curveState()));
    connect(poly, SIGNAL(triggered()), this, SLOT(polyState()));

    connect(save, SIGNAL(triggered()), model, SLOT(saveSerie()));
    connect(crop, SIGNAL(triggered()), this, SLOT(crop()));

    this->addAction(undo);
    this->addAction(redo);

    this->addAction(nrml);
    this->addAction(curv);
    this->addAction(poly);
    this->addAction(save);
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
        if (lastView->parent() != ui->graphicsHolder) {
            ui->graphicsHolder->layout()->addWidget(lastView);
        }
        lastView->show();
    }
        serie->refresh();
        ui->tableView->setModel(serie->getLayersModel());
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

void dicom::LayeredImageVisualizerView::removeSelectedLayers()
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
    std::set<int> rows;
    for (auto it = indexes.begin(); it != indexes.end(); ++it) {
        rows.insert(it->row());
    }
    if (rows.size() == 1) {
        model->removeLayer(*rows.begin());
    }
}

void dicom::LayeredImageVisualizerView::editSelectedSerie()
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
    std::set<int> rows;
    for (auto it = indexes.begin(); it != indexes.end(); ++it) {
        rows.insert(it->row());
    }
    if (rows.size() == 1) {
        model->editSerie(*rows.begin());
    }
}

void dicom::LayeredImageVisualizerView::selectionChanged(const QModelIndex & )
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
    std::set<int> rows;
    for (auto it = indexes.begin(); it != indexes.end(); ++it) {
        rows.insert(it->row());
    }
    if (rows.size() == 1) {
        model->selectLayer(*rows.begin());
    }
}

void dicom::LayeredImageVisualizerView::crop()
{
    LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        serie->switchCrop();
    }
}
