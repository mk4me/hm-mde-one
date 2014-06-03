#include "MedusaExporterPCH.h"
#include "MedusaExporterServiceWidget.h"
#include "ui_MedusaExporter.h"
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
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
        QMessageBox::warning(this, tr("Error"), tr("'Export from' directory does not exist"));
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
		exporterModel->exportData(outDir.absolutePath(), user, exportFrom.absolutePath(), config, callbackFunction);
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
        exporterModel->extractData(innerDataDir.absolutePath(), callbackFunction);
        setExportFrom(innerDirPath);
        QString fileName = dirPath + "/" + innerDir.dirName() + "/";
        if (ui->metaCheck->isChecked() && ui->imagesCheck->isChecked() && ui->togetherRadio->isChecked()) {
            fileName += "all.zip";
            exporterModel->packBoth(innerDirPath, fileName, callbackFunction);
        } else {
            if (ui->metaCheck->isChecked()) {
                QString fn = fileName + "metadata.zip";
                exporterModel->packMeta(innerDirPath, fn, callbackFunction);
            }
            if (ui->imagesCheck->isChecked()) {
                QString fn = fileName + "images.zip";
                exporterModel->packImages(innerDirPath, fn, callbackFunction);
            }
        }

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

void medusaExporter::MedusaExporterServiceWidget::setExportFrom(const QString& dir)
{
	ui->exportFromLineEdit->setText(dir);
    QDir d(dir);
    ui->curveDensitySpinBox->setEnabled(true);
	ui->skipIdenticalComboBox->setEnabled(true);
	ui->exportButton->setEnabled(true);
	ui->chooseUserComboBox->setEnabled(true);
	ui->chooseUserComboBox->addItems(exporterModel->getUsers(dir));
}

void medusaExporter::MedusaExporterServiceWidget::onExportFrom()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
	ui->extractToLineEdit->setText(dir);
	if (!dir.isEmpty()) {
		setExportFrom(dir);
	}
}

void medusaExporter::MedusaExporterServiceWidget::callback(float ratio, const QString& description)
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
        ui->packOptionGroup->setEnabled(true);
    } else {
        ui->packOptionGroup->setEnabled(false);
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
        exporterModel->clearMedusaExportDir();
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
