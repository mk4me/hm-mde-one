#include "PCH.h"
#include "IMUCostumeListWidget.h"
#include "ui_IMUCostumeListWidget.h"
#include "DataSource.h"
#include <QtWidgets/QMenu>
#include <coreui/CoreCursorChanger.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <corelib/Filesystem.h>
#include "IMUCostumeCalibrationWizard.h"
#include "corelib/IVisualizerManager.h"
#include "corelib/PluginCommon.h"
#include "utils/ObjectWrapper.h"
#include "osg/PositionAttitudeTransform"
#include "osg/MatrixTransform"
#include "osg/Geode"
#include "osg/ShapeDrawable"
#include "osg/PolygonMode"
#include "osg/ref_ptr"
#include "osgDB/ReadFile"
#include "osg/LineWidth"
#include "osg/StateAttribute"
#include "osg/StateSet"
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CANopenSensor.h>
#include <imucostumelib/ImuSensor.h>
#include <thread>

IMUCostumeWidget::IMUCostumeWidget(IMU::IMUCostumeDataSource * ds,
	QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui_IMUCostumeListWidget), ds(ds)
{
	ui->setupUi(this);

	ui->connectPushButton->setChecked(ds->connected());
	ui->connectPushButton->setText(ds->connected() == true ? tr("Disconnect") : tr("Connect"));

#ifndef _DEBUG
	ui->loadDatFileButton->setVisible(false);
#endif

	const auto cid = core::UID::GenerateUniqueID("{E8B5DEB2-5C57-4323-937D-1FFD288B65B9}");
	auto proto = plugin::getVisualizerManager()->getVisualizerPrototype(cid);
	QHBoxLayout* bl = new QHBoxLayout();
	static core::Visualizer* vis = proto->create();
	core::VariantPtr wrapper = createFbxWrapper();

	
	bl->addWidget(vis->getOrCreateWidget());
	ui->visualizerPlaceholder->setLayout(bl);
	auto *serie = vis->createSerie(typeid(osg::PositionAttitudeTransform), wrapper);
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	delete ui;
}

