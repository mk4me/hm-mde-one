#include "RecordingWizard.h"
#include "ui_RecordingWizard.h"
#include <QtWidgets/QFileDialog>
#include <corelib/PluginCommon.h>

Q_DECLARE_METATYPE(IMU::IIMUDataSource::CostumeID);

RecordingWizard::RecordingWizard(const std::set<IMU::IIMUDataSource::CostumeID> & costumes,
	QWidget *parent, Qt::WindowFlags flags)
	: QWizard(parent, flags), ui(new Ui::RecordingWizard)
{
	if (costumes.empty() == true){
		throw std::runtime_error("No costumes to choose for recording");
	}

	ui->setupUi(this);

	for (const auto & id : costumes)
	{
		QListWidgetItem* item = new QListWidgetItem(QString("%1:%2").arg(id.ip.c_str()).arg(id.port), ui->costumesToRecordListWidget);
		item->setData(Qt::UserRole, QVariant::fromValue(id));
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
		item->setCheckState(Qt::Unchecked); // AND initialize check state
		ui->costumesToRecordListWidget->addItem(item);
	}

	ui->costumesToRecordListWidget->item(0)->setCheckState(Qt::Checked);
}

RecordingWizard::~RecordingWizard()
{

}

QString RecordingWizard::outputPath() const
{
	return ui->outputLineEdit->text();
}

std::set<IMU::IIMUDataSource::CostumeID> RecordingWizard::costumes() const
{
	std::set<IMU::IIMUDataSource::CostumeID> ret;

	for (unsigned int i = 0; i < ui->costumesToRecordListWidget->count(); ++i)
	{
		auto item = ui->costumesToRecordListWidget->item(i);
		if (item->checkState() == Qt::Checked){
			ret.insert(item->data(Qt::UserRole).value<IMU::IIMUDataSource::CostumeID>());
		}
	}

	return std::move(ret);
}

void RecordingWizard::onCostumeSelectionChange(QListWidgetItem* item)
{
	if (item->checkState() == Qt::Checked){
		button(QWizard::NextButton)->setEnabled(true);
	}
	else{
		
		bool found = false;
		for (unsigned int i = 0; i < ui->costumesToRecordListWidget->count(); ++i)
		{
			auto item = ui->costumesToRecordListWidget->item(i);

			if (item->checkState() == Qt::Checked){
				found = true;
				break;
			}
		}

		button(QWizard::NextButton)->setEnabled(found);
	}
}

void RecordingWizard::onChoose()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose output file"),
		plugin::getPaths()->getUserDataPath().string().c_str(),
		tr("Costume data (*.cdata)"));

	if (fileName.isEmpty() == false){
		
		ui->outputLineEdit->setText(fileName);		

	}
}

bool verifyFilePath(const QString & path)
{
	bool ret = false;

	QFileInfo fi(path);

	if (fi.isFile() == true || QFileInfo::exists(fi.absolutePath()) == true)
	{
		ret = true;
	}

	return ret;
}

void RecordingWizard::onOutputFilePathEdit(const QString & path)
{
	button(QWizard::FinishButton)->setEnabled(verifyFilePath(path));
}

void RecordingWizard::pageChanged(int idx)
{
	if (idx == 1){
		button(QWizard::FinishButton)->setEnabled(verifyFilePath(ui->outputLineEdit->text()));
	}
}