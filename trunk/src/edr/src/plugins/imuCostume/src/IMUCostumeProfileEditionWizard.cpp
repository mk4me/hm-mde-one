#include "IMUCostumeProfileEditionWizard.h"
#include "ui_IMUCostumeProfileEditionWizard.h"
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
#include <boost/lexical_cast.hpp>

Q_DECLARE_METATYPE(IMU::IIMUOrientationEstimationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeCalibrationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeMotionEstimationAlgorithmConstPtr);

Q_DECLARE_METATYPE(kinematic::JointConstPtr);

Q_DECLARE_METATYPE(osg::Vec3d);

Q_DECLARE_METATYPE(IMU::SkeletonConstPtr);

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
					editor->setMaximum(joint->children.front()->value.position.length());					
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
					auto direction = joint->children.front()->value.position;				
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
					osg::Vec3d direction = joint->children.front()->value.position;
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

class IMUCostumeProfileEditionWizard::JointsItemDelegate : public QStyledItemDelegate
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
		map.insert(std::map<std::string, kinematic::JointConstPtr>::value_type(joint->value.name, joint));
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
			unsigned int counter = 1;
			for (auto a : algos)
			{				
				editor->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
				editor->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
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

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(
	const imuCostume::Costume::SensorIDsSet & sensorsIDs,
	const QString & profileName, QWidget * parent)
	: QWizard(parent), ui(new Ui::IMUCostumeProfileEditionWizard), jointsItemDelegate(nullptr)
{
	ui->setupUi(this);
	setWindowTitle(windowTitle().arg(profileName));

	ui->calibrationAlgorithmComboBox->installEventFilter(this);
	ui->motionEstimationAglorithmComboBox->installEventFilter(this);
	ui->singleEstimationAlgorithmComboBox->installEventFilter(this);

	ui->sensorsMappingTableWidget->setRowCount(sensorsIDs.size());
	ui->sensorsEstimationAlgorithmsTableWidget->setRowCount(sensorsIDs.size());

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
			auto visitor = [&jointsCount](kinematic::JointConstPtr joint) -> void
			{
				if (joint->children.empty() == false){
					++jointsCount;
				}
			};
			utils::TreeNode::visitPostOrder(m.second->root, visitor);

			ui->modelComboBox->addItem(tr("Model %1: %2 joints").arg(QString::fromStdString(m.second->name)).arg(jointsCount), QVariant::fromValue(m.second));
		}
	}

	unsigned int i = 0;
	for (const auto & id : sensorsIDs)
	//for (unsigned int i = 0; i < sensorsIDs.size(); ++i)
	{		
		ui->sensorsMappingTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(id)));		
		ui->sensorsMappingTableWidget->item(i, 0)->setFlags(ui->sensorsMappingTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 0), QVariant(id));
		ui->sensorsMappingTableWidget->setItem(i, 1, new QTableWidgetItem(tr("Select joint...")));
		ui->sensorsMappingTableWidget->setItem(i, 2, new QTableWidgetItem(QString("(0, 0, 0)")));
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 2), QVariant::fromValue(osg::Vec3d(0,0,0)), Qt::UserRole);
		ui->sensorsMappingTableWidget->setItem(i, 3, new QTableWidgetItem(QString("[(0, 0, 0), 1]")));
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 3), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);

		ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(id)));		
		ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->setFlags(ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);
		ui->sensorsEstimationAlgorithmsTableWidget->model()->setData(ui->sensorsEstimationAlgorithmsTableWidget->model()->index(i, 0), QVariant(id));
		ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 1, new QTableWidgetItem(tr("Select algorithm...")));
		++i;
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

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(
	const IMU::CostumeProfile & costumeProfile,
	QWidget * parent)
	: QWizard(parent), ui(new Ui::IMUCostumeProfileEditionWizard), jointsItemDelegate(nullptr),
	baseCostumeProfile_(costumeProfile)
{	
	//TODO
	ui->setupUi(this);
	setWindowTitle(windowTitle().arg(QString::fromStdString(costumeProfile.name)));

	ui->calibrationAlgorithmComboBox->installEventFilter(this);
	ui->motionEstimationAglorithmComboBox->installEventFilter(this);
	ui->singleEstimationAlgorithmComboBox->installEventFilter(this);

	ui->sensorsMappingTableWidget->setRowCount(baseCostumeProfile_.sensorsMapping.size());
	ui->sensorsEstimationAlgorithmsTableWidget->setRowCount(baseCostumeProfile_.sensorsMapping.size());

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
			auto visitor = [&jointsCount](kinematic::JointConstPtr joint) -> void
			{
				if (joint->children.empty() == false){
					++jointsCount;
				}
			};
			utils::TreeNode::visitPostOrder(m.second->root, visitor);

			ui->modelComboBox->addItem(tr("Model %1: %2 joints").arg(QString::fromStdString(m.second->name)).arg(jointsCount), QVariant::fromValue(m.second));

			if (m.second->ID() == baseCostumeProfile_.skeleton->ID()){
				ui->modelComboBox->blockSignals(true);
				ui->modelComboBox->setCurrentIndex(ui->modelComboBox->count() - 1);
				ui->modelComboBox->blockSignals(false);
			}
		}	
	}	

	bool same = true;

	core::UniqueID id = boost::uuids::nil_uuid();

	unsigned int i = 0;
	for (const auto & sm : baseCostumeProfile_.sensorsMapping)		
	{
		ui->sensorsMappingTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(sm.get_left())));
		ui->sensorsMappingTableWidget->item(i, 0)->setFlags(ui->sensorsMappingTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);
		ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 0), QVariant(sm.get_left()));		

		ui->sensorsMappingTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(sm.get_right())));

		auto mIT = baseCostumeProfile_.sensorsAdjustments.find(sm.get_left());

		if (mIT != baseCostumeProfile_.sensorsAdjustments.end()){
			ui->sensorsMappingTableWidget->setItem(i, 2, new QTableWidgetItem(QString("(%1, %2, %3)").arg(mIT->second.offset[0]).arg(mIT->second.offset[1]).arg(mIT->second.offset[2])));
			ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 2), QVariant::fromValue(mIT->second.offset), Qt::UserRole);
			ui->sensorsMappingTableWidget->setItem(i, 3, new QTableWidgetItem(QString("[(%1, %2, %3), %4]").arg(mIT->second.rotation[0]).arg(mIT->second.rotation[1]).arg(mIT->second.rotation[2]).arg(mIT->second.rotation[3])));
			ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 3), QVariant::fromValue(mIT->second.rotation), Qt::UserRole);
		}
		else{
			ui->sensorsMappingTableWidget->setItem(i, 2, new QTableWidgetItem(QString("(0, 0, 0)")));
			ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 2), QVariant::fromValue(osg::Vec3d(0, 0, 0)), Qt::UserRole);
			ui->sensorsMappingTableWidget->setItem(i, 3, new QTableWidgetItem(QString("[(0, 0, 0), 1]")));
			ui->sensorsMappingTableWidget->model()->setData(ui->sensorsMappingTableWidget->model()->index(i, 3), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);
		}

		ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(sm.get_left())));
		ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->setFlags(ui->sensorsEstimationAlgorithmsTableWidget->item(i, 0)->flags() & ~Qt::ItemIsEditable);

		auto aIT = baseCostumeProfile_.sensorsOrientationEstimationAlgorithms.find(sm.get_left());

		if (aIT != baseCostumeProfile_.sensorsOrientationEstimationAlgorithms.end()){
			ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(aIT->second->name())));
			ui->sensorsEstimationAlgorithmsTableWidget->item(i, 1)->setData(Qt::UserRole, QVariant::fromValue(aIT->second));

			if (id.is_nil() == true){
				id = aIT->second->ID();
			} else if(same == true){
				same = (id == aIT->second->ID());
			}
		}
		else{
			ui->sensorsEstimationAlgorithmsTableWidget->setItem(i, 1, new QTableWidgetItem(tr("Select algorithm...")));
			same = false;
		}

		++i;
	}

	jointsItemDelegate = new JointsItemDelegate(ui->oneToOneMappingCheckBox, ui->sensorsMappingTableWidget);
	ui->sensorsMappingTableWidget->setItemDelegateForColumn(1, jointsItemDelegate);
	//delegat dla offsetu czujnika	
	ui->sensorsMappingTableWidget->setItemDelegateForColumn(2, new SensorJointOffsetDelegate);
	//delegat dla orientacji	
	ui->sensorsMappingTableWidget->setItemDelegateForColumn(3, new OrientationDelegate);

	ui->sensorsEstimationAlgorithmsTableWidget->setItemDelegateForColumn(1, new OrientationEstimationAlgorithmsDelegate(ui->sensorsEstimationAlgorithmsTableWidget));

	ui->sensorsEstimationAlgorithmsTableWidget->setVisible(false);

	if (baseCostumeProfile_.calibrationAlgorithm != nullptr){

		ui->calibrationAlgorithmComboBox->blockSignals(true);
		ui->calibrationAlgorithmComboBox->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->calibrationAlgorithmComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->calibrationAlgorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

		ui->calibrationAlgorithmComboBox->insertSeparator(1);
		ui->calibrationAlgorithmComboBox->addItem(QString::fromStdString(baseCostumeProfile_.calibrationAlgorithm->name()), QVariant::fromValue(baseCostumeProfile_.calibrationAlgorithm));
		ui->calibrationAlgorithmComboBox->setItemData(2, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(baseCostumeProfile_.calibrationAlgorithm->ID()))), Qt::ToolTipRole);

		ui->calibrationAlgorithmComboBox->setCurrentText(QString::fromStdString(baseCostumeProfile_.calibrationAlgorithm->name()));

		ui->calibrationAlgorithmComboBox->blockSignals(false);
	}

	if (baseCostumeProfile_.motionEstimationAlgorithm != nullptr){
		ui->motionEstimationAglorithmComboBox->blockSignals(true);
		ui->motionEstimationAglorithmComboBox->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->motionEstimationAglorithmComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->motionEstimationAglorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

		ui->motionEstimationAglorithmComboBox->insertSeparator(1);
		ui->motionEstimationAglorithmComboBox->addItem(QString::fromStdString(baseCostumeProfile_.motionEstimationAlgorithm->name()), QVariant::fromValue(baseCostumeProfile_.motionEstimationAlgorithm));
		ui->motionEstimationAglorithmComboBox->setItemData(2, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(baseCostumeProfile_.motionEstimationAlgorithm->ID()))), Qt::ToolTipRole);

		ui->motionEstimationAglorithmComboBox->setCurrentText(QString::fromStdString(baseCostumeProfile_.motionEstimationAlgorithm->name()));

		ui->motionEstimationAglorithmComboBox->blockSignals(false);
	}

	if (same == true && id.is_nil() == false){
		ui->singleEstimationAlgorithmComboBox->blockSignals(true);
		ui->singleEstimationAlgorithmComboBox->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->singleEstimationAlgorithmComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->singleEstimationAlgorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

		ui->singleEstimationAlgorithmComboBox->insertSeparator(1);
		ui->singleEstimationAlgorithmComboBox->addItem(QString::fromStdString(baseCostumeProfile_.sensorsOrientationEstimationAlgorithms.begin()->second->name()), QVariant::fromValue(baseCostumeProfile_.sensorsOrientationEstimationAlgorithms.begin()->second));
		ui->singleEstimationAlgorithmComboBox->setItemData(2, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(id))), Qt::ToolTipRole);

		ui->singleEstimationAlgorithmComboBox->setCurrentText(QString::fromStdString(baseCostumeProfile_.sensorsOrientationEstimationAlgorithms.begin()->second->name()));

		ui->singleEstimationAlgorithmComboBox->blockSignals(false);
	}
}

