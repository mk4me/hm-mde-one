#include "PCH.h"
#include "KinematicVisualizer.h"
#include "GRFSerie.h"
#include "SkeletonSerie.h"
#include "MarkerSerie.h"
#include "PickHandler.h"
#include "Manipulators.h"

#ifdef _DEBUG

#include <core/MultiViewWidgetItem.h>

#endif

const float DEFAULT_SHIFT = 1.0f;

void KinematicVisualizer::setUp( core::IObjectSource* source )
{
    reset();
}

void KinematicVisualizer::reset()
{
    resetScene();
    refillDrawersMaps();
}

core::IVisualizer* KinematicVisualizer::createClone() const
{
    return new KinematicVisualizer();
}

void KinematicVisualizer::getInputInfo( std::vector<core::IInputDescription::InputInfo>& info)
{
    core::IInputDescription::InputInfo input;

	input.name = "markers";
	input.type = typeid(MarkerCollection);
	input.required = false;
	input.modify = false;

	info.push_back(input);

	input.name = "grf";
	input.type = typeid(GRFCollection);
	input.required = false;
	input.modify = false;

    info.push_back(input);

	input.name = "skeleton";
	input.type = typeid(kinematic::JointAnglesCollection);
	input.required = false;
	input.modify = false;

	info.push_back(input);
}

int KinematicVisualizer::getMaxDataSeries() const
{
    return -1;
}

core::IVisualizer::TimeSerieBase *KinematicVisualizer::createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
{
	KinematicSerie * ret = nullptr;
	if (data->getTypeInfo() == typeid(GRFCollection)) {
		ret = new GRFSerie(this);
		ret->setName(name + "_grf");
		ret->setData(data);
	} else if (data->getTypeInfo() == typeid (MarkerCollection)) {
		ret = new MarkerSerie(this);
		ret->setName(name + "_markers");
		ret->setData(data);
	} else if (data->getTypeInfo() == typeid (kinematic::JointAnglesCollection)) {
		ret = new SkeletonSerie(this);
		ret->setName(name + "_skeleton");
		ret->setData(data);
	} else {
		UTILS_ASSERT(false);
	}

    transformNode->addChild(ret->getMatrixTransformNode());
    
    series.push_back(ret);
    if(series.size() == 1){
        activeSerieCombo->blockSignals(true);
        activeSerieCombo->clear();
        activeSerieCombo->addItem(name.c_str());
        activeSerieCombo->setCurrentIndex(0);
        activeSerieCombo->blockSignals(false);
        setActiveSerie(0);
    }else{
        activeSerieCombo->addItem(name.c_str());
    }

    activeSerieCombo->setEnabled(true);
    return ret;
}

core::IVisualizer::TimeSerieBase * KinematicVisualizer::createSerie(const core::IVisualizer::SerieBase* serie)
{
    return nullptr;
}

void KinematicVisualizer::removeSerie(core::IVisualizer::SerieBase *serie)
{
    reset();
}