void IMUCostumeWidget::onCalibrate()
{
	coreUI::CoreCursorChanger cc;

	auto ci = ui->costumesTreeWidget->currentItem();

	if(ci != nullptr){

		IMUCostumeCalibrationWizard wizard(this);
		auto ret = wizard.exec();
		if (ret == QDialog::Accepted){
			QMessageBox::information(this, tr("Costume connection problem"), tr("Could not connect costume - please retry and follow costume initialization instructions."));
		}

		return;

		unsigned int idx = ci->data(0, Qt::UserRole).toUInt();

		if(ds->isCalibrated(idx) == true &&
			QMessageBox::information(this, tr("Calibration status"), tr("Selected Costume already calibrated. Do You want to re-calibrate it?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No){

			return;
		}
			

		bool retry = false;

		do {
			retry = true;

			QMessageBox::information(this, tr("Calibration - first step"), tr("Please stand still with Your hands along the body and press OK"), QMessageBox::Ok);

			ds->callibrateFirstPass(idx);

			unsigned int i = 2000;

			while(--i > 0){				
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				QCoreApplication::processEvents();
			}

			QApplication::beep();

			QMessageBox::information(this, tr("Calibration - second step"), tr("Please stand in 'T' pose and press OK"), QMessageBox::Ok);

			ds->callibrateSecondPass(idx);

			i = 2000;

			while(--i > 0){				
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				QCoreApplication::processEvents();
			}

			ds->finalizeCalibration(idx);

			if(ds->isCalibrated(idx) == false){

				if(QMessageBox::question(this, tr("Costume calibration failed"), tr("Costume calibration failed. Would You like to retry calibration?"),
					QMessageBox::Yes, QMessageBox::No) == QMessageBox::No){

					retry = false;
				}else{

					QMessageBox::information(this, tr("Calibration - summary"), tr("Calibration failed for the given costume."), QMessageBox::Ok);

				}

			}else{

				QMessageBox::information(this, tr("Calibration - summary"), tr("Calibration successful for the given costume."), QMessageBox::Ok);

				retry = false;
			}

		}while(retry == true);

	}else{
		QMessageBox::warning(this, tr("Warning - no costume"), tr("No costume selected for configuration. Chose costume from the list and retry"), QMessageBox::Ok, QMessageBox::NoButton);
	}
}

void IMUCostumeWidget::onConnect()
{
	coreUI::CoreCursorChanger cc;

	ui->costumesTreeWidget->clear();

	bool connected = ds->connected();

	if(connected == true){
		ds->disconnectCostiumes();
	}else{
		ds->connectCostiumes();
	}

	if(connected == false && connected == ds->connected()){

		QMessageBox::information(this, tr("Costume connection problem"), tr("Could not connect costume - please retry and follow costume initialization instructions."));
		return;
	}

	ui->connectPushButton->setText(ds->connected() == true ? tr("Disconnect") : tr("Connect"));

	//teraz wype³niam kostiumami je¿eli s¹
	auto s = ds->costumesCout();

	if(s > 0){
		for(unsigned int i = 0; i < s; ++i){

			QTreeWidgetItem * lwi = new QTreeWidgetItem;
			lwi->setText(1, QString::fromStdString(ds->costumeConfiguration(i).name));
			lwi->setData(0, Qt::UserRole, i);

			ui->costumesTreeWidget->addTopLevelItem(lwi);
		}
	}	
}

void IMUCostumeWidget::onCostumesListContextMenu(const QPoint & position)
{
	QMenu * menu = new QMenu(this);

	const auto cc = ds->costumesCout();

	if(cc > 0){

		auto loadAll = menu->addAction(tr("Load all"));
		auto unloadAll = menu->addAction(tr("Unload all"));	

		if(cc != ds->costumesLoadedCount()){
			loadAll->setEnabled(true);
			connect(loadAll, SIGNAL(triggered()), this, SLOT(onLoadAll()));
		}else{
			loadAll->setEnabled(false);
		}

		if(ds->costumesLoadedCount() > 0){
			unloadAll->setEnabled(true);
			connect(unloadAll, SIGNAL(triggered()), this, SLOT(onUnloadAll()));
		}else{
			unloadAll->setEnabled(false);
		}

		if(ui->costumesTreeWidget->currentItem() != nullptr){

			const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

			menu->addSeparator();

			auto load = menu->addAction(tr("Load"));
			auto unload = menu->addAction(tr("Unload"));
			auto cl = ds->costumeLoaded(idx);

			load->setEnabled(cl == false);
			unload->setEnabled(cl == true);

			connect(load, SIGNAL(triggered()), this, SLOT(onLoad()));
			connect(unload, SIGNAL(triggered()), this, SLOT(onUnload()));
		}
	}else{
		menu->addAction("--- " + tr("No options") + " ---");
	}
			
	menu->exec(mapToGlobal(position));
}

void IMUCostumeWidget::onLoad()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

	ds->loadCostume(idx);
}

void IMUCostumeWidget::onUnload()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

	ds->unloadCostume(idx);
}

void IMUCostumeWidget::onLoadAll()
{
	ds->loadAllCostumes();
}

void IMUCostumeWidget::onUnloadAll()
{
	ds->unloadAllCostumes();
}

void IMUCostumeWidget::onLoadDatFile()
{
    //ds->testMethod();
    //return;
    QString file = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Imu file (*.dat)"));
    core::Filesystem::Path p(file.toStdString());
    if (core::Filesystem::isRegularFile(p)) {
        //ds->loadDatFile(p);
    }
    
}

osg::Geometry* createLine(osg::Vec3 a, osg::Vec3 b, osg::Vec4 color){

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(a);
	vertices->push_back(b);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color);

	osg::Geometry* lineGeometry = new osg::Geometry;
	lineGeometry->setVertexArray(vertices.get());
	lineGeometry->setColorArray(colors);

	lineGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	lineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
	return lineGeometry;
}

