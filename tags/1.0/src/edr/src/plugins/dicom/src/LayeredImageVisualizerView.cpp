#include "DicomPCH.h"
#include "LayeredImageVisualizerView.h"
#include "ui_layeredImageVisualizer.h"
#include "LayeredImageVisualizer.h"

using namespace dicom;

LayeredImageVisualizerView::LayeredImageVisualizerView(LayeredImageVisualizer* model, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
    model(model),
    QWidget(parent, f),
    ui(new Ui::LayeredImageVisualizer())
{
    ui->setupUi(this);
    connect(ui->prevButton, SIGNAL(clicked()), model, SLOT(setPrevSerie()));
    connect(ui->nextButton, SIGNAL(clicked()), model, SLOT(setNextSerie()));
    connect(ui->sliderBar, SIGNAL(valueChanged(int)), model, SLOT(trySetSerie(int)));
    connect(model, SIGNAL(serieChanged()), this, SLOT(refresh()));
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


    auto serie = dynamic_cast<const LayeredSerie*>(model->getActiveSerie());
    if (serie) {
        ui->pictureHolder->setPixmap(serie->getPixmap());
    } else {
        ui->pictureHolder->setPixmap(QPixmap());
    }
}
