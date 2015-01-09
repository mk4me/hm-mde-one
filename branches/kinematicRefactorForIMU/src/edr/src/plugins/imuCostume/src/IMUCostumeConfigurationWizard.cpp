#include "IMUCostumeConfigurationWizard.h"
#include "ui_IMUCostumeConfigurationWizard.h"
#include <QtGui/QMouseEvent>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QDoubleSpinBox>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>
#include <coreui/CoreRotationWidgetEditor.h>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include "OrientationDelegate.h"

Q_DECLARE_METATYPE(IMU::IIMUOrientationEstimationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeCalibrationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeMotionEstimationAlgorithmConstPtr);

Q_DECLARE_METATYPE(kinematic::JointConstPtr);

Q_DECLARE_METATYPE(osg::Vec3d);

Q_DECLARE_METATYPE(kinematic::SkeletonConstPtr);

using namespace IMU;

class SensorJointOffsetDelegate : public QStyledItemDelegate
{
public:
	SensorJointOffsetDelegate(QObject * parent = nullptr) : QStyledItemDelegate(parent)
	{

	}

	virtual ~SensorJointOffsetDelegate()
	{

	}

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		QWidget * retEditor = nullptr;
		auto row = index.row();
		auto data = index.model()->data(index.model()->index(row, 1), Qt::UserRole);
		if (data.canConvert<kinematic::JointConstPtr>() == true){
			auto joint = data.value<kinematic::JointConstPtr>();
			if (joint != nullptr){
				if (joint->children.size() == 1){
					std::unique_ptr<QDoubleSpinBox> editor(new QDoubleSpinBox(parent));
					editor->setMinimum(0);
					editor->setMaximum(joint->children.front()->position.length());					
					const auto val = index.model()->data(index, Qt::UserRole).value<osg::Vec3d>();
					editor->setValue(val.length());
					return editor.release();
				}
				else{
					//TODO - custom editor for local position: x, y, z
				}
			}
		}

		return retEditor;
	}

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override
	{
		auto row = index.row();
		auto data = index.model()->data(index.model()->index(row, 1), Qt::UserRole);
		if (data.canConvert<kinematic::JointConstPtr>() == true){
			auto joint = data.value<kinematic::JointConstPtr>();
			if (joint != nullptr){
				if (joint->children.size() == 1){
					QDoubleSpinBox * e = qobject_cast<QDoubleSpinBox*>(editor);
					auto direction = joint->children.front()->position;				
					e->setValue(direction.length());
				}
				else{
					//TODO - custom editor for local position: x, y, z
				}
			}
		}		
	}

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
	{
		auto row = index.row();
		auto data = index.model()->data(index.model()->index(row, 1), Qt::UserRole);
		if (data.canConvert<kinematic::JointConstPtr>() == true){
			auto joint = data.value<kinematic::JointConstPtr>();
			if (joint != nullptr){
				if (joint->children.size() == 1){
					QDoubleSpinBox * e = qobject_cast<QDoubleSpinBox*>(editor);
					osg::Vec3d direction = joint->children.front()->position;
					direction.normalize();
					direction *= e->value();
					model->setData(index, QVariant::fromValue(direction), Qt::UserRole);
					model->setData(index, QString("(%1, %2, %3)").arg(direction[0]).arg(direction[1]).arg(direction[2]));
				}
				else{
					//TODO - custom editor for local position: x, y, z
				}
			}
		}
	}

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		editor->setGeometry(option.rect);
	}
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

	static void fillMap(std::map<std::string, kinematic::JointConstPtr> & map, kinematic::JointConstPtr joint)
	{
		map.insert(std::map<std::string, kinematic::JointConstPtr>::value_type(joint->name, joint));
		for (const auto & j : joint->children)
		{
			fillMap(map, j);
		}
	}

	void setJoints(kinematic::JointConstPtr root)
	{
		fillMap(joints, root);		
	}

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		std::unique_ptr<QComboBox> editor(new QComboBox(parent));
		auto val = index.model()->data(index).toString();

		std::map<std::string, kinematic::JointConstPtr> locJoints(joints);

		if (oneToOne->isChecked() == true){
			auto s = index.model()->rowCount();
			for (unsigned int i = 0; i < s; ++i)
			{
				if (i != index.row())
				{
					auto jointName = index.model()->data(index.model()->index(i, 1)).toString().toStdString();
					locJoints.erase(jointName);
				}
			}
		}

		editor->addItem(tr("Select joint..."));
		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(editor->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));

		if (locJoints.empty() == false)
		{
			editor->insertSeparator(1);
		}

		for (const auto & j : locJoints)
		{
			editor->addItem(QString::fromStdString(j.first), QVariant::fromValue(j.second));
		}

		editor->setCurrentText(val);

		return editor.release();
	}

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		e->setCurrentText(index.model()->data(index).toString());
	}

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		model->setData(index, e->currentText());
	}

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		editor->setGeometry(option.rect);
	}