osg::Geometry* createCoordinate(osg::Vec3 center, int size){

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(center);
	vertices->push_back(center + osg::Vec3(size, 0, 0));
	vertices->push_back(center + osg::Vec3(0, size, 0));
	vertices->push_back(center + osg::Vec3(0, 0, size));
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
	color->push_back(osg::Vec4(1, 0, 0, 1));
	color->push_back(osg::Vec4(0, 1, 0, 1));
	color->push_back(osg::Vec4(0, 0, 1, 1));
	osg::Geometry* lineGeometry = new osg::Geometry;
	lineGeometry->setVertexArray(vertices.get());
	lineGeometry->setColorArray(color);

	lineGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

	lineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 4));
	return lineGeometry;
}

double scaleFactor = 1;
void updateStickmanScheleton(osg::Group* ref, kinematic::JointConstPtr joint, osg::Vec3 currentPos, osg::Geode* parentNode) {
	//getting joint translation:
	osg::MatrixTransform* m = new osg::MatrixTransform;

	osg::Geode* jointNode = new osg::Geode;
	osg::Vec3 jointTrans = joint->direction*joint->length*scaleFactor;
	osg::Vec3 jointPos = currentPos + jointTrans;

	//joint
	osg::ShapeDrawable* jointShape = new osg::ShapeDrawable;
	//jointShape->setShape(new osg::Sphere(osg::Vec3(0,0,0), 4));
	jointShape->setShape(new osg::Box(osg::Vec3(0, 0, 0), 2));
	jointShape->setColor(osg::Vec4(0, 0.2, 1, 0.8));

	//bone
	osg::Geometry* bone = createLine(osg::Vec3(0, 0, 0), jointTrans, osg::Vec4(1, 1, 1, 1));
	parentNode->addDrawable(bone);

	//coordinate system:
	int size = 15;
	osg::Geometry* xAxis = createLine(osg::Vec3(0, 0, 0), osg::Vec3(size, 0, 0), osg::Vec4(1, 0, 0, 1));
	osg::Geometry* yAxis = createLine(osg::Vec3(0, 0, 0), osg::Vec3(0, size, 0), osg::Vec4(0, 1, 0, 1));
	osg::Geometry* zAxis = createLine(osg::Vec3(0, 0, 0), osg::Vec3(0, 0, size), osg::Vec4(0, 0, 1, 1));
	/*jointNode->addDrawable(xAxis);
	jointNode->addDrawable(yAxis);
	jointNode->addDrawable(zAxis);*/



	jointNode->addDrawable(jointShape);
	m->addChild(jointNode);
	m->setMatrix(osg::Matrix::translate(jointPos));
	m->setName(joint->name);
	ref->addChild(m);
	for (int i = 0; i < joint->children.size(); ++i)
		updateStickmanScheleton(ref, joint->children[i], jointPos, jointNode);
}
osg::PositionAttitudeTransform* createStickman(kinematic::SkeletalModelPtr modelBVH) {
	osg::Group* ref = new osg::Group;
	osg::PositionAttitudeTransform* sm = new osg::PositionAttitudeTransform;
	osg::Geode* root = new osg::Geode;
	osg::ShapeDrawable* rootShape = new osg::ShapeDrawable;
	rootShape->setShape(new osg::Sphere(osg::Vec3(0, 0, 0), 2));
	rootShape->setColor(osg::Vec4(0.2, 1, 0.2, 0.8));
	root->addDrawable(rootShape);
	osg::MatrixTransform* m = new osg::MatrixTransform;
	m->addChild(root);
	m->setName("root");
	m->setMatrix(osg::Matrix::translate(osg::Vec3(0, 0, 0)));
	ref->addChild(m);

	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(5.0f);
	osg::StateSet* stateSet = ref->getOrCreateStateSet();
	stateSet->setAttributeAndModes(linewidth,	osg::StateAttribute::ON);

	updateStickmanScheleton(ref, modelBVH->getJointByName("root"), osg::Vec3(0, 0, 0), root);
	sm->addChild(ref);
	return sm;
}


