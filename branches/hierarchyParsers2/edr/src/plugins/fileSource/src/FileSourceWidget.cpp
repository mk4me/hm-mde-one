#include "FileSourceWidget.h"
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
#include "FileSource.h"

FileSourceWidget::FileSourceWidget( FileSource* source ) :
    fileSource(source)
{
    QLayout* layout = new QVBoxLayout();
    this->setLayout(layout);

    QPushButton* loadFile = new QPushButton();
    loadFile->setText(tr("Load files"));
    layout->addWidget(loadFile);
    connect(loadFile, SIGNAL(clicked()), this, SLOT(onLoadFiles()));

    QPushButton* removeFiles = new QPushButton();
    removeFiles->setText(tr("Remove files"));
    layout->addWidget(removeFiles);
    connect(removeFiles, SIGNAL(clicked()), this, SLOT(onRemoveFiles()));

    QPushButton* loadRemove = new QPushButton();
    loadRemove->setText(tr("Load and remove"));
    layout->addWidget(loadRemove);
    connect(loadRemove, SIGNAL(clicked()), this, SLOT(onLoadAndRemove()));
}

void FileSourceWidget::onLoadFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file"));
    for (auto it = fileNames.begin(); it != fileNames.end(); ++it) {
        core::Filesystem::Path path = it->toStdString();
        fileSource->addFile(path);
    }    
}

void FileSourceWidget::onRemoveFiles()
{
    fileSource->removeFiles();
}

void FileSourceWidget::onLoadAndRemove()
{
    fileSource->addFile("E:/programming/WORK/EDR/_out_CLEAN/edr/src/mde_view/2010-12-16-B0035-S02-T01.c3d");
    fileSource->removeFiles();
}

