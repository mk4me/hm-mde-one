#include "IMUCostumeConfigurationWizard.h"
#include "ui_IMUCostumeConfigurationWizard.h"
#include <QtGui/QMouseEvent>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QDoubleSpinBox>
#include <QtGui/QStandardItemModel>

Q_DECLARE_METATYPE(IMU::IIMUDataSource::IIMUOrientationEstimationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IIMUDataSource::IMUCostumeCalibrationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IIMUDataSource::IMUCostumeMotionEstimationAlgorithmConstPtr);
Q_DECLARE_METATYPE(osg::Quat);
Q_DECLARE_METATYPE(osg::Vec3);

class SensorJointOffsetDelegate : public QStyledItemDelegate
{
public:
	SensorJointOffsetDelegate(kinematic::JointPtr joint,
		QObject * parent = nullptr) : QStyledItemDelegate(parent)
	{

	}

	virtual ~SensorJointOffsetDelegate()
	{

	}

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		if (joint->children.size() == 1){
			std::unique_ptr<QDoubleSpinBox> editor(new QDoubleSpinBox(parent));
			editor->setMinimum(0);
			//editor->setMaximum(joint->children.front()->position().length());
			editor->setMaximum(5);
			auto val = index.model()->data(index, Qt::DisplayRole).value<osg::Vec3>();
			editor->setValue(val.length());
			return editor.release();
		}
		else{
			//TODO - custom editor for local position: x, y, z
		}		
	}

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override
	{
		if (joint->children.size() == 1){
			QDoubleSpinBox * e = qobject_cast<QDoubleSpinBox*>(editor);
			e->setValue(index.model()->data(index).value<osg::Vec3>().length());
		}
		else{
			//TODO - custom editor for local position: x, y, z
		}		
	}

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
	{
		if (joint->children.size() == 1){
			QDoubleSpinBox * e = qobject_cast<QDoubleSpinBox*>(editor);
			auto val = e->value();
			auto pos = osg::Vec3(1, 0, 0);
			pos.normalize();
			pos *= val;
			QVariant data;
			data.setValue(pos);
			model->setData(index, data);
		}
		else{
			//TODO - custom editor for local position: x, y, z
		}		
	}

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		editor->setGeometry(option.rect);
	}

private:
	kinematic::JointPtr joint;
};

class IMUCostumeConfigurationWizard::JointsItemDelegate : public QStyledItemDelegate
{

public:

	JointsItemDelegate(QCheckBox * oneToOne,
		QObject * parent = nullptr) : QStyledItemDelegate(parent),
		oneToOne(oneToOne)
	{

	}

	virtual ~JointsItemDelegate()
	{

	}

	void setJoints(const std::set<std::string> & joints)
	{
		this->joints = joints;
	}

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		std::unique_ptr<QComboBox> editor(new QComboBox(parent));
		auto val = index.model()->data(index, Qt::DisplayRole).toString();

		std::set<std::string> locJoints(joints);

		if (oneToOne->isChecked() == true){
			auto s = index.model()->rowCount();
			for (unsigned int i = 0; i < s; ++i)
			{
				if (i != index.row())
				{
					locJoints.erase(index.model()->data(index.model()->index(i, 1), Qt::DisplayRole).toString().toStdString());
				}
			}
		}

		editor->addItem(tr("Select joint..."));
		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(editor->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));

		if (joints.empty() == false)
		{
			editor->insertSeparator(1);
		}

		for (const auto & j : joints)
		{
			editor->addItem(QString::fromStdString(j));
		}

		editor->setCurrentText(val);

		return editor.release();
	}

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		e->setCurrentText(index.model()->data(index, Qt::DisplayRole).toString());
	}

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		model->setData(index, e->currentText(), Qt::DisplayRole);
	}

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		editor->setGeometry(option.rect);
	}

private:
	std::set<std::string> joints;
	QCheckBox * oneToOne;
};

class OrientationEstimationAlgorithmsDelegate : public QStyledItemDelegate
{
public:

	OrientationEstimationAlgorithmsDelegate(QObject * parent = nullptr)
		: QStyledItemDelegate(parent)		
	{

	}

	virtual ~OrientationEstimationAlgorithmsDelegate()
	{

	}

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		std::unique_ptr<QComboBox> editor(new QComboBox(parent));
		//editor->setDuplicatesEnabled(true);

		auto val = index.model()->data(index, Qt::DisplayRole).toString();

		editor->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(editor->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(editor->palette().color(QPalette::Disabled, QPalette::Text));

		auto source = core::querySource<IMU::IIMUDataSource>(plugin::getSourceManager());
		if (source != nullptr){
			auto algos = source->orientationEstimationAlgorithms();
			for (auto a : algos)
			{
				QVariant val;
				val.setValue(a);
				editor->addItem(QString::fromStdString(a->name()), val);
			}
		}

		editor->setCurrentText(val);

		return editor.release();
	}

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		e->setCurrentText(index.model()->data(index, Qt::DisplayRole).toString());
	}

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		model->setData(index, e->currentData());
		model->setData(index, e->currentText(), Qt::DisplayRole);
	}

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		editor->setGeometry(option.rect);
	}

private:
	std::set<std::string> joints;
	QCheckBox * oneToOne;
};


