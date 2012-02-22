#include "PCH.h"
#include "KinematicVisualizer.h"
#include "GRFSerie.h"
#include "SkeletonSerie.h"
#include "MarkerSerie.h"

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

    transformNode->addChild(ret->getTransformNode());
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
    actionSwitchAxes = new QAction("Switch Axes", widget);
    actionSwitchAxes->setIcon(icon0);
    actionSwitchAxes->setCheckable(true);
    actionSwitchAxes->setChecked(false );
    actionSwitchAxes->setVisible(false);
    //connect(actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/tracea.png"));
	actionTrajectories = new QAction("Trajectories", widget);
    actionTrajectories->setIcon(icon1);
    actionTrajectories->setCheckable(true);
    connect(actionTrajectories, SIGNAL(triggered()), this, SLOT(showTrajectoriesDialog()));

    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/skeletal_tracea.png"));
    actionGhost = new QAction("Ghost", widget);
    actionGhost->setIcon(icon2);
    actionGhost->setCheckable(true);
    actionGhost->setVisible(false);
	
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/viewa.png"));
    QMenu* viewMenu = new QMenu("View", widget);
    viewMenu->setIcon(icon3);

    activeSerieCombo = new QComboBox();
    activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
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
    QWidget* spinWidgetX = new QWidget(widget);
    QWidget* spinWidgetY = new QWidget(widget);
    QWidget* spinWidgetZ = new QWidget(widget);
    spinWidgetX->setLayout(new QHBoxLayout());
    spinWidgetY->setLayout(new QHBoxLayout());
    spinWidgetZ->setLayout(new QHBoxLayout());
    spinWidgetX->layout()->addWidget(new QLabel("X:"));
    spinWidgetY->layout()->addWidget(new QLabel("Y:"));
    spinWidgetZ->layout()->addWidget(new QLabel("Z:"));
    QDoubleSpinBox* spinX = new QDoubleSpinBox(widget);
    QDoubleSpinBox* spinY = new QDoubleSpinBox(widget);
    QDoubleSpinBox* spinZ = new QDoubleSpinBox(widget);
    connect(spinX, SIGNAL(valueChanged(double)), this, SLOT(shiftX(double)));
    connect(spinY, SIGNAL(valueChanged(double)), this, SLOT(shiftY(double)));
    connect(spinZ, SIGNAL(valueChanged(double)), this, SLOT(shiftZ(double)));
    spinX->setMaximum(1000.0);
    spinY->setMaximum(1000.0);
    spinZ->setMaximum(1000.0);
    spinX->setMinimum(-1000.0);
    spinY->setMinimum(-1000.0);
    spinZ->setMinimum(-1000.0);
    spinX->setSingleStep(0.03);
    spinY->setSingleStep(0.03);
    spinZ->setSingleStep(0.03);

    spinWidgetX->layout()->addWidget(spinX);
    spinWidgetY->layout()->addWidget(spinY);
    spinWidgetZ->layout()->addWidget(spinZ);

    spinWidgetX->layout()->setMargin(0);
    spinWidgetY->layout()->setMargin(0);
    spinWidgetZ->layout()->setMargin(0);

    spinWidgetX->layout()->setContentsMargins(0, 0, 0, 0);
    spinWidgetY->layout()->setContentsMargins(0, 0, 0, 0);
    spinWidgetZ->layout()->setContentsMargins(0, 0, 0, 0);

	QAction* lft_action = viewMenu->addAction("Left"); 
	QAction* rht_action = viewMenu->addAction("Right"); 
	QAction* frn_action = viewMenu->addAction("Front"); 
	QAction* bck_action = viewMenu->addAction("Back"); 
	QAction* top_action = viewMenu->addAction("Top"); 
	QAction* btm_action = viewMenu->addAction("Bottom"); 
	
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

    core::IActionsGroupManager::GroupID id = manager->createGroup("Properties");
    manager->addGroupAction(id, activeSerieCombo);
    manager->addGroupAction(id, actionTrajectories);
    manager->addGroupAction(id, actionGhost);
    manager->addGroupAction(id, spinWidgetX);
    manager->addGroupAction(id, spinWidgetY);
    manager->addGroupAction(id, spinWidgetZ);

    id = manager->createGroup("View");
    manager->addGroupAction(id, viewMenu);
    //manager->addGroupAction(id, leftAction);
    //manager->addGroupAction(id, rightAction);
    manager->addGroupAction(id, actionSwitchAxes);
    
    retWidget->setFocusPolicy(Qt::StrongFocus);
    widget->setFocusPolicy(Qt::StrongFocus);
    retWidget->layout()->setContentsMargins(2,0,2,2);

    return retWidget;
}

const std::string& KinematicVisualizer::getName() const
{
    return name;
}

KinematicVisualizer::KinematicVisualizer() :
    name("Visualizer 3D"),
    trajectoriesDialog(nullptr),
    currentSerie(-1)
{

}

void KinematicVisualizer::update( double deltaTime )
{

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
    /*for (int i = series.size() - 1; i >= 0; --i) {
        series[i]->setActive(idx == i);
    }*/
    if (currentSerie >= 0) {
        series[currentSerie]->getTransformNode()->removeChild(indicatorNode);
    }
    currentSerie = idx;
    series[currentSerie]->getTransformNode()->addChild(indicatorNode);
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
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        KinematicSerie::TransformPtr transform = current->getTransformNode();
        transform->setPosition(transform->getPosition() + osg::Vec3(DEFAULT_SHIFT, 0.0f, 0.0f));
    }
}

void KinematicVisualizer::shiftRight()
{
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        KinematicSerie::TransformPtr transform = current->getTransformNode();
        transform->setPosition(transform->getPosition() + osg::Vec3(-DEFAULT_SHIFT, 0.0f, 0.0f));
    }
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
        KinematicSerie::TransformPtr transform = current->getTransformNode();
        osg::Vec3 pos = transform->getPosition();
        pos._v[0] = d;
        transform->setPosition(pos);
    }
}

void KinematicVisualizer::shiftY( double d )
{
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        KinematicSerie::TransformPtr transform = current->getTransformNode();
        osg::Vec3 pos = transform->getPosition();
        pos._v[1] = d;
        transform->setPosition(pos);
    }
}

void KinematicVisualizer::shiftZ( double d )
{
    KinematicSerie* current = tryGetCurrentSerie();
    if (current) {
        KinematicSerie::TransformPtr transform = current->getTransformNode();
        osg::Vec3 pos = transform->getPosition();
        pos._v[2] = d;
        transform->setPosition(pos);
    }
}




