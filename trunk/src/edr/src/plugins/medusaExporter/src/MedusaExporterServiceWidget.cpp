#include "MedusaExporterPCH.h"
#include "MedusaExporterServiceWidget.h"
#include "ui_MedusaExporter.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#include <corelib/PluginCommon.h>
#include <corelib/IFileDataManager.h>
#include <corelib/Filesystem.h>
#include "MedusaExporterService.h"
#include <corelib/ISourceManager.h>
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <quazip/JlCompress.h>
#include <boost/bind.hpp>
#include <QtCore/QUrl>
#include <tuple>

using namespace medusaExporter;
typedef core::Filesystem fs;




MedusaExporterServiceWidget::MedusaExporterServiceWidget(MedusaExporterService* service) :
    medusaService(service),
    ui(new Ui::MedusaExporter())
{
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(onExport()));
	connect(ui->extractButton, SIGNAL(clicked()), this, SLOT(onExtract()));
	connect(ui->extractToButton, SIGNAL(clicked()), this, SLOT(onExtractDirDialog()));
	connect(ui->exportFromButton, SIGNAL(clicked()), this, SLOT(onExportFrom()));
    connect(ui->imagesCheck, SIGNAL(stateChanged(int)), this, SLOT(onPackChecks()));
    connect(ui->metaCheck, SIGNAL(stateChanged(int)), this, SLOT(onPackChecks()));
    connect(ui->clearMedusaFolderButton, SIGNAL(clicked()), this, SLOT(onClearMedusaExport()));
    connect(ui->openMedusaFolderButton, SIGNAL(clicked()), this, SLOT(onOpenMedusaExport()));

    ui->chooseFormatComboBox->addItem("CSV");

	ui->progressBar->setVisible(false);
	ui->progressBarLabel->setVisible(false);

	callbackFunction = boost::bind(&MedusaExporterServiceWidget::callback, this, ::_1, ::_2);
}

void medusaExporter::MedusaExporterServiceWidget::onExport()
{    
    
    QString user = ui->chooseUserComboBox->currentText();
    QDir exportFrom(ui->exportFromLineEdit->text() + "/data");
    QDir outDir(ui->exportFromLineEdit->text());

    if (!exportFrom.exists()) {
        QMessageBox::warning(this, tr("Error"), tr("Data export doesn't exist. Please Export Data before Export Attributes"));
        return;
    }

    if (user.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Choose user before exporting"));
        return;
    }


	ExportConfig config;
	config.skipIdentical = ui->skipIdenticalComboBox->isChecked();
	config.pointsDensity = ui->curveDensitySpinBox->value();
    config.normalizePointVectorsLenght = ui->normalizePointsCheck->isChecked();

	try {
        ExporterThread* t = new ExporterThread();
        connect(t, SIGNAL(progressChanged(double, const QString&)), this, SLOT(callback(double, const QString&)), Qt::QueuedConnection);
        connect(t, SIGNAL(finished()), this, SLOT(afterExport()));
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        connect(t, SIGNAL(terminated()), this, SLOT(afterExport()));
        connect(t, SIGNAL(terminated()), t, SLOT(deleteLater()));
        
		//exporterModel->exportData(outDir.absolutePath(), user, exportFrom.absolutePath(), config, callbackFunction);
        CallbackCollector::Operation op = boost::bind(&ExporterModel::exportData, exporterModel.get(),
                                                      outDir.absolutePath(), user, exportFrom.absolutePath(), config, ::_1);
        t->addOperation(op, 1.0, tr("Exporting"));

        disabler = utils::make_shared<WidgetDisabler>(ui->miscBox, ui->dataBox);
        disabler->addWidget(ui->exportFromButton);
        disabler->addWidget(ui->normalizePointsCheck);
        disabler->addWidget(ui->skipIdenticalComboBox);
        //disabler->addWidget(ui->chooseFormatComboBox);
        disabler->addWidget(ui->curveDensitySpinBox);
        disabler->addWidget(ui->chooseUserComboBox);
        disabler->addWidget(ui->exportButton);
        disabler->checkButton(ui->exportButton);
        t->start();
	} catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Error"), tr(e.what()));
	} catch (...) {
		QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
	}
}

void medusaExporter::MedusaExporterServiceWidget::onDownload()
{
	try {
		exporterModel->downloadAll();
	} catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Error"), tr(e.what()));
	} catch (...) {
		QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
	}
	this->close();
}


