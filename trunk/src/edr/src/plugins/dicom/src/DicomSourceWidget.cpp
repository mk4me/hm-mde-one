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
    /*QString filePath = QFileDialog::getExistingDirectory(this);
    if ( filePath.isNull() == false ) {
        core::Filesystem::Path path(filePath.toStdString());
        if(core::Filesystem::pathExists(path)) {
            dicomSource->openInternalDataMainFile(path);
        }
    }*/

    /*{
        QString test = "2013-09-03-S0005.xml";
        QRegExp r1(".*S[0-9]{4}\.xml");
        bool test1 = r1.exactMatch(test);

        QRegExp r2(".*S.*\.xml");
        bool test2 = r2.exactMatch(test);

        QRegExp r3("[0-9]{4}-[0-9]{2}-[0-9]{2}-.*");
        bool test3 = r3.exactMatch(test);

        QRegExp r4("[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}\.xml");
        bool test4 = r4.exactMatch(test);

        QRegExp r5(".*-S\d{4}\.xml");
        bool test5 = r5.exactMatch(test);

        QRegExp r6(".*\.xml");
        bool test6 = r6.exactMatch(test);
    }*/


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

