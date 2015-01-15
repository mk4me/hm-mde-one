#include "FileSourceWidget.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLayout>
#include <utils/Debug.h> 
#include <corelib/PluginCommon.h>
#include <corelib/ILog.h>
#include <corelib/IFileDataManager.h>
#include <corelib/Filesystem.h>
#include <QtWidgets/QFileDialog>
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

	QPushButton* loadAsfAmcButton = new QPushButton();
	loadAsfAmcButton->setText(tr("ASF AMC"));
	layout->addWidget(loadAsfAmcButton);
	connect(loadAsfAmcButton, SIGNAL(clicked()), this, SLOT(onLoadAsfAmc()));
}

void FileSourceWidget::onLoadFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file"));
    for (auto it = fileNames.begin(); it != fileNames.end(); ++it) {
        core::Filesystem::Path path = it->toStdString();
        fileSource->addFile(path);
    }    
}

void FileSourceWidget::onLoadAsfAmc()
{
	fileSource->loadAsfAmc();
}

