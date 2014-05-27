#include "MedusaExporterPCH.h"
#include "MedusaExporterServiceWidget.h"
#include "ui_MedusaExporter.h"
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <corelib/PluginCommon.h>
#include <corelib/IFileDataManager.h>
#include <corelib/Filesystem.h>
#include "MedusaExporterService.h"
#include <corelib/ISourceManager.h>
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <quazip/JlCompress.h>
#include <boost/bind.hpp>

using namespace medusaExporter;
typedef core::Filesystem fs;



MedusaExporterServiceWidget::MedusaExporterServiceWidget(MedusaExporterService* service) :
    medusaService(service),
    ui(new Ui::MedusaExporter())
{
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(onExport()));
	connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(onDownload()));
	connect(ui->extractButton, SIGNAL(clicked()), this, SLOT(onExtract()));
	connect(ui->extractToButton, SIGNAL(clicked()), this, SLOT(onExtractDirDialog()));
	connect(ui->packImagesButton, SIGNAL(clicked()), this, SLOT(onPackImages()));
	connect(ui->packMetadataButton, SIGNAL(clicked()), this, SLOT(onPackMeta()));
	connect(ui->exportFromButton, SIGNAL(clicked()), this, SLOT(onExportFrom()));

	ui->progressBar->setVisible(false);
	ui->progressBarLabel->setVisible(false);

	callbackFunction = boost::bind(&MedusaExporterServiceWidget::callback, this, ::_1, ::_2);

#ifdef _DEBUG
	ui->extractToLineEdit->setText("E:/MedusaExtractTest");
#endif
}

void medusaExporter::MedusaExporterServiceWidget::onExport()
{
	ExportConfig config;
	config.skipIdentical = ui->skipIdenticalComboBox->isChecked();
	config.pointsDensity = ui->curveDensitySpinBox->value();
	QString user = ui->chooseUserComboBox->currentText();
	if (!user.isEmpty()) {
		QString fileName = QFileDialog::getSaveFileName(this,
			tr("Choose where to save"), "",
			tr("Comma seperated values (*.csv);;All Files (*)"));
		if (!fileName.isEmpty()) {
			try {
				exporterModel->exportData(fileName, user, ui->extractToLineEdit->text(), config, callbackFunction);
			} catch (const std::exception& e) {
				QMessageBox::critical(this, tr("Error"), tr(e.what()));
			} catch (...) {
				QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
			}
		}
	} else {
		QMessageBox::warning(this, tr("Error"), tr("Choose user before exporting"));
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
	QString dir = ui->extractToLineEdit->text();
	if (!dir.isEmpty()) {
		try {
			exporterModel->extractData(dir, callbackFunction);
			setExportFrom(dir);
		} catch (const std::exception& e) {
			QMessageBox::critical(this, tr("Error"), tr(e.what()));
		} catch (...) {
			QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
		}
	} else {
		QMessageBox::warning(this, tr("Error"), tr("Choose valid directory where files will be extracted"));
	}
}

void medusaExporter::MedusaExporterServiceWidget::onExtractDirDialog()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
	ui->extractToLineEdit->setText(dir);
}

void medusaExporter::MedusaExporterServiceWidget::onPackImages()
{
	QDir dir(ui->extractToLineEdit->text());
	if (dir.exists()) {
		QString fileName = QFileDialog::getSaveFileName(this,
			tr("Save images"), "",
			tr("Zip file (*.zip);;All Files (*)"));
		try {
			exporterModel->packImages(dir.path(), fileName, callbackFunction);
		} catch (const std::exception& e) {
			QMessageBox::critical(this, tr("Error"), tr(e.what()));
		} catch (...) {
			QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
		}
	} else {
		QMessageBox::warning(this, tr("Warning"), tr("Valid directory was not set"));
	}
}

void medusaExporter::MedusaExporterServiceWidget::onPackMeta()
{
	QDir dir(ui->extractToLineEdit->text());
	if (dir.exists()) {
		QString fileName = QFileDialog::getSaveFileName(this,
			tr("Save meta data"), "",
			tr("Zip file (*.zip);;All Files (*)"));
		try {
			exporterModel->packMeta(ui->extractToLineEdit->text(), fileName, callbackFunction);
		} catch (const std::exception& e) {
			QMessageBox::critical(this, tr("Error"), tr(e.what()));
		} catch (...) {
			QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
		}
	} else {
		QMessageBox::warning(this, tr("Warning"), tr("Valid directory was not set"));
	}
}

void medusaExporter::MedusaExporterServiceWidget::setExportFrom(const QString& dir)
{
	ui->exportFromLineEdit->setText(dir);

	//ui->chooseFormatComboBox->setEnabled(true);
	ui->curveDensitySpinBox->setEnabled(true);
	ui->skipIdenticalComboBox->setEnabled(true);
	ui->exportButton->setEnabled(true);
	ui->packImagesButton->setEnabled(true);
	ui->packMetadataButton->setEnabled(true);

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
