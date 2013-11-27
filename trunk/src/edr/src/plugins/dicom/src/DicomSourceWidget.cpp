#include "DicomPCH.h"
#include "DicomSourceWidget.h"
#include "ui_DicomSource.h"
#include <QtGui/QFileDialog>
#include <corelib/Filesystem.h>
#include "DicomSource.h"

using namespace dicom;

DicomSourceWidget::DicomSourceWidget( DicomSource* source ) :
    dicomSource(source),
    ui(new Ui::DicomSource)
{
    ui->setupUi(this);
    connect(ui->openProjectButton, SIGNAL(clicked()), this, SLOT(onOpenProject()));
}


void dicom::DicomSourceWidget::onOpenProject()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Main xml file"));
    if ( filePath.isNull() == false ) {
        core::Filesystem::Path path(filePath.toStdString());
        if(core::Filesystem::pathExists(path)) {
            dicomSource->openInternalDataMainFile(path);
        }
    }
}