IMUCostumeProfileEditionWizard::~IMUCostumeProfileEditionWizard()
{

}

bool IMUCostumeProfileEditionWizard::eventFilter(QObject * watched, QEvent * event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		if (watched == ui->calibrationAlgorithmComboBox){

			auto val = ui->calibrationAlgorithmComboBox->currentText();

			ui->calibrationAlgorithmComboBox->blockSignals(true);
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

				unsigned int counter = 1;

				for (auto a : algos)
				{					
					ui->calibrationAlgorithmComboBox->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
					ui->calibrationAlgorithmComboBox->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
				}
			}

			ui->calibrationAlgorithmComboBox->setCurrentText(val);
			ui->calibrationAlgorithmComboBox->blockSignals(false);
		}
		else if (watched == ui->motionEstimationAglorithmComboBox)
		{
			auto val = ui->motionEstimationAglorithmComboBox->currentText();

			ui->motionEstimationAglorithmComboBox->blockSignals(true);
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

				if (algos.empty() == false){
					ui->motionEstimationAglorithmComboBox->insertSeparator(1);
				}

				unsigned int counter = 1;

				for (auto a : algos)
				{					
					ui->motionEstimationAglorithmComboBox->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
					ui->motionEstimationAglorithmComboBox->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
				}
			}

			ui->motionEstimationAglorithmComboBox->setCurrentText(val);
			ui->motionEstimationAglorithmComboBox->blockSignals(false);
		}
		else if (watched == ui->singleEstimationAlgorithmComboBox)
		{
			//musze sprawdzi� czy to co jest wybrane jest te� ustawione we wszystkich pozycjach tabeli,
			//je�li tak to ok, mo�e zosta� jako domy�lna warto��, w przeciwnym wypadku musimy wybiera�
			auto val = ui->singleEstimationAlgorithmComboBox->currentText();

			auto m = ui->sensorsEstimationAlgorithmsTableWidget->model();

			for (unsigned int i = 0; i < m->rowCount(); ++i)
			{
				if (val != m->data(m->index(i, 1)).toString()){
					val = QString();
					break;
				}
			}

			ui->singleEstimationAlgorithmComboBox->blockSignals(true);
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

				if (algos.empty() == false){
					ui->motionEstimationAglorithmComboBox->insertSeparator(1);
				}

				unsigned int counter = 1;

				for (auto a : algos)
				{					
					ui->singleEstimationAlgorithmComboBox->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
					ui->motionEstimationAglorithmComboBox->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
				}
			}

			ui->singleEstimationAlgorithmComboBox->setCurrentText(val);
			ui->singleEstimationAlgorithmComboBox->blockSignals(false);
		}
	}

	return false;
}

