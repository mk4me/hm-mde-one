#include "DicomPCH.h"
#include "DicomSourceWidget.h"
#include "ui_DicomSource.h"
#include <QtGui/QFileDialog>
#include <corelib/Filesystem.h>
#include "DicomSource.h"
#include <QtGui/QMessageBox>

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
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if ( dirPath.isNull() == false ) {
        QDir fileDir(dirPath);        
        QFileInfoList fileList = fileDir.entryInfoList();

        QRegExp rxlen("[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}\.xml");        
        BOOST_FOREACH(const QFileInfo &info, fileList) {
            QString filename = info.fileName();
            if(rxlen.exactMatch(filename)) {
                core::Filesystem::Path path(info.filePath().toStdString());
                if(core::Filesystem::pathExists(path)) {
                    dicomSource->openInternalDataMainFile(path);
                    return;
                }
            }
        }
        QMessageBox::warning(this, tr("Warning"), tr("Given folder was not proper"));
    }

}

