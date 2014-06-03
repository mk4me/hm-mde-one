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
    ui->openFileButton->setVisible(false);
    ui->loadDirectoryButton->setVisible(false);

    connect(ui->importFromButton, SIGNAL(clicked()), this, SLOT(onSelectImportDir()));
    connect(ui->exportToButton, SIGNAL(clicked()), this, SLOT(onSelectSaveDir()));
    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(onImport()));

    connect(ui->updateMetaButton, SIGNAL(clicked()), this, SLOT(onUpdateMeta()));
    

    // TODO : wywalic
#ifdef _DEBUG
    ui->importFromEdit->setText("C:\\Users\\Wojciech\\Desktop\\testImport\\pts 10-12 and 23-26");
    ui->exportToEdit->setText("C:\\Users\\Wojciech\\Desktop\\testImport\\_out");
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
        
        DicomImporter importer(ui->startIndex->value());
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

void dicomImporter::DicomImporterSourceWidget::onUpdateMeta()
{
    core::Filesystem::Path dir = "C:/Users/Wojciech/Desktop/dbrip/data";
    auto subdirs = core::Filesystem::listSubdirectories(dir);
    for (auto iDir = subdirs.begin(); iDir != subdirs.end(); ++iDir) {
        DicomLoader l;
        std::string name = iDir->stem().string() + ".xml";

        auto inter = l.load(*iDir / name);
        UTILS_ASSERT(inter->getNumPatients() == 1);
        UTILS_ASSERT(inter->patients[0]->sessions.size() == 1);
        UTILS_ASSERT(inter->patients[0]->sessions[0]->series.size() == 1);

        auto images = inter->patients[0]->sessions[0]->series[0]->images;
        //auto files = core::Filesystem::listFiles(*iDir, false, "png");
        //UTILS_ASSERT(files.size() == images.size());
        for (auto iImg = images.begin(); iImg != images.end(); ++iImg) {
            (*iImg)->isPowerDoppler = DicomImporter::testPowerDoppler(QPixmap((*iDir / (*iImg)->imageFile).string().c_str()));
        }
        DicomSaver s;
        name += "2";
        s.save("C:/Users/Wojciech/Desktop/dbrip/drop", inter);
    }
}