QWidget* KinematicVisualizer::createWidget(core::IActionsGroupManager * manager)
{
    widget = new osgui::QOsgDefaultWidget();
    widget->setTimerActive(true);

    trajectoriesDialog = new TrajectoriesDialog(widget);
    schemeDialog = new SchemeDialog(widget);
    const osg::GraphicsContext::Traits* traits = widget->getCamera()->getGraphicsContext()->getTraits();

    widget->addEventHandler(new osgGA::StateSetManipulator(
        widget->getCamera()->getOrCreateStateSet()
        ));

    cameraManipulator = new osgGA::OrbitManipulator();
    widget->setCameraManipulator(cameraManipulator);
	setLeft();
    // stworzenie i dodanie œwiat³a przyczepionego do kamery
    osg::ref_ptr<osg::Light> light = new osg::Light;
    light->setLightNum(1);
    light->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
    light->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,0.0f));
    light->setConstantAttenuation(1.0f);

    widget->setLight(light);
    // tworzenie kamery
    widget->getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.1f, 1));

    QIcon icon0;
    icon0.addFile(QString::fromUtf8(":/resources/icons/switch-axisa.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    icon0.addFile(QString::fromUtf8(":/resources/icons/switch-axis.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    actionSwitchAxes = new QAction(tr("Switch Axes"), widget);
    actionSwitchAxes->setIcon(icon0);
    actionSwitchAxes->setCheckable(true);
    actionSwitchAxes->setChecked(false );
    actionSwitchAxes->setVisible(false);
    //connect(actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/tracea.png"));
	actionTrajectories = new QAction(tr("Trajectories"), widget);
    actionTrajectories->setIcon(icon1);
    actionTrajectories->setCheckable(true);
    connect(actionTrajectories, SIGNAL(triggered()), this, SLOT(showTrajectoriesDialog()));

    actionScheme = new QAction(tr("Scheme Drawers"), widget);
    actionScheme->setIcon(QIcon(QString::fromUtf8(":/resources/icons/trace-blue.png")));
    actionScheme->setCheckable(true);
    connect(actionScheme, SIGNAL(triggered()), this, SLOT(showSchemeDialog()));

    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/skeletal_tracea.png"));
    actionGhost = new QAction(tr("Ghost"), widget);
    actionGhost->setIcon(icon2);
    actionGhost->setCheckable(true);
    actionGhost->setVisible(false);
	
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/viewa.png"));
    QMenu* viewMenu = new QMenu(tr("View"), widget);
    viewMenu->setIcon(icon3);

    activeSerieCombo = new QComboBox();
    activeSerieCombo->addItem(tr("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    /*QAction* leftAction = new QAction("Shift left", widget);
    connect(leftAction, SIGNAL(triggered()), this, SLOT(shiftLeft()));
    QIcon iconLeft;
    iconLeft.addFile(QString::fromUtf8(":/resources/icons/left-a.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    iconLeft.addFile(QString::fromUtf8(":/resources/icons/left-b.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    leftAction->setIcon(iconLeft);*/


    /*QAction* rightAction = new QAction("Shift right", widget);
    connect(rightAction, SIGNAL(triggered()), this, SLOT(shiftRight()));
    QIcon iconRight;
    iconRight.addFile(QString::fromUtf8(":/resources/icons/right-a.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    iconRight.addFile(QString::fromUtf8(":/resources/icons/right-b.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    rightAction->setIcon(iconRight);*/
    //QWidget* spinWidgetX = new QWidget(widget);
    //QWidget* spinWidgetY = new QWidget(widget);
    //QWidget* spinWidgetZ = new QWidget(widget);
    //spinWidgetX->setLayout(new QHBoxLayout());
    //spinWidgetY->setLayout(new QHBoxLayout());
    //spinWidgetZ->setLayout(new QHBoxLayout());
    //spinWidgetX->layout()->addWidget(new QLabel("X:"));
    //spinWidgetY->layout()->addWidget(new QLabel("Y:"));
    //spinWidgetZ->layout()->addWidget(new QLabel("Z:"));
    //spinX = new QDoubleSpinBox(widget);
    //spinY = new QDoubleSpinBox(widget);
    //spinZ = new QDoubleSpinBox(widget);
    //connect(spinX, SIGNAL(valueChanged(double)), this, SLOT(shiftX(double)));
    //connect(spinY, SIGNAL(valueChanged(double)), this, SLOT(shiftY(double)));
    //connect(spinZ, SIGNAL(valueChanged(double)), this, SLOT(shiftZ(double)));
    //spinX->setMaximum(1000.0);
    //spinY->setMaximum(1000.0);
    //spinZ->setMaximum(1000.0);
    //spinX->setMinimum(-1000.0);
    //spinY->setMinimum(-1000.0);
    //spinZ->setMinimum(-1000.0);
    //spinX->setSingleStep(0.03);
    //spinY->setSingleStep(0.03);
    //spinZ->setSingleStep(0.03);
    //
    //spinWidgetX->layout()->addWidget(spinX);
    //spinWidgetY->layout()->addWidget(spinY);
    //spinWidgetZ->layout()->addWidget(spinZ);
    //
    //spinWidgetX->layout()->setMargin(0);
    //spinWidgetY->layout()->setMargin(0);
    //spinWidgetZ->layout()->setMargin(0);
    //
    //spinWidgetX->layout()->setContentsMargins(0, 0, 0, 0);
    //spinWidgetY->layout()->setContentsMargins(0, 0, 0, 0);
    //spinWidgetZ->layout()->setContentsMargins(0, 0, 0, 0);

    pickerAction = new QAction(widget);
    translateAction = new QAction(widget);
    rotateAction = new QAction(widget);
    scaleAction = new QAction(widget);

    pickerAction->setText(tr("Picker"));
    translateAction->setText(tr("Translate"));
    rotateAction->setText(tr("Rotate"));
    scaleAction->setText(tr("Scale"));

    pickerAction->setIcon(QIcon(":/resources/icons/manipulator-objects.png"));
    translateAction->setIcon(QIcon(":/resources/icons/manipulator-translate.png"));
    rotateAction->setIcon(QIcon(":/resources/icons/manipulator-rotate.png"));
    scaleAction->setIcon(QIcon(":/resources/icons/manipulator-scale.png"));

    connect(pickerAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
    connect(translateAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
    connect(rotateAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
    connect(scaleAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));

	QAction* lft_action = viewMenu->addAction(tr("Left")); 
	QAction* rht_action = viewMenu->addAction(tr("Right")); 
	QAction* frn_action = viewMenu->addAction(tr("Front")); 
	QAction* bck_action = viewMenu->addAction(tr("Back")); 
	QAction* top_action = viewMenu->addAction(tr("Top")); 
	QAction* btm_action = viewMenu->addAction(tr("Bottom")); 
	
	connect(lft_action, SIGNAL(triggered()), this, SLOT(setLeft()));
	connect(rht_action, SIGNAL(triggered()), this, SLOT(setRight())); 
	connect(frn_action, SIGNAL(triggered()), this, SLOT(setFront())); 
	connect(bck_action, SIGNAL(triggered()), this, SLOT(setBehind()));
	connect(top_action, SIGNAL(triggered()), this, SLOT(setTop()));
	connect(btm_action, SIGNAL(triggered()), this, SLOT(setBottom())); 	
	
    osg::Vec3 pos (0.0f, 9.0f, 3.0f);
    osg::Vec3 up(0,0,1);

    osg::Vec3 at;
    widget->getCamera()->setViewMatrixAsLookAt(pos, at, up);
    rootNode = new osg::Group;
    rootNode->setUpdateCallback(new KinematicNotifier(this));
    rootNode->addChild(createFloor());
    widget->setSceneData(rootNode);
    widget->setMinimumSize(50, 50);

    indicatorNode = createIndicator();


#ifndef _DEBUG

    //dodajemy dodatkowy zwyk³y widget
    QWidget * retWidget = new QWidget();
    retWidget->setLayout(new QVBoxLayout());
    retWidget->layout()->addWidget(widget);

#else
    //widget proxy przekierowujacy focusa do okienka osg
    QWidget * retWidget = new core::QOsgEncapsulatorWidget(widget);
    //dodajemy tez event handler ze statystykami
    widget->addEventHandler( new osgViewer::StatsHandler() );
    widget->setTimerActive(true);
#endif

    //auto info = [&](const PickHandler::PickerList& list) {
    //    for (auto it = list.begin(); it != list.end(); it++) {
    //        GeodePtr geode = it->get<0>();
    //        if (geode) {
    //            geode->setNodeMask(geode->getNodeMask() == 0 ? 0xFFFF : 0);
    //        }
    //    }
    //    //QString message;
    //    //for (auto it = list.begin(); it != list.end(); it++) {
    //    //    message += QString("Hit : %1, D: %2\n").arg(it->get<0>() ? it->get<0>()->getName().c_str() : "n/a").arg(it->get<1>() ? it->get<1>()->className() : "n/a");
    //    //}
    //    //QMessageBox box(QMessageBox::Warning, "HITS", message);
    //    //box.exec();
    //};
    auto info = [&](const PickHandler::PickerList& list) {
        auto it = list.begin();
        if (it != list.end()) {
            GeodePtr geode = it->get<0>();
            KinematicSerie* s = getParentSerie(geode);
            if (s) {
                setActiveSerie(s);
            }
        }
    };
    widget->addEventHandler(new PickHandler(info));

    core::IActionsGroupManager::GroupID id = manager->createGroup(tr("Properties"));
    manager->addGroupAction(id, activeSerieCombo);
    manager->addGroupAction(id, actionTrajectories);
    manager->addGroupAction(id, actionScheme);
    manager->addGroupAction(id, actionGhost);
    //manager->addGroupAction(id, spinWidgetX);
    //manager->addGroupAction(id, spinWidgetY);
    //manager->addGroupAction(id, spinWidgetZ);
    manager->addGroupAction(id, pickerAction);
    manager->addGroupAction(id, translateAction);
    manager->addGroupAction(id, rotateAction);
    manager->addGroupAction(id, scaleAction);

    id = manager->createGroup(tr("View"));
    manager->addGroupAction(id, viewMenu);
    //manager->addGroupAction(id, leftAction);
    //manager->addGroupAction(id, rightAction);
    manager->addGroupAction(id, actionSwitchAxes);
    
    retWidget->setFocusPolicy(Qt::StrongFocus);
    widget->setFocusPolicy(Qt::StrongFocus);
    retWidget->layout()->setContentsMargins(2,0,2,2);

    currentDragger = nullptr;
    translateDragger = Manipulators::creatreDraggerContainer(Manipulators::TranslateAxis);
    rotationDragger = Manipulators::creatreDraggerContainer(Manipulators::Trackball);
    scaleDragger = Manipulators::creatreDraggerContainer(Manipulators::TabBox);


    return retWidget;
}

const std::string& KinematicVisualizer::getName() const
{
    return name;
}

KinematicVisualizer::KinematicVisualizer() :
    name("Visualizer 3D"),
    trajectoriesDialog(nullptr),
    schemeDialog(nullptr),
    currentSerie(-1),
    spinX(nullptr),
    spinY(nullptr),
    spinZ(nullptr),
    currentDragger(nullptr),
    lastTime(-1.0f)
{

}

void KinematicVisualizer::update( double deltaTime )
{
    KinematicSerie* serie = tryGetCurrentSerie();
    if (serie && serie->getTime() != lastTime) {
        lastTime = serie->getTime();
        if (currentDragger && serie) {
            currentDragger->setDraggerPivot(serie->getPivot());
        }
    }
    
}

void KinematicVisualizer::updateAnimation()
{
    if (currentDrawer) {
        currentDrawer->update();
    }
}

osg::ref_ptr<osg::Group> KinematicVisualizer::createFloor()
{
	osg::ref_ptr<osg::Group> group = new osg::Group;
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.05f, 0.05f, 0.3f, 1.0f));

    GeodePtr geode = new osg::Geode();
    // todo sparametryzowac
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();

	float length = 3.5f;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(44);
    osg::Vec3 v;
    for (int i = 0; i < 11; i++) {
        v.set(-length + 2 * (i / 10.0f) * length, -length, 0);
        (*vertices)[2 * i] = v;      
        v.set(-length + 2 * (i / 10.0f) * length, length, 0);
        (*vertices)[2 * i + 1] = v;   
        v.set(-length, -length + 2 * (i / 10.0f) * length, 0);
        (*vertices)[22 + 2 * i] = v;   
        v.set(length, -length + 2 * (i / 10.0f) * length, 0);
        (*vertices)[22 + 2 * i + 1] = v;
    }

    linesGeom->setVertexArray(vertices);
    linesGeom->setDataVariance(osg::Object::DYNAMIC);
    geode->setStateSet(stateset);
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 44));

    geode->addDrawable(linesGeom);
	group->addChild(geode);

    return group;
}

QIcon* KinematicVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/resources/icons/3D.png"));
}

//void KinematicVisualizer::setAxis( bool xyz )
//{
//    /*if (xyz) {
//        osg::Quat q(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f));
//        transformNode->setAttitude(q);
//    } else {
//        osg::Quat q;
//        transformNode->setAttitude(q);
//    }*/
//}

void KinematicVisualizer::actionTriggered( QAction* action )
{
    resetScene();
    currentDrawer = drawersByAction[action];
    transformNode->addChild(currentDrawer->getNode());
}

void KinematicVisualizer::resetScene()
{
    if (rootNode) {
        for (int i = rootNode->getNumChildren() - 1; i >= 0; --i) {
            rootNode->removeChild(i);
        }
    }

    transformNode = new osg::PositionAttitudeTransform();
    rootNode->addChild(createFloor());
    rootNode->addChild(transformNode);
}

void KinematicVisualizer::addAction(const std::string& name, QMenu* menu, QActionGroup* group)
{
    QAction* act = menu->addAction(name.c_str());

    actionByName[name] = act;
    act->setCheckable(true);
    group->addAction(act); 
}

void KinematicVisualizer::refillDrawersMaps()
{
    drawersByAction.clear();
    drawersByName.clear();
    for (std::map<std::string, QAction*>::iterator it = actionByName.begin(); it != actionByName.end(); it++) {
        SchemeDrawerContainerPtr drawer(new SchemeDrawerContainer());
        drawersByAction[it->second] = drawer;
        drawersByName[it->first] = drawer;
    }
}

osg::Node* KinematicVisualizer::debugGetLocalSceneRoot()
{
    return rootNode;
}

void KinematicVisualizer::setRight()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(-90.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setLeft()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(90.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setFront()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setBehind()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(180.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setBottom()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(osg::DegreesToRadians(-90.0));
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setTop()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(osg::DegreesToRadians(90.0));
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::showTrajectoriesDialog()
{
    trajectoriesDialog->show();
    
}

void KinematicVisualizer::setActiveSerie( int idx )
{
    if (currentSerie >= 0) {
        series[currentSerie]->getMatrixTransformNode()->removeChild(indicatorNode);
    }
    currentSerie = idx;
    refreshSpinboxes();
    series[currentSerie]->getMatrixTransformNode()->addChild(indicatorNode);
}

void KinematicVisualizer::setActiveSerie( KinematicSerie* serie )
{
    for (int i = 0; i < series.size(); i++) {
        if (series[i] == serie) {
            setActiveSerie(i);
            return;
        }
    }
    UTILS_ASSERT(false);
}

KinematicSerie* KinematicVisualizer::tryGetCurrentSerie()
{
    if (currentSerie >= 0 && currentSerie < series.size()) {
        return series[currentSerie];
    }
    return nullptr;
}



void KinematicVisualizer::shiftLeft()
{
    //KinematicSerie* current = tryGetCurrentSerie();
    //if (current) {
    //    KinematicSerie::TransformPtr transform = current->getMatrixTransformNode();
    //    transform->setPosition(transform->getPosition() + osg::Vec3(DEFAULT_SHIFT, 0.0f, 0.0f));
    //}
}

void KinematicVisualizer::shiftRight()
{
    //KinematicSerie* current = tryGetCurrentSerie();
    //if (current) {
    //    KinematicSerie::TransformPtr transform = current->getTransformNode();
    //    transform->setPosition(transform->getPosition() + osg::Vec3(-DEFAULT_SHIFT, 0.0f, 0.0f));
    //}
}

osg::ref_ptr<osg::PositionAttitudeTransform> KinematicVisualizer::createIndicator() const
{
    using namespace osg;
    GeodePtr boxGeode = new osg::Geode();
    GeodePtr coneGeode = new osg::Geode();

    osg::ref_ptr<osg::Box> unitBox = new osg::Box(Vec3(0, 0, 0), 0.5f, 0.5f, 0.9f);
    osg::ref_ptr<osg::Cone> unitCone = new osg::Cone(Vec3(0, 0, 0), 1.0f, 0.1f);
    osg::ref_ptr<osg::ShapeDrawable> boxShape = new osg::ShapeDrawable(unitBox);
    osg::ref_ptr<osg::ShapeDrawable> coneShape = new osg::ShapeDrawable(unitCone);

    boxGeode->addDrawable(boxShape);
    coneGeode->addDrawable(coneShape);

    TransformPtr transformBox = new osg::PositionAttitudeTransform();
    TransformPtr transformCone = new osg::PositionAttitudeTransform();
    TransformPtr transform = new osg::PositionAttitudeTransform();

    transformBox->addChild(boxGeode);
    transformBox->setName("BOX");
    transformBox->setPosition(Vec3(0, 0, 0.45f));
    transformCone->addChild(coneGeode);
    transformCone->setName("CONE");
    transformCone->setPosition(Vec3(0, 0, 0.45f));

    transform->setScale(Vec3(0.03f, 0.03f, 0.5f));

    transform->addChild(transformBox);
    transformBox->addChild(transformCone);
    //transform->addChild(transformCone);
    osg::Matrix mat; 
    mat.makeRotate(osg::DegreesToRadians(90.0f), 1.0f, 0.0f, 0.0f);
    osg::Quat orientation;
    orientation.set(mat);
    transform->setAttitude(orientation);
    transform->setPosition(osg::Vec3(0.0f, 3.0f, 0.0f));
    return transform;
}

QPixmap KinematicVisualizer::print() const
{
    QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    QRect widgetRect = widget->geometry();
    widgetRect.moveTopLeft(widget->parentWidget()->mapToGlobal(widgetRect.topLeft()));
    return pixmap.copy(widgetRect);
}

void KinematicVisualizer::shiftX( double d )
{
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        //KinematicSerie::TransformPtr transform = current->getTransformNode();
        //osg::Vec3 pos = transform->getPosition();
        //pos._v[0] = d;
        //transform->setPosition(pos);
    }
}

void KinematicVisualizer::shiftY( double d )
{
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        //KinematicSerie::TransformPtr transform = current->getTransformNode();
        //osg::Vec3 pos = transform->getPosition();
        //pos._v[1] = d;
        //transform->setPosition(pos);
    }
}

void KinematicVisualizer::shiftZ( double d )
{
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        //KinematicSerie::TransformPtr transform = current->getTransformNode();
        //osg::Vec3 pos = transform->getPosition();
        //pos._v[2] = d;
        //transform->setPosition(pos);
    }
}

void KinematicVisualizer::showSchemeDialog()
{
    schemeDialog->show();
}

void KinematicVisualizer::refreshSpinboxes()
{
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        //KinematicSerie::TransformPtr transform = current->getMatrixTransformNode();
        //osg::Vec3 pos = transform->getPosition();
        //
        //spinX->blockSignals(true);
        //spinX->setValue(pos.x());
        //spinX->blockSignals(false);
        //
        //spinY->blockSignals(true);
        //spinY->setValue(pos.y());
        //spinY->blockSignals(false);
        //
        //spinZ->blockSignals(true);
        //spinZ->setValue(pos.z());
        //spinZ->blockSignals(false);
    }
    
}

void KinematicVisualizer::draggerTriggered()
{
    auto serie = tryGetCurrentSerie();
    if (serie) {
        QAction* a = qobject_cast<QAction*>(sender());
        if (currentDragger) {
            Manipulators::disconnect(transformNode, serie->getMatrixTransformNode(), currentDragger);
        }

        if (a == translateAction) {
            currentDragger = translateDragger;
        } else if (a == rotateAction) {
            currentDragger = rotationDragger;
        } else if (a == scaleAction) {
            currentDragger = scaleDragger;
        } else if (a == pickerAction) {
            currentDragger = nullptr;
        } else {
            UTILS_ASSERT(false);
        }
    
        if (currentDragger) {
            Manipulators::connect(transformNode, serie->getMatrixTransformNode(), currentDragger, serie->getPivot()); 
        }
    }
}

bool isChildRecursive(osg::Group* group, osg::Geode* geode)
{
    int count = group->getNumChildren();
    for (int i = 0; i < count; i++) {
        osg::Node* child = group->getChild(i);
        if (geode == child) {
            return true;
        }
        osg::Group* childGroup = dynamic_cast<osg::Group*>(child);
        if (childGroup && isChildRecursive(childGroup, geode)) {
            return true;
        }
    }

    return false;
}

KinematicSerie* KinematicVisualizer::getParentSerie( GeodePtr geode )
{
    for (auto it = series.begin(); it != series.end(); it++) {
        osg::MatrixTransform* node = (*it)->getMatrixTransformNode();
        if (isChildRecursive(node, geode)) {
            return *it;
        }
    }

    return nullptr;
}