void medusaExporter::MedusaExporterServiceWidget::setExporter(ExporterModelPtr exporterModel)
{
	this->exporterModel = exporterModel;
}

void medusaExporter::MedusaExporterServiceWidget::onExtract()
{    
    
	QString dirPath = ui->extractToLineEdit->text();

    communication::ICommunicationDataSourcePtr icomm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
    if (!icomm || !icomm->isLogged()) {
        QMessageBox::critical(this, tr("Error"), tr("User is not logged"));
        return;
    }

    if (dirPath.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Choose directory where files will be exported"));
        return;
    }

    QDir dir(dirPath);
    if (!dir.exists()) {
        auto create = QMessageBox::question(this, tr("Warning"), tr("Folder does not exist. Create?"), QMessageBox::Yes | QMessageBox::No);
        if (create == QMessageBox::Yes) {
            if (!dir.mkpath(".")) {
                QMessageBox::critical(this, tr("Error"), tr("Unable to create directory"));
                return;
            }
        } else {
            return;
        }
    }

    QString innerDirPath = dirPath + "/" + "Export_" + QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss");
    QDir innerDir(innerDirPath);
    QDir innerDataDir(innerDirPath + "/" + "Data");
    innerDataDir.mkpath(".");
	try {
        //CallbackCollector cc(callbackFunction);
        ExporterThread* t = new ExporterThread();
        connect(t, SIGNAL(progressChanged(double, const QString&)), this, SLOT(callback(double, const QString&)), Qt::QueuedConnection);
        connect(t, SIGNAL(finished()), this, SLOT(afterExtract()));
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        connect(t, SIGNAL(terminated()), this, SLOT(afterExtract()));
        connect(t, SIGNAL(terminated()), t, SLOT(deleteLater()));
               
        
        //exporterModel->extractData(innerDataDir.absolutePath(), callbackFunction);
        auto extractOp = boost::bind(&ExporterModel::extractData, exporterModel.get(), innerDataDir.absolutePath(), ::_1);
        t->addOperation(extractOp, 0.2, tr("Exporting"));
        QString fileName = dirPath + "/" + innerDir.dirName() + "/";
        if (ui->metaCheck->isChecked() && ui->imagesCheck->isChecked() && ui->togetherRadio->isChecked()) {
            fileName += "all.zip";
            //exporterModel->packBoth(innerDirPath, fileName, callbackFunction);
            auto compressOp = boost::bind(&ExporterModel::packBoth, exporterModel.get(), innerDirPath, fileName, ::_1);
            t->addOperation(compressOp, 0.8, tr("Compressing"));
        } else {
            if (ui->metaCheck->isChecked()) {
                QString fn = fileName + "metadata.zip";
                //exporterModel->packMeta(innerDirPath, fn, callbackFunction);
                auto compressOp = boost::bind(&ExporterModel::packMeta, exporterModel.get(), innerDirPath, fn, ::_1);
                t->addOperation(compressOp, 0.2, tr("Compressing metadata"));
            }
            if (ui->imagesCheck->isChecked()) {
                QString fn = fileName + "images.zip";
                //exporterModel->packImages(innerDirPath, fn, callbackFunction);
                CallbackCollector::Operation compressOp = boost::bind(&ExporterModel::packImages, exporterModel.get(), innerDirPath, fn, ::_1);
                t->addOperation(compressOp, 0.6, tr("Compressing images"));
            }
        }
        innerExportDirPath = innerDirPath;
        auto collectUsers = [&](ExporterModel::CallbackFunction f) {
            this->tempUsers = this->exporterModel->getUsers(this->innerExportDirPath, f);
        };
        //auto usersOp = boost::bind(&MedusaExporterServiceWidget::setExportFrom, this, innerDirPath, ::_1);
        t->addOperation(collectUsers, 0.05, tr("Collecting users"));
        disabler = utils::make_shared<WidgetDisabler>(ui->miscBox, ui->atributesBox);
        disabler->addWidget(ui->compressOptionBox);
        disabler->addWidget(ui->compressBox);
        disabler->addWidget(ui->extractButton);
        disabler->addWidget(ui->extractToButton);
        disabler->checkButton(ui->extractButton);
        t->start();

        ui->extractButton->setChecked(false);

	} catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Error"), tr(e.what()));
	} catch (...) {
		QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
	}
}