private:
	std::map<std::string, kinematic::JointConstPtr> joints;
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

		auto val = index.model()->data(index).toString();

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
				editor->addItem(QString::fromStdString(a->name()), QVariant::fromValue(a));
			}
		}

		editor->setCurrentText(val);

		return editor.release();
	}

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		e->setCurrentText(index.model()->data(index).toString());
	}

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
	{
		QComboBox * e = qobject_cast<QComboBox*>(editor);
		model->setData(index, e->currentData(), Qt::UserRole);
		model->setData(index, e->currentText());
	}

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		editor->setGeometry(option.rect);
	}

private:
	std::set<std::string> joints;
	QCheckBox * oneToOne;
};


IMUCostumeConfigurationWizard::IMUCostumeConfigurationWizard(const std::string & name,
	const std::string & ip,
	const unsigned int imusCount,
	const unsigned int insolesCount)
	: jointsItemDelegate(nullptr)
{
	ui = new Ui::IMUCostumeConfigurationWizard;
	ui->setupUi(this);

	ui->ipPlaceholderLabel->setText(QString::fromStdString(ip));
	
	ui->imuSensorsCountPlaceholderLabel->setText(QString("%1").arg(imusCount));
	ui->grfSensorsCountPlaceholderLabel->setText(QString("%1").arg(insolesCount));

	ui->calibrationAlgorithmComboBox->installEventFilter(this);
	ui->motionEstimationAglorithmComboBox->installEventFilter(this);
	ui->singleEstimationAlgorithmComboBox->installEventFilter(this);

	ui->sensorsMappingTableWidget->setRowCount(imusCount);
	ui->sensorsEstimationAlgorithmsTableWidget->setRowCount(imusCount);

	ui->modelComboBox->addItem(tr("Select model..."));

	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->modelComboBox->model());
	QStandardItem* item = model->item(0);

	item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
	item->setData(ui->modelComboBox->palette().color(QPalette::Disabled, QPalette::Text));

	auto source = core::querySource<IMU::IIMUDataSource>(plugin::getSourceManager());
	if (source != nullptr){
		auto models = source->skeletonModels();

		if (models.empty() == false){
			ui->modelComboBox->insertSeparator(1);
		}

		for (auto m : models)
		{

			unsigned int jointsCount = 0;

			kinematic::Joint::visit(m->root, [&jointsCount](kinematic::JointConstPtr joint) -> void
			{
				if (joint->children.empty() == false){
					++jointsCount;
				}
			});

			ui->modelComboBox->addItem(tr("Model %1: %2 joints").arg(QString::fromStdString(m->name)).arg(jointsCount), QVariant::fromValue(m));
		}
	}

	for (unsigned int i = 0; i < imusCount; ++i)
	{		
		ui->sensorsMappingTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i)));		
		ui->sensorsMappingTableWidget->item(i, 0)->setFlags(ui->sensorsMappingTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 0), QVariant(i));
		ui->sensorsMappingTableWidget->setItem(i, 1, new QTableWidgetItem(tr("Select joint...")));
		ui->sensorsMappingTableWidget->setItem(i, 2, new QTableWidgetItem(QString("(0, 0, 0)")));
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 2), QVariant::fromValue(osg::Vec3d(0,0,0)), Qt::UserRole);
		ui->sensorsMappingTableWidget->setItem(i, 3, new QTableWidgetItem(QString("[(0, 0, 0), 1]")));
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 3), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);

		ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i)));		
		ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->setFlags(ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);
		ui->sensorsEstimationAlgorithmsTableWidget->model()->setData(ui->sensorsEstimationAlgorithmsTableWidget->model()->index(i, 0), QVariant(i));
		ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 1, new QTableWidgetItem(tr("Select algorithm...")));
	}

	jointsItemDelegate = new JointsItemDelegate(ui->oneToOneMappingCheckBox, ui->sensorsMappingTableWidget);
	ui->sensorsMappingTableWidget->setItemDelegateForColumn(1, jointsItemDelegate);
	//delegat dla offsetu czujnika	
	ui->sensorsMappingTableWidget->setItemDelegateForColumn(2, new SensorJointOffsetDelegate);	
	//delegat dla orientacji	
	ui->sensorsMappingTableWidget->setItemDelegateForColumn(3, new OrientationDelegate);
	
	ui->sensorsEstimationAlgorithmsTableWidget->setItemDelegateForColumn(1, new OrientationEstimationAlgorithmsDelegate(ui->sensorsEstimationAlgorithmsTableWidget));

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

void IMUCostumeConfigurationWizard::onEstimationAlgorithmSelectionModeChange(int state)
{
	bool singleAlgorithmMode = (state == Qt::Checked);

	ui->sensorsEstimationAlgorithmsTableWidget->setVisible(!singleAlgorithmMode);
	ui->singleEstimationWidget->setVisible(singleAlgorithmMode);
}

void IMUCostumeConfigurationWizard::pageChanged(int idx)
{	
	auto w = page(idx);

	if (w == ui->modelConfigurationPage)
	{
		verifyModel(-1);
	}
	else if (w == ui->sensorsMappingPage)
	{
		verifySensorsMapping(-1, -1);
	}
	else if (w == ui->sensorsEstimationPage)
	{
		verifyEstimationAlgorithms(-1, -1);
	}
	else if (w == ui->calibrationPage)
	{
		verifyCalibrationAlgorithm(-1);
	}
	else if (w == ui->motionEstimationPage)
	{
		verifyMotionEstimationAlgorithm(-1);
	}	
}