core::VariantPtr IMUCostumeWidget::createFbxWrapper()
{
	kinematic::SkeletalModelPtr modelBVH = utils::make_shared<kinematic::SkeletalModel>();
	kinematic::SkeletalDataPtr data = utils::make_shared<kinematic::SkeletalData>();
	kinematic::BvhParser parser;
	std::string bvhPath = (plugin::getPaths()->getResourcesPath() / "imu3d" / "imu_sk.bvh").string();
	
	parser.parse(modelBVH, data, bvhPath);

	osg::Vec3 modelPos(0, 0, -5);
	osg::PositionAttitudeTransform* BVHStickman = createStickman(modelBVH);

	size_t id = 0;
	size_t jointsCount = data->getFrames().operator[](0)->jointsData.size();
	std::string fbxPath = (plugin::getPaths()->getResourcesPath() / "imu3d" /"imu_mesh.FBX").string();
	osg::Node* model = osgDB::readNodeFile(fbxPath);

	//FbxStickSolver fbxSticlSolver(model);
	//FbxSolver4 fbxSolver(model);
	osg::PositionAttitudeTransform* paModel = new osg::PositionAttitudeTransform;

	osg::PolygonMode * polygonMode = new osg::PolygonMode;

	polygonMode->setMode(osg::PolygonMode::Face::FRONT_AND_BACK, osg::PolygonMode::Mode::LINE);
	osg::StateSet* stateSet = paModel->getOrCreateStateSet();
	stateSet->setAttributeAndModes(polygonMode, osg::StateAttribute::Values::PROTECTED | osg::StateAttribute::ON);
	paModel->setStateSet(stateSet);
	paModel->addChild(model);
	paModel->setPosition(osg::Vec3(0, 0, 0));
	BVHStickman->setPosition(osg::Vec3(0, 0, 35));
	paModel->setAttitude(osg::Quat(3.14 / 2, osg::Vec3(1, 0, 0), 0, osg::Vec3(0, 1, 0), 0, osg::Vec3(0, 0, 1)));
	BVHStickman->setAttitude(osg::Quat(3.14 / 2, osg::Vec3(1, 0, 0), 0, osg::Vec3(0, 1, 0), 0, osg::Vec3(0, 0, 1)));

	//auto bonem = createBoneMap(model);


	osg::Group* rootF = new osg::Group;

	auto fbxWrapper = core::Variant::create<osg::PositionAttitudeTransform>();
	osg::ref_ptr<osg::PositionAttitudeTransform> root = new osg::PositionAttitudeTransform;
	root->addChild(paModel);
	root->addChild(BVHStickman);
	root->setScale(osg::Vec3(0.03f, 0.03f, 0.03f));
	fbxWrapper->set(root);

	return fbxWrapper;

}

void IMUCostumeWidget::testCommunication()
{
	try{
		std::auto_ptr<imuCostume::CostumeRawIO> rawCostume(new imuCostume::CostumeRawIO("192.168.1.173"));
		std::auto_ptr<imuCostume::Costume> costume(new imuCostume::Costume(rawCostume.get()));
		auto data = costume->read(300);
		imuCostume::CANopenSensor canSensor(rawCostume.get(), 1, 300);
		imuCostume::CANopenSensor::ErrorCode error = imuCostume::CANopenSensor::NO_ERROR;
		auto ret1 = canSensor.writeSDO(imuCostume::IMUSensor::CONFIGURATION, imuCostume::CANopenSensor::Size1B, 1, error, imuCostume::IMUSensor::CONFIGURATION_SOFT_IRON_CAL_ENABLE);
		auto ret2 = canSensor.writeSDO(imuCostume::IMUSensor::CONFIGURATION, imuCostume::CANopenSensor::Size1B, 2, error, imuCostume::IMUSensor::CONFIGURATION_SOFT_IRON_CAL_ENABLE);
		auto ret3 = canSensor.saveConfiguration(error);
		bool wait = true;
		wait = false;
	}
	catch (std::exception & e){
		std::string errorcode = e.what();
		errorcode += "_";
	}
	catch (...){
		std::string errorcode = "unknown error";
		errorcode += "_";
	}
}