void medusaExporter::MedusaExporterServiceWidget::onExtractDirDialog()
{
	QString dir = QFileDialog::getExistingDirectory(this, QString());
	ui->extractToLineEdit->setText(dir);
}

void medusaExporter::MedusaExporterServiceWidget::setExportFrom(const QString& dir, const QStringList& users)
{
	ui->exportFromLineEdit->setText(QDir::toNativeSeparators(dir));
    QDir d(dir);
    ui->curveDensitySpinBox->setEnabled(true);
	ui->skipIdenticalComboBox->setEnabled(true);
	ui->exportButton->setEnabled(true);
	ui->chooseUserComboBox->setEnabled(true);
    ui->chooseUserComboBox->clear();
	ui->chooseUserComboBox->addItems(users);
}

void medusaExporter::MedusaExporterServiceWidget::onExportFrom()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
	ui->extractToLineEdit->setText(dir);
	if (!dir.isEmpty()) {
        auto users = exporterModel->getUsers(dir, callbackFunction);
		setExportFrom(dir, users);
	}
}

void medusaExporter::MedusaExporterServiceWidget::callback(double ratio, const QString& description)
{
	ui->progressBar->setVisible(true);
	ui->progressBar->setValue(ratio * 100);
	ui->progressBarLabel->setVisible(true);
	ui->progressBarLabel->setText(description);
	QApplication::processEvents();
}

void medusaExporter::MedusaExporterServiceWidget::onPackChecks()
{
    if (ui->imagesCheck->isChecked() && ui->metaCheck->isChecked()) {
        ui->compressOptionBox->setEnabled(true);
    } else {
        ui->compressOptionBox->setEnabled(false);
    }
}

void medusaExporter::MedusaExporterServiceWidget::setExportDir(const QString& dir)
{
    ui->extractToLineEdit->setText(dir);
}

QString medusaExporter::MedusaExporterServiceWidget::getExportDir() const
{
    return ui->extractToLineEdit->text();
}

void medusaExporter::MedusaExporterServiceWidget::onClearMedusaExport()
{
    auto create = QMessageBox::question(this, tr("Warning"), tr("Are you sure you want to delete whole export folder?"), QMessageBox::Yes | QMessageBox::No);
    if (create == QMessageBox::Yes) {
        ExporterThread* t = new ExporterThread();
        connect(t, SIGNAL(progressChanged(double, const QString&)), this, SLOT(callback(double, const QString&)), Qt::QueuedConnection);
        connect(t, SIGNAL(finished()), this, SLOT(afterClear()));
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        connect(t, SIGNAL(terminated()), this, SLOT(afterClear()));
        auto op = [&](ExporterModel::CallbackFunction f) {
            f(0.0, tr("Started"));
            this->exporterModel->clearMedusaExportDir();
            f(1.0, tr("Done"));
        }; 
        t->addOperation(op, 1.0, tr("Clearing folder"));
        disabler = utils::make_shared<WidgetDisabler>(ui->dataBox, ui->atributesBox);
        disabler->addWidget(ui->openMedusaFolderButton);
        disabler->addWidget(ui->clearMedusaFolderButton);
        disabler->checkButton(ui->clearMedusaFolderButton);
        t->start();

        ui->exportFromLineEdit->setText("");
        ui->chooseUserComboBox->clear();
    }
}

void medusaExporter::MedusaExporterServiceWidget::onOpenMedusaExport()
{
    QString path = QString::fromStdString(plugin::getUserDataPath("Export").string());
    QDir dir(QDir::toNativeSeparators(path));
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QDesktopServices::openUrl(QUrl("file:///" + dir.path()));
}

void medusaExporter::MedusaExporterServiceWidget::afterExtract()
{
    UTILS_ASSERT(this->innerExportDirPath.isEmpty() == false);
    UTILS_ASSERT(this->tempUsers.isEmpty() == false);
    setExportFrom(this->innerExportDirPath, tempUsers);
    disabler = WidgetDisablerPtr();
}

void medusaExporter::MedusaExporterServiceWidget::afterExport()
{
    //setExportFrom(this->innerExportDirPath, tempUsers);
    disabler = WidgetDisablerPtr();
}

void medusaExporter::MedusaExporterServiceWidget::afterClear()
{
    disabler = WidgetDisablerPtr();
}