void IMUCostumeProfileEditionWizard::onEstimationAlgorithmSelectionModeChange(int state)
{
	bool singleAlgorithmMode = (state == Qt::Checked);

	ui->sensorsEstimationAlgorithmsTableWidget->setVisible(!singleAlgorithmMode);
	ui->singleEstimationWidget->setVisible(singleAlgorithmMode);
}

void IMUCostumeProfileEditionWizard::pageChanged(int idx)
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

void IMUCostumeProfileEditionWizard::verifyModel(int idx)
{
	bool enable = (ui->modelComboBox->currentIndex() > 0);

	if (enable == true){
		auto skeleton = ui->modelComboBox->currentData(Qt::UserRole).value<IMU::SkeletonConstPtr>();
		jointsItemDelegate->setJoints(skeleton->root);
	}

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeProfileEditionWizard::verifySensorsMapping(int row, int col)
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

void IMUCostumeProfileEditionWizard::verifyEstimationAlgorithms(int row, int col)
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

void IMUCostumeProfileEditionWizard::verifyCalibrationAlgorithm(int idx)
{
	bool enable = (ui->calibrationAlgorithmComboBox->currentIndex() > 0);

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeProfileEditionWizard::verifyMotionEstimationAlgorithm(int idx)
{
	bool enable = (ui->motionEstimationAglorithmComboBox->currentIndex() > 0);

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeProfileEditionWizard::onOneToOneMappingChanged(int state)
{
	if (state == Qt::Checked){
		auto model = ui->sensorsMappingTableWidget->model();
		std::set<std::string> jointsNames;
		for (unsigned int i = 0; i < model->rowCount(); ++i){
			auto jointName = model->data(model->index(i, 1)).toString().toStdString();
			if (jointsNames.find(jointName) != jointsNames.end()){
				//ju� mamy taki joint
				model->setData(model->index(i, 1), tr("Select joint..."));
				model->setData(model->index(i, 1), QVariant::fromValue(kinematic::JointConstPtr()), Qt::UserRole);
			}
			else{
				//jointa nie by�o - zapami�tujemy
				jointsNames.insert(jointName);
			}
		}
	}	
}

void IMUCostumeProfileEditionWizard::onSingleEstimationAlgorithmChange(int idx)
{
	auto data = ui->singleEstimationAlgorithmComboBox->currentData(Qt::UserRole).value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();

	if (data != nullptr){

		auto m = ui->sensorsEstimationAlgorithmsTableWidget->model();
		for (unsigned int i = 0; i < m->rowCount(); ++i)
		{
			auto idx = m->index(i, 1);
			m->setData(idx, QString::fromStdString(data->name()));
			m->setData(idx, QVariant::fromValue(data), Qt::UserRole);
		}
	}
}

void IMUCostumeProfileEditionWizard::refreshProfile()
{
	/*
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

	ui->calibrationAlgorithmComboBox->currentData(Qt::UserRole).value<IMU::IMUCostumeCalibrationAlgorithmConstPtr>();
	ui->motionEstimationAglorithmComboBox->currentData(Qt::UserRole).value<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>();
	ui->modelComboBox->currentData(Qt::UserRole).value<kinematic::SkeletonConstPtr>();
	*/
	setWindowModified(true);
}

CostumeProfile IMUCostumeProfileEditionWizard::costumeProfile() const
{
	CostumeProfile ret;
	ret.name = "New profile";
	ret.calibrationAlgorithm = ui->calibrationAlgorithmComboBox->currentData().value<IMUCostumeCalibrationAlgorithmConstPtr>();
	ret.motionEstimationAlgorithm = ui->motionEstimationAglorithmComboBox->currentData().value<IMUCostumeMotionEstimationAlgorithmConstPtr>();
	ret.skeleton = ui->modelComboBox->currentData().value<SkeletonConstPtr>();

	auto smm = ui->sensorsMappingTableWidget->model();
	for (unsigned int i = 0; i < smm->rowCount(); ++i)
	{
		auto sensorID = smm->data(smm->index(i, 0)).toUInt();
		auto jointName = smm->data(smm->index(i, 1)).toString().toStdString();
		IMUCostumeCalibrationAlgorithm::SensorAdjustment sa;
		sa.offset = smm->data(smm->index(i, 2), Qt::UserRole).value<osg::Vec3d>();
		sa.rotation = smm->data(smm->index(i, 3), Qt::UserRole).value<osg::Quat>();

		ret.sensorsMapping.insert(SensorsMapping::value_type(sensorID, jointName));
		ret.sensorsAdjustments.insert(IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts::value_type(sensorID, sa));
	}

	auto seam = ui->sensorsEstimationAlgorithmsTableWidget->model();
	for (unsigned int i = 0; i < seam->rowCount(); ++i)
	{
		auto sensorID = seam->data(seam->index(i, 0)).toUInt();
		auto algo = seam->data(seam->index(i, 1), Qt::UserRole).value<IIMUOrientationEstimationAlgorithmConstPtr>();
		ret.sensorsOrientationEstimationAlgorithms.insert(IIMUDataSource::OrientationEstimationPrototypeAlgorithmsMapping::value_type(sensorID, algo));
	}

	return ret;
}
