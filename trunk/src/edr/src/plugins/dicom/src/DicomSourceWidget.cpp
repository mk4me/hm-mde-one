#include "DicomPCH.h"
#include "DicomSourceWidget.h"
#include "ui_DicomSource.h"
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QLayout>
#include <utils/Debug.h> 
#include <corelib/PluginCommon.h>
#include <corelib/ILog.h>
#include <corelib/IFileDataManager.h>
#include <corelib/Filesystem.h>
#include <QtGui/QFileDialog>
#include "DicomSource.h"
#include <QtCore/QDir>

using namespace dicom;

DicomSourceWidget::DicomSourceWidget( DicomSource* source ) :
    dicomSource(source),
    ui(new Ui::DicomSource)
{
    ui->setupUi(this);
    connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(onLoadFiles()));
    connect(ui->loadDirectoryButton, SIGNAL(clicked()), this, SLOT(onLoadDirectory()));

    /*QLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    QPushButton* loadFile = new QPushButton();
    loadFile->setText(tr("Load files"));
    layout->addWidget(loadFile);
    connect(loadFile, SIGNAL(clicked()), this, SLOT(onLoadFiles()));

    QPushButton* loadDictionary = new QPushButton();
    loadDictionary->setText(tr("Select directory to load"));
    layout->addWidget(loadDictionary);
    connect(loadDictionary, SIGNAL(clicked()), this, SLOT(onLoadDirectory()));*/
}

void DicomSourceWidget::onLoadFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file"));
    for (auto it = fileNames.begin(); it != fileNames.end(); ++it) {
        core::Filesystem::Path path = it->toStdString();
        dicomSource->addFile(path);
    }    
}

void DicomSourceWidget::onLoadDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Directory"));
    if ( dirPath.isNull() == false )
    {
        core::Filesystem::Path path(dirPath.toStdString());
        if(core::Filesystem::pathExists(path)) {
            dicomSource->loadDirFile(path);
        }
        /*QDir p(path);
        core::Filesystem::Path path(p.filePath("DICOMDIR").toStdString());
        if(core::Filesystem::pathExists(path)) {
        dicomSource->loadDirFile(path);
        }*/
    }
}

