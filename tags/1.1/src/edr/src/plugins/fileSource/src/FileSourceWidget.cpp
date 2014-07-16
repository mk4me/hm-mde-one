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
}

void FileSourceWidget::onLoadFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file"));
    for (auto it = fileNames.begin(); it != fileNames.end(); ++it) {
        core::Filesystem::Path path = it->toStdString();
        fileSource->addFile(path);
    }    
}

