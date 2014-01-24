#include "DicomImporterPCH.h"
#include "DicomImporterSourceWidget.h"
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
#include "DicomImporterSource.h"
#include <QtCore/QDir>
#include <QtGui/QProgressDialog>
#include "DicomImporter.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace dicomImporter;

DicomImporterSourceWidget::DicomImporterSourceWidget( DicomImporterSource* source ) :
    dicomSource(source),
    ui(new Ui::DicomSource)
{
    ui->setupUi(this);
    connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(onLoadFiles()));
    connect(ui->loadDirectoryButton, SIGNAL(clicked()), this, SLOT(onLoadDirectory()));

    connect(ui->importFromButton, SIGNAL(clicked()), this, SLOT(onSelectImportDir()));
    connect(ui->exportToButton, SIGNAL(clicked()), this, SLOT(onSelectSaveDir()));
    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(onImport()));
    

    // TODO : wywalic
#ifdef _DEBUG
    ui->importFromEdit->setText("C:\\Users\\Wojciech\\Desktop\\USG_pas-M0001-M0006\\USG_pas-M0001-M0006");
    ui->exportToEdit->setText("C:\\Users\\Wojciech\\Desktop\\testImport1_6");
#endif
}

//void DicomImporterSourceWidget::onLoadFiles()
//{
//    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file"));
//    for (auto it = fileNames.begin(); it != fileNames.end(); ++it) {
//        core::Filesystem::Path path = it->toStdString();
//        dicomSource->addFile(path);
//    }    
//}

//void DicomImporterSourceWidget::onLoadDirectory()
//{
//    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Directory"));
//    if ( dirPath.isNull() == false ) {
//        core::Filesystem::Path path(dirPath.toStdString());
//        if(core::Filesystem::pathExists(path)) {
//            dicomSource->loadDirFile(path);
//        }
//    }
//}

//void dicomImporter::DicomImporterSourceWidget::onOpenProject()
//{
//    QString filePath = QFileDialog::getOpenFileName(this, tr("Main xml file"));
//    if ( filePath.isNull() == false ) {
//        core::Filesystem::Path path(filePath.toStdString());
//        if(core::Filesystem::pathExists(path)) {
//            dicomSource->openInternalDataMainFile(path);
//        }
//    }
//}

class Refresher
{
public:
    Refresher(QProgressDialog* dlg, DicomImporter* imp) : 
      dialog(dlg),
      importer(imp),
      counter(0)
      {
          importer->setCallBack( boost::bind( &Refresher::refresh, this, ::_1 ));
          QApplication::processEvents();
      }

private:
    void refresh(const std::string& str) {
        dialog->setValue(++counter);
    }
private:
    QProgressDialog* dialog;
    DicomImporter* importer;
    int counter;
};

void dicomImporter::DicomImporterSourceWidget::onImport()
{
    
    
    QString importFrom = ui->importFromEdit->text();
    QString exportTo = ui->exportToEdit->text();

    QDir dir;
    if ( dir.exists(importFrom) && dir.exists(exportTo) ) {
        core::Filesystem::Path from = importFrom.toStdString();
        core::Filesystem::Path to = exportTo.toStdString();
        //dicomSource->import(from, to);
        
        DicomImporter importer;
        auto inter = importer.import(from);
        QProgressDialog progress("Importing files...", "Abort", 0, inter->getNumImages(), this);
        progress.setWindowModality(Qt::WindowModal);
        Refresher r(&progress, &importer);
        std::vector<DicomInternalStructPtr> splits = importer.split(inter);

        DicomSaver saver;
        for (auto it = splits.begin(); it != splits.end(); ++it) {
            importer.convertImages(*it, from, to);
            saver.save(to, *it);
        }

    } else {
        QMessageBox::critical(this, tr("Error"), tr("Unable to import DICOM structure, check directories"));
    }

}

void dicomImporter::DicomImporterSourceWidget::onSelectImportDir()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Directory"));
    if ( dirPath.isNull() == false ) {
        ui->importFromEdit->setText(dirPath);
    }
}

void dicomImporter::DicomImporterSourceWidget::onSelectSaveDir()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Directory"));
    if ( dirPath.isNull() == false ) {
        ui->exportToEdit->setText(dirPath);
    }
}