void IMUCostumeConfigurationWizard::verifyModel(int idx)
{
	bool enable = (ui->modelComboBox->currentIndex() > 0);

	if (enable == true){
		auto skeleton = ui->modelComboBox->currentData(Qt::UserRole).value<kinematic::SkeletonConstPtr>();
		jointsItemDelegate->setJoints(skeleton->root);
	}

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeConfigurationWizard::verifySensorsMapping(int row, int col)
{
	bool enable = true;

	auto model = ui->sensorsMappingTableWidget->model();

	for (unsigned int i = 0; i < model->rowCount(); ++i)
	{
		auto data = model->data(model->index(i, 1), Qt::UserRole);
		if (data.canConvert<kinematic::JointConstPtr>() == true){
			auto j = data.value<kinematic::JointConstPtr>();
			if (j == nullptr){
				enable = false;
				break;
			}
		}
		else{
			enable = false;
			break;
		}
	}

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeConfigurationWizard::verifyEstimationAlgorithms(int row, int col)
{
	bool enable = true;

	auto model = ui->sensorsEstimationAlgorithmsTableWidget->model();

	for (unsigned int i = 0; i < model->rowCount(); ++i)
	{
		auto data = model->data(model->index(i, 1), Qt::UserRole);
		if (data.canConvert<IMU::IIMUOrientationEstimationAlgorithmConstPtr>() == true){
			auto e = data.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();
			if (e == nullptr){
				enable = false;
				break;
			}
		}
		else{
			enable = false;
			break;
		}
	}

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeConfigurationWizard::verifyCalibrationAlgorithm(int idx)
{
	bool enable = (ui->calibrationAlgorithmComboBox->currentIndex() > 0);

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeConfigurationWizard::verifyMotionEstimationAlgorithm(int idx)
{
	bool enable = (ui->motionEstimationAglorithmComboBox->currentIndex() > 0);

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeConfigurationWizard::onOneToOneMappingChanged(int state)
{
	if (state == Qt::Checked){
		auto model = ui->sensorsMappingTableWidget->model();
		std::set<std::string> jointsNames;
		for (unsigned int i = 0; i < model->rowCount(); ++i){
			auto jointName = model->data(model->index(i, 1)).toString().toStdString();
			if (jointsNames.find(jointName) != jointsNames.end()){
				//ju¿ mamy taki joint
				model->setData(model->index(i, 1), tr("Select joint..."));
				model->setData(model->index(i, 1), QVariant::fromValue(kinematic::JointConstPtr()), Qt::UserRole);
			}
			else{
				//jointa nie by³o - zapamiêtujemy
				jointsNames.insert(jointName);
			}
		}
	}	
}

IMUCostumeConfigurationWizard::SensorsMappingDetails IMUCostumeConfigurationWizard::mappingDetails() const
{
	SensorsMappingDetails ret;

	auto modelMapping = ui->sensorsMappingTableWidget->model();
	auto modelEstimating = ui->sensorsEstimationAlgorithmsTableWidget->model();

	UTILS_ASSERT(modelMapping->rowCount() == modelEstimating->rowCount());

	for (unsigned int i = 0; i < modelMapping->rowCount(); ++i)
	{		
		imuCostume::Costume::SensorID sID = modelMapping->data(modelMapping->index(i, 0)).toInt();
		SensorMappingDetails smd;
		smd.jointName = modelMapping->data(modelMapping->index(i, 1)).toString().toStdString();
		smd.sensorAdjustment.offset = modelMapping->data(modelMapping->index(i, 2), Qt::UserRole).value<osg::Vec3d>();
		smd.sensorAdjustment.rotation = modelMapping->data(modelMapping->index(i, 3), Qt::UserRole).value<osg::Quat>();
		smd.estimationAlgorithm = modelEstimating->data(modelEstimating->index(i, 1), Qt::UserRole).value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();

		ret.insert(SensorsMappingDetails::value_type(sID, smd));
	}

	return std::move(ret);
}

IMU::IMUCostumeCalibrationAlgorithmConstPtr IMUCostumeConfigurationWizard::calibrationAlgorithm() const
{
	return ui->calibrationAlgorithmComboBox->currentData(Qt::UserRole).value<IMU::IMUCostumeCalibrationAlgorithmConstPtr>();
}

IMU::IMUCostumeMotionEstimationAlgorithmConstPtr IMUCostumeConfigurationWizard::motionEstimationAlgorithm() const
{
	return ui->motionEstimationAglorithmComboBox->currentData(Qt::UserRole).value<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>();
}

kinematic::SkeletonConstPtr IMUCostumeConfigurationWizard::skeleton() const
{
	return ui->modelComboBox->currentData(Qt::UserRole).value<kinematic::SkeletonConstPtr>();
}