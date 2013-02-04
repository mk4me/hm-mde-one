#include "PCH.h"
#include "KinematicVisualizer.h"
#include "GRFSerie.h"
#include "SkeletonSerie.h"
#include "MarkerSerie.h"
#include "PickHandler.h"
#include "Manipulators.h"
#include <coreui/CoreAction.h>
#include <coreui/CoreWidgetAction.h>

using namespace coreUI;

KinematicVisualizer::KinematicVisualizer() :
	actionGhost(nullptr), actionSwitchAxes(nullptr), lastTime(-1.0f),
	trajectoriesDialog(nullptr), schemeDialog(nullptr)
{
	
}

plugin::IVisualizer* KinematicVisualizer::create() const
{
    return new KinematicVisualizer();
}

void KinematicVisualizer::getSupportedTypes(utils::TypeInfoList & supportedTypes) const
{
	supportedTypes.push_back(typeid(MarkerCollection));
	supportedTypes.push_back(typeid(GRFCollection));
	supportedTypes.push_back(typeid(kinematic::JointAnglesCollection));
}

int KinematicVisualizer::getMaxDataSeries() const
{
    return -1;
}

plugin::IVisualizer::ISerie *KinematicVisualizer::createSerie(const core::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data)
{
	KinematicSerie * ret = nullptr;
	if (requestedType == typeid(GRFCollection)) {
		ret = new GRFSerie(this);
		ret->setName("grf");
		ret->setData(requestedType, data);
	} else if (requestedType == typeid (MarkerCollection)) {
		ret = new MarkerSerie(this);
		ret->setName("markers");
		ret->setData(requestedType, data);
	} else if (requestedType == typeid (kinematic::JointAnglesCollection)) {
		ret = new SkeletonSerie(this);
		ret->setName("skeleton");
		ret->setData(requestedType, data);
	} else {
		UTILS_ASSERT(false);
	}

    if (ret) {
        transformNode->addChild(ret->getMatrixTransformNode());
        series.push_back(ret);
    }

    refreshSpinboxes();
    return ret;
}

plugin::IVisualizer::ISerie * KinematicVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie)
{
    return nullptr;
}

void KinematicVisualizer::removeSerie(plugin::IVisualizer::ISerie *serie)
{
    KinematicSerie* kSerie = dynamic_cast<KinematicSerie*>(serie);
    KinematicSerie* activeSerie = tryGetCurrentSerie();

    auto it = std::find(series.begin(), series.end(), kSerie);

    transformNode->removeChild((*it)->getMatrixTransformNode());
    series.erase(it);
}