IMUCostumeConfigurationWizard::IMUCostumeConfigurationWizard(utils::shared_ptr<imuCostume::CostumeRawIO> costume,
	const unsigned int imusCount, const unsigned int insolesCount)
	: costume(costume), jointsItemDelegate(nullptr)
{
	/*
	if (costume == nullptr){
		throw std::runtime_error("Uninitialized costume");
	}*/

	ui = new Ui::IMUCostumeConfigurationWizard;
	ui->setupUi(this);

	//ui->ipPlaceholderLabel->setText(QString::fromStdString(costume->ip()));
	
	ui->imuSensorsCountPlaceholderLabel->setText(QString("%1").arg(imusCount));
	ui->grfSensorsCountPlaceholderLabel->setText(QString("%1").arg(insolesCount));

	ui->calibrationAlgorithmComboBox->installEventFilter(this);
	ui->motionEstimationAglorithmComboBox->installEventFilter(this);
	ui->singleEstimationAlgorithmComboBox->installEventFilter(this);

	jointsItemDelegate = new JointsItemDelegate(ui->oneToOneMappingCheckBox, ui->sensorsMappingTableWidget);
	ui->sensorsMappingTableWidget->setItemDelegateForColumn(1, jointsItemDelegate);
	//TODO - delegat dla offsetu czujnika
	//ui->sensorsMappingTableWidget->setItemDelegateForColumn(2, new SensorJointOffsetDelegate());

	ui->sensorsEstimationAlgorithmsTableWidget->setItemDelegateForColumn(1, new OrientationEstimationAlgorithmsDelegate(ui->sensorsEstimationAlgorithmsTableWidget));

	ui->sensorsMappingTableWidget->setRowCount(imusCount);
	ui->sensorsEstimationAlgorithmsTableWidget->setRowCount(imusCount);

	for (unsigned int i = 0; i < imusCount; ++i)
	{		
		ui->sensorsMappingTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i)));		
		ui->sensorsMappingTableWidget->item(i, 0)->setFlags(ui->sensorsMappingTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 0), QVariant(i));
		ui->sensorsMappingTableWidget->setItem(i, 1, new QTableWidgetItem(tr("Select joint...")));
		ui->sensorsMappingTableWidget->setItem(i, 2, new QTableWidgetItem(QString("0.0")));
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 2), QVariant(0.0));
		ui->sensorsMappingTableWidget->setItem(i, 3, new QTableWidgetItem(QString("[(0, 0, 0), 1]")));
		QVariant v;
		v.setValue(osg::Quat(0, 0, 0, 1));
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 3), v);

		ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i)));		
		ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->setFlags(ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);
		ui->sensorsEstimationAlgorithmsTableWidget->model()->setData(ui->sensorsEstimationAlgorithmsTableWidget->model()->index(i, 0), QVariant(i));
		ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 1, new QTableWidgetItem(tr("Select algorithm...")));
	}

	ui->sensorsEstimationAlgorithmsTableWidget->setVisible(false);
}

IMUCostumeConfigurationWizard::~IMUCostumeConfigurationWizard()
{

}

bool IMUCostumeConfigurationWizard::eventFilter(QObject * watched, QEvent * event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		if (watched == ui->calibrationAlgorithmComboBox){

			ui->calibrationAlgorithmComboBox->clear();

			ui->calibrationAlgorithmComboBox->addItem(tr("Select algorithm..."));

			const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->calibrationAlgorithmComboBox->model());
			QStandardItem* item = model->item(0);

			item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
			// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
			item->setData(ui->calibrationAlgorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

			auto source = core::querySource<IMU::IIMUDataSource>(plugin::getSourceManager());
			if (source != nullptr){
				auto algos = source->calibrationAlgorithms();

				if (algos.empty() == false){
					ui->calibrationAlgorithmComboBox->insertSeparator(1);
				}

				for (auto a : algos)
				{
					QVariant val;
					val.setValue(a);
					ui->calibrationAlgorithmComboBox->addItem(QString::fromStdString(a->name()), val);
				}
			}
		}
		else if (watched == ui->motionEstimationAglorithmComboBox)
		{
			ui->motionEstimationAglorithmComboBox->clear();

			ui->motionEstimationAglorithmComboBox->addItem(tr("Select algorithm..."));

			const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->motionEstimationAglorithmComboBox->model());
			QStandardItem* item = model->item(0);

			item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
			// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
			item->setData(ui->motionEstimationAglorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

			auto source = core::querySource<IMU::IIMUDataSource>(plugin::getSourceManager());
			if (source != nullptr){
				auto algos = source->motionEstimationAlgorithms();
				for (auto a : algos)
				{
					QVariant val;
					val.setValue(a);
					ui->motionEstimationAglorithmComboBox->addItem(QString::fromStdString(a->name()), val);
				}
			}
		}
		else if (watched == ui->singleEstimationAlgorithmComboBox)
		{
			ui->singleEstimationAlgorithmComboBox->clear();

			ui->singleEstimationAlgorithmComboBox->addItem(tr("Select algorithm..."));

			const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->singleEstimationAlgorithmComboBox->model());
			QStandardItem* item = model->item(0);

			item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
			// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
			item->setData(ui->singleEstimationAlgorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

			auto source = core::querySource<IMU::IIMUDataSource>(plugin::getSourceManager());
			if (source != nullptr){
				auto algos = source->orientationEstimationAlgorithms();
				for (auto a : algos)
				{
					QVariant val;
					val.setValue(a);
					ui->singleEstimationAlgorithmComboBox->addItem(QString::fromStdString(a->name()), val);
				}
			}
		}
	}

	return false;
}

void * IMUCostumeConfigurationWizard::configuredCostumeStream()
{
	return nullptr;
}

void IMUCostumeConfigurationWizard::onEstimationAlgorithmSelectionModeChange(int state)
{
	bool singleAlgorithmMode = (state == Qt::Checked);

	ui->sensorsEstimationAlgorithmsTableWidget->setVisible(!singleAlgorithmMode);
	ui->singleEstimationWidget->setVisible(singleAlgorithmMode);
}