QWidget* KinematicVisualizer::createWidget()
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
    // stworzenie i dodanie światła przyczepionego do kamery
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
    icon0.addFile(QString::fromUtf8(":/resources/icons/switch-axisa.png"), QSize(), QIcon::Normal, QIcon::Off);
    icon0.addFile(QString::fromUtf8(":/resources/icons/switch-axis.png"), QSize(), QIcon::Normal, QIcon::On);
    actionSwitchAxes = new CoreAction(tr("View"), icon0, tr("Switch Axes"), widget, CoreTitleBar::Right);
    actionSwitchAxes->setCheckable(true);
    actionSwitchAxes->setChecked(false);
    actionSwitchAxes->setVisible(false);
    connect(actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));
	widget->addAction(actionSwitchAxes);


	CoreAction * actionTrajectories = new CoreAction(tr("Properties"), QIcon(QString::fromUtf8(":/resources/icons/tracea.png")), tr("Trajectories"), widget, CoreTitleBar::Left);
    actionTrajectories->setCheckable(true);
    connect(actionTrajectories, SIGNAL(triggered()), this, SLOT(showTrajectoriesDialog()));
	widget->addAction(actionTrajectories);


    CoreAction * actionScheme = new CoreAction(tr("Properties"), QIcon(QString::fromUtf8(":/resources/icons/trace-blue.png")), tr("Scheme Drawers"), widget, CoreTitleBar::Left);
    actionScheme->setCheckable(true);
    connect(actionScheme, SIGNAL(triggered()), this, SLOT(showSchemeDialog()));
	widget->addAction(actionScheme);
	

    CoreAction * actionGhost = new CoreAction(tr("Properties"), QIcon(QString::fromUtf8(":/resources/icons/skeletal_tracea.png")), tr("Ghost"), widget, CoreTitleBar::Left);
    actionGhost->setCheckable(true);
    actionGhost->setVisible(false);
	connect(actionGhost, SIGNAL(triggered(bool)), this, SLOT(showGhost(bool)));
	widget->addAction(actionGhost);
	

	QMenu* viewMenu = new QMenu(tr("View"), widget);
    viewMenu->setIcon(QIcon(QString::fromUtf8(":/resources/icons/viewa.png")));
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

	CoreWidgetAction * viewAction = new CoreWidgetAction(widget, tr("View"), CoreTitleBar::Right);
	viewAction->setDefaultWidget(viewMenu);
	widget->addAction(viewAction);


    translateSpinWidgetX = createSpinWidget(widget, "TX: ", 0.3);
    translateSpinWidgetY = createSpinWidget(widget, "TY: ", 0.3);
    translateSpinWidgetZ = createSpinWidget(widget, "TZ: ", 0.3);

    connect(translateSpinWidgetX.second, SIGNAL(valueChanged(double)), this, SLOT(shiftX(double)));
    connect(translateSpinWidgetY.second, SIGNAL(valueChanged(double)), this, SLOT(shiftY(double)));
    connect(translateSpinWidgetZ.second, SIGNAL(valueChanged(double)), this, SLOT(shiftZ(double)));

    rotateSpinWidgetX = createSpinWidget(widget, "RX: ", 5.0);
    rotateSpinWidgetY = createSpinWidget(widget, "RY: ", 5.0);
    rotateSpinWidgetZ = createSpinWidget(widget, "RZ: ", 5.0);
    
    connect(rotateSpinWidgetX.second, SIGNAL(valueChanged(double)), this, SLOT(rotateX(double)));
    connect(rotateSpinWidgetY.second, SIGNAL(valueChanged(double)), this, SLOT(rotateY(double)));
    connect(rotateSpinWidgetZ.second, SIGNAL(valueChanged(double)), this, SLOT(rotateZ(double)));

    scaleSpinWidgetX = createSpinWidget(widget, "SX: ", 0.1);
    scaleSpinWidgetY = createSpinWidget(widget, "SY: ", 0.1);
    scaleSpinWidgetZ = createSpinWidget(widget, "SZ: ", 0.1);

    connect(scaleSpinWidgetX.second, SIGNAL(valueChanged(double)), this, SLOT(scaleX(double)));
    connect(scaleSpinWidgetY.second, SIGNAL(valueChanged(double)), this, SLOT(scaleY(double)));
    connect(scaleSpinWidgetZ.second, SIGNAL(valueChanged(double)), this, SLOT(scaleZ(double)));

    
    CoreAction * pickerAction = new CoreAction(tr("Manipulators"), QIcon(":/resources/icons/picker.png"), tr("Picker"), widget, CoreTitleBar::Left);
	connect(pickerAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(pickerAction);


	CoreAction * translateAction = new CoreAction(tr("Manipulators"), QIcon(":/resources/icons/manipulator-translate.png"), tr("Translate"), widget, CoreTitleBar::Left);
	connect(translateAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(translateAction);


	CoreAction * rotateAction = new CoreAction(tr("Manipulators"), QIcon(":/resources/icons/manipulator-rotate.png"), tr("Rotate"), widget, CoreTitleBar::Left);
	connect(rotateAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(rotateAction);


	CoreAction * scaleAction = new CoreAction(tr("Manipulators"), QIcon(":/resources/icons/manipulator-scale.png"), tr("Scale"), widget, CoreTitleBar::Left);    
    connect(scaleAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(scaleAction);
		

    CoreAction * resetTransformAction = new CoreAction(tr("Manipulators"), QIcon(":/resources/icons/left-b.png"), tr("Reset transform"), widget, CoreTitleBar::Left);    
    connect(resetTransformAction, SIGNAL(triggered()), this, SLOT(resetTransform()));
	widget->addAction(resetTransformAction);

	
    osg::Vec3 pos (0.0f, 9.0f, 3.0f);
    osg::Vec3 up(0,0,1);

    osg::Vec3 at;
    widget->getCamera()->setViewMatrixAsLookAt(pos, at, up);
    rootNode = new osg::Group;
    rootNode->addChild(createFloor());
    widget->setSceneData(rootNode);
    widget->setMinimumSize(50, 50);

    indicatorNode = createIndicator();

#ifdef _DEBUG

	//dodajemy tez event handler ze statystykami
	widget->addEventHandler( new osgViewer::StatsHandler() );
	widget->setTimerActive(true);

#endif

	widget->setFocusPolicy(Qt::StrongFocus);

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

    id = manager->createGroup(tr("Manipulators properties"));
   
    manager->addGroupAction(id, translateSpinWidgetX.first);
    manager->addGroupAction(id, translateSpinWidgetY.first);
    manager->addGroupAction(id, translateSpinWidgetZ.first);
    manager->addGroupAction(id, rotateSpinWidgetX.first);
    manager->addGroupAction(id, rotateSpinWidgetY.first);
    manager->addGroupAction(id, rotateSpinWidgetZ.first);
    manager->addGroupAction(id, scaleSpinWidgetX.first);
    manager->addGroupAction(id, scaleSpinWidgetY.first);
    manager->addGroupAction(id, scaleSpinWidgetZ.first);



    id = manager->createGroup(tr("View"));
    manager->addGroupAction(id, viewMenu);
    manager->addGroupAction(id, actionSwitchAxes);

    currentDragger = nullptr;
    translateDragger = Manipulators::creatreDraggerContainer(Manipulators::TranslateAxis);
    rotationDragger = Manipulators::creatreDraggerContainer(Manipulators::Trackball);
    scaleDragger = Manipulators::creatreDraggerContainer(Manipulators::TabBox);

    translateDragger->getDragger()->addDraggerCallback(new KinematicDraggerCallback(this));
    rotationDragger->getDragger()->addDraggerCallback(new KinematicDraggerCallback(this));
    scaleDragger->getDragger()->addDraggerCallback(new KinematicDraggerCallback(this));

    return widget;
}

void KinematicVisualizer::update( double deltaTime )
{
    KinematicSerie* serie = tryGetCurrentSerie();
    if (serie && serie->getTime() != lastTime) {
        lastTime = serie->getTime();
        if (currentDragger && serie) {
            currentDragger->setDraggerPivot(serie->getPivot());
        }
        refreshSpinboxes();
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
    // todo sparametryzować
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();

	float length = 3.5f;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(44);
    osg::Vec3 v;
    for (int i = 0; i < 11; ++i) {
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

//void KinematicVisualizer::actionTriggered( QAction* action )
//{
//    resetScene();
//    currentDrawer = drawersByAction[action];
//    transformNode->addChild(currentDrawer->getNode());
//}

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

//void KinematicVisualizer::refillDrawersMaps()
//{
//    drawersByAction.clear();
//    drawersByName.clear();
//    for (std::map<std::string, QAction*>::iterator it = actionByName.begin(); it != actionByName.end(); ++it) {
//        SchemeDrawerContainerPtr drawer(new SchemeDrawerContainer());
//        drawersByAction[it->second] = drawer;
//        drawersByName[it->first] = drawer;
//    }
//}

//osg::Node* KinematicVisualizer::debugGetLocalSceneRoot()
//{
//    return rootNode;
//}

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
    if (idx >= 0 && idx < static_cast<int>(series.size())) {
        if (currentSerie >= 0) {
            series[currentSerie]->getMatrixTransformNode()->removeChild(indicatorNode);
            if (currentDragger) {
                Manipulators::disconnect(transformNode, series[currentSerie]->getMatrixTransformNode(), currentDragger);
            }
        }
        currentSerie = idx;
        refreshSpinboxes();
        series[currentSerie]->getMatrixTransformNode()->addChild(indicatorNode);
    } else {
        UTILS_ASSERT(false);
        throw std::runtime_error("Wrong serie index");
    }
}

void KinematicVisualizer::setActiveSerie( KinematicSerie* serie )
{
    for (unsigned int i = 0; i < series.size(); ++i) {
        if (series[i] == serie) {
            setActiveSerie(i);
            return;
        }
    }
    UTILS_ASSERT(false);
}

KinematicSerie* KinematicVisualizer::tryGetCurrentSerie()
{
    int count = static_cast<int>(series.size());
    if (currentSerie >= 0 && currentSerie < count) {
        return series[currentSerie];
    }
    return nullptr;
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
    mat.makeRotate(osg::DegreesToRadians(180.0f), 1.0f, 0.0f, 0.0f);
    osg::Quat orientation;
    orientation.set(mat);
    transform->setAttitude(orientation);
    transform->setPosition(osg::Vec3(0.0f, 0.0f, 1.5f));
    return transform;
}

QPixmap KinematicVisualizer::takeScreenshot() const
{
	return QPixmap::fromImage(widget->grabFrameBuffer(true));
}

void KinematicVisualizer::shiftX( double d )
{
    setTranslation(tryGetCurrentSerie(), 0, d);
}

void KinematicVisualizer::shiftY( double d )
{
    setTranslation(tryGetCurrentSerie(), 1, d);
}

void KinematicVisualizer::shiftZ( double d )
{
    setTranslation(tryGetCurrentSerie(), 2, d);
}

void KinematicVisualizer::showSchemeDialog()
{
    schemeDialog->show();
}

void KinematicVisualizer::refreshSpinboxes()
{
    refreshTranslateSpinboxes();
    refreshRotateSpinboxes();
    refreshScaleSpinboxes();
}

void KinematicVisualizer::draggerTriggered()
{
    //translateSpinWidgetX.first->setVisible(false);
    //translateSpinWidgetY.first->setVisible(false);
    //translateSpinWidgetZ.first->setVisible(false);
    //rotateSpinWidgetX.first->setVisible(false);
    //rotateSpinWidgetY.first->setVisible(false);
    //rotateSpinWidgetZ.first->setVisible(false);
    //scaleSpinWidgetX.first->setVisible(false);
    //scaleSpinWidgetY.first->setVisible(false);
    //scaleSpinWidgetZ.first->setVisible(false);
    auto serie = tryGetCurrentSerie();
    if (serie) {
        QAction* a = qobject_cast<QAction*>(sender());
        if (currentDragger) {
            Manipulators::disconnect(transformNode, serie->getMatrixTransformNode(), currentDragger);
        }

        if (a == translateAction) {
            currentDragger = translateDragger;
            translateSpinWidgetX.first->setVisible(true);
            translateSpinWidgetY.first->setVisible(true);
            translateSpinWidgetZ.first->setVisible(true);
        } else if (a == rotateAction) {
            currentDragger = rotationDragger;
            rotateSpinWidgetX.first->setVisible(true);
            rotateSpinWidgetY.first->setVisible(true);
            rotateSpinWidgetZ.first->setVisible(true);
        } else if (a == scaleAction) {
            currentDragger = scaleDragger;
            scaleSpinWidgetX.first->setVisible(true);
            scaleSpinWidgetY.first->setVisible(true);
            scaleSpinWidgetZ.first->setVisible(true);
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
    for (int i = 0; i < count; ++i) {
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
    for (auto it = series.begin(); it != series.end(); ++it) {
        osg::MatrixTransform* node = (*it)->getMatrixTransformNode();
        if (isChildRecursive(node, geode)) {
            return *it;
        }
    }

    return nullptr;
}

void KinematicVisualizer::refreshTranslateSpinboxes()
{
    translateSpinWidgetX.second->blockSignals(true);
    translateSpinWidgetY.second->blockSignals(true);
    translateSpinWidgetZ.second->blockSignals(true);

    auto serie = tryGetCurrentSerie();
    if (serie) {
        auto t = serie->getMatrixTransformNode();
        auto v = t->getMatrix().getTrans();
        translateSpinWidgetX.second->setValue(v.x());
        translateSpinWidgetY.second->setValue(v.y());
        translateSpinWidgetZ.second->setValue(v.z());
    } else {
        translateSpinWidgetX.second->setValue(0.0);
        translateSpinWidgetY.second->setValue(0.0);
        translateSpinWidgetZ.second->setValue(0.0);
    }
    translateSpinWidgetX.second->blockSignals(false);
    translateSpinWidgetY.second->blockSignals(false);
    translateSpinWidgetZ.second->blockSignals(false);
}



std::pair<QWidget*, QDoubleSpinBox*> KinematicVisualizer::createSpinWidget( QWidget* parent, QString name, double step, bool visible )
{
    QWidget* spinWidget = new QWidget(parent);
    spinWidget->setLayout(new QHBoxLayout());
	auto layout = spinWidget->layout();
    layout->addWidget(new QLabel(name));
    QDoubleSpinBox* spin = new QDoubleSpinBox(parent);
    spin->setMaximum(1000.0);
    spin->setMinimum(-1000.0);
    layout->addWidget(spin);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    spinWidget->setVisible(visible);
    spin->setSingleStep(step);
    return std::make_pair(spinWidget, spin);
}

void KinematicVisualizer::rotateX( double d )
{
    setRotation(tryGetCurrentSerie(), 0, d);
}

void KinematicVisualizer::rotateY( double d )
{
    setRotation(tryGetCurrentSerie(), 1, d);
}

void KinematicVisualizer::rotateZ( double d )
{
    setRotation(tryGetCurrentSerie(), 2, d);
}

void KinematicVisualizer::refreshRotateSpinboxes()
{
    rotateSpinWidgetX.second->blockSignals(true);
    rotateSpinWidgetY.second->blockSignals(true);
    rotateSpinWidgetZ.second->blockSignals(true);

    auto serie = tryGetCurrentSerie();
    if (serie) {
        auto t = serie->getMatrixTransformNode();
        auto q = t->getMatrix().getRotate();
        osg::Vec3 v = getEulerFromQuat(q);
        rotateSpinWidgetX.second->setValue(osg::RadiansToDegrees(v.x()));
        rotateSpinWidgetY.second->setValue(osg::RadiansToDegrees(v.y()));
        rotateSpinWidgetZ.second->setValue(osg::RadiansToDegrees(v.z()));
    } else {
        rotateSpinWidgetX.second->setValue(0.0);
        rotateSpinWidgetY.second->setValue(0.0);
        rotateSpinWidgetZ.second->setValue(0.0);
    }
    rotateSpinWidgetX.second->blockSignals(false);
    rotateSpinWidgetY.second->blockSignals(false);
    rotateSpinWidgetZ.second->blockSignals(false);
}

void KinematicVisualizer::refreshScaleSpinboxes()
{
    scaleSpinWidgetX.second->blockSignals(true);
    scaleSpinWidgetY.second->blockSignals(true);
    scaleSpinWidgetZ.second->blockSignals(true);

    auto serie = tryGetCurrentSerie();
    if (serie) {
        auto t = serie->getMatrixTransformNode();
        auto s = t->getMatrix().getScale();
        scaleSpinWidgetX.second->setValue(s.x());
        scaleSpinWidgetY.second->setValue(s.y());
        scaleSpinWidgetZ.second->setValue(s.z());
    } else {
        scaleSpinWidgetX.second->setValue(1.0);
        scaleSpinWidgetY.second->setValue(1.0);
        scaleSpinWidgetZ.second->setValue(1.0);
    }
    scaleSpinWidgetX.second->blockSignals(false);
    scaleSpinWidgetY.second->blockSignals(false);
    scaleSpinWidgetZ.second->blockSignals(false);
}

osg::Quat KinematicVisualizer::getQuatFromEuler( double heading, double attitude, double bank)
{
    double c1 = cos(heading/2);   
    double s1 = sin(heading/2);   
    double c2 = cos(attitude/2);   
    double s2 = sin(attitude/2);   
    double c3 = cos(bank/2);   
    double s3 = sin(bank/2);
    double c1c2 = c1*c2;  
    double s1s2 = s1*s2;  
    
    double w =c1c2*c3 - s1s2*s3;
    double x =c1c2*s3 + s1s2*c3;
    double y =s1*c2*c3 + c1*s2*s3;

    double z =c1*s2*c3 - s1*c2*s3;

    return osg::Quat(x, y, z, w);
    //q[0] = x; q[1] = y;
    //q[2] = z; q[3] = w;
}

osg::Quat KinematicVisualizer::getQuatFromEuler( const osg::Vec3& euler )
{
    return getQuatFromEuler(euler[0], euler[1], euler[2]);
}

osg::Vec3 KinematicVisualizer::getEulerFromQuat(const osg::Quat& q )
{
    double heading, attitude, bank;

    double limit = 0.499999;
    double sqx = q.x()*q.x();   
    double sqy = q.y()*q.y();   
    double sqz = q.z()*q.z();

    double t = q.x()*q.y() + q.z()*q.w();
    // gimbal lock ?
    if (t > limit) {
        heading = 2 * atan2(q.x(),q.w());
        attitude = osg::PI_2;
        bank = 0;
    } else if (t < -limit) {
        heading = -2 * atan2(q.x(),q.w());
        attitude = - osg::PI_2;
        bank = 0;
    } else {
        heading = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz);
        attitude = asin(2*t);
        bank = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz);
    }

    return osg::Vec3(heading, attitude, bank);
}

void KinematicVisualizer::setTranslation(KinematicSerie* serie, int index, double d )
{
    if (serie) {
        KinematicSerie::MatrixTransformPtr transform = serie->getMatrixTransformNode();
        auto matrix = transform->getMatrix();
        osg::Vec3 pos = matrix.getTrans();
        pos._v[index] = d;
        matrix.setTrans(pos);
        transform->setMatrix(matrix);
        translateDragger->setDraggerPivot(serie->getPivot());
    }
}

void KinematicVisualizer::setTranslation( KinematicSerie* serie, const osg::Vec3& t )
{
    setTranslation(serie, 0, t[0]);
    setTranslation(serie, 1, t[1]);
    setTranslation(serie, 2, t[2]);
}

void KinematicVisualizer::setRotation( KinematicSerie* serie, int index, double d )
{
    if (serie) {
        KinematicSerie::MatrixTransformPtr transform = serie->getMatrixTransformNode();
        auto matrix = transform->getMatrix();
        //auto invT = matrix;
        //invT.setRotate(osg::Quat());
        //matrix = matrix * invT;
        osg::Quat q = matrix.getRotate();
        osg::Vec3 euler = getEulerFromQuat(q);
        euler[index] = osg::DegreesToRadians(d);
        q = getQuatFromEuler(euler);
        //osg::Matrix rMat;
        //rMat.makeIdentity();
        //rMat.setRotate(q);
        //osg::Matrix tMat;
        //tMat.makeIdentity();
        //tMat.setTrans(matrix.getTrans());
        matrix.setRotate(q);
        transform->setMatrix(matrix);
        rotationDragger->setDraggerPivot(serie->getPivot());
    }
}

void KinematicVisualizer::setRotation( KinematicSerie* serie, const osg::Vec3& r )
{
    setRotation(serie, 0, r[0]);
    setRotation(serie, 1, r[1]);
    setRotation(serie, 2, r[2]);
}

void KinematicVisualizer::setRotation( KinematicSerie* serie, const osg::Quat& q )
{
    if (serie) {
        KinematicSerie::MatrixTransformPtr transform = serie->getMatrixTransformNode();
        auto matrix = transform->getMatrix();
        matrix.setRotate(q);
        transform->setMatrix(matrix);
        rotationDragger->setDraggerPivot(serie->getPivot());
    }
}


void KinematicVisualizer::setScale( KinematicSerie* serie, int index, double d )
{
    if (serie && d != 0.0) {
        KinematicSerie::MatrixTransformPtr transform = serie->getMatrixTransformNode();
        osg::Matrix matrix = transform->getMatrix();
        osg::Vec3 scale(matrix(0,index), matrix(1,index), matrix(2,index));
        scale.normalize();
        scale *= d;
        matrix(0,index) = scale[0];
        matrix(1,index) = scale[1];
        matrix(2,index) = scale[2];
        transform->setMatrix(matrix);
    }
}

void KinematicVisualizer::setScale( KinematicSerie* serie, const osg::Vec3& s )
{
    setScale(serie, 0, s[0]);
    setScale(serie, 1, s[1]);
    setScale(serie, 2, s[2]);
}

void KinematicVisualizer::setTransformMatrix( KinematicSerie* serie, const osg::Matrix& m )
{
    if (serie) {
        KinematicSerie::MatrixTransformPtr transform = serie->getMatrixTransformNode();
        transform->setMatrix(m);
    }
}


void KinematicVisualizer::resetTransform()
{
    auto serie = tryGetCurrentSerie();
    if (serie) {
        serie->resetTransform();
    } else {
        setTranslation(serie,osg::Vec3());
        setRotation(serie, osg::Quat());
        setScale(serie, osg::Vec3(1.0f, 1.0f, 1.0f));
    }
}

void KinematicVisualizer::scaleX( double d )
{
    setScale(tryGetCurrentSerie(), 0, d);
}

void KinematicVisualizer::scaleY( double d )
{
    setScale(tryGetCurrentSerie(), 1, d);
}

void KinematicVisualizer::scaleZ( double d )
{
    setScale(tryGetCurrentSerie(), 2, d);
}


bool KinematicDraggerCallback::receive( const osgManipulator::MotionCommand& mc)
{
    if (mc.getStage() == osgManipulator::MotionCommand::MOVE) {
        visualizer->refreshSpinboxes();
        return true;
    } 
    return osgManipulator::DraggerCallback::receive(mc);
}

