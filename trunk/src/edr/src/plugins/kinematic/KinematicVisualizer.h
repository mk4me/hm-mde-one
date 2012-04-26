/********************************************************************
    created:  2011/07/15
    created:  15:7:2011   12:35
    filename: KinematicVisualizer.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___KINEMATICVISUALIZER_H__
#define HEADER_GUARD___KINEMATICVISUALIZER_H__

#include <QtCore/QObject>

#include <core/IVisualizer.h>
#include <osg/Geode>
#include <osgManipulator/Dragger>
#include <osgManipulator/Command>
#include <osgui/QOsgWidgets.h>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>

#include <plugins/c3d/EventSerieBase.h>
#include "ISchemeDrawer.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"
#include "PointSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"
#include "GlLineSchemeDrawer.h"
#include "SchemeDrawerContainer.h"
#include "Manipulators.h"
#include "TrajectoriesDialog.h"
#include "SchemeDialog.h"



class KinematicSerie : public EventSerieBase
{
public: 
    typedef osg::ref_ptr<osg::MatrixTransform> MatrixTransformPtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;

    KinematicSerie() :
        transformNode(new osg::PositionAttitudeTransform()),
        matrixTransform(new osg::MatrixTransform()),
        time(0.0)
    {
        matrixTransform->addChild(transformNode);
    }

    MatrixTransformPtr getMatrixTransformNode() { return matrixTransform; }
    osg::Vec3 getPivot() const 
    {
        auto matrix = matrixTransform->getMatrix();
        auto t1 = matrix.getTrans();
        return t1;
        //matrix.setTrans(osg::Vec3());
        //return getLocalPivot() * matrix + t1; 
    }

     virtual void setEvents(EventsCollectionConstPtr val) {
         events = val;
     }

    double getTime() const { return time; }
    virtual void setTime(double val) { 
        time = val; setLocalTime(val);
    }

    virtual osg::Matrix getInitialMatrix() const { return osg::Matrix(); }
    virtual void resetTransform();

protected:
    virtual void setLocalTime(double time) = 0;
protected:
    TransformPtr transformNode;
    EventsCollectionConstPtr events;
    MatrixTransformPtr matrixTransform;

private:
    double time;
};

class KinematicVisualizer :  public QObject, public core::IVisualizer
{
	// wszystkie mozliwe serie przeniesione do osobnego pliku
	friend class GRFSerie;
	friend class MarkerSerie;
	friend class SkeletonSerie;
    friend class KinematicDraggerCallback;
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    typedef std::pair<QWidget*, QDoubleSpinBox*> SpinPair;
    Q_OBJECT;
    UNIQUE_ID("{E8B5DEB2-5C57-4323-937D-1FFD288B65B9}", "Kinematic visualizer");

public:
    KinematicVisualizer();

public:
    virtual void update( double deltaTime );
    virtual void setUp( core::IObjectSource* source );
	virtual int getMaxDataSeries() const;
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::IVisualizer::SerieBase* serie);
    virtual void removeSerie(core::IVisualizer::SerieBase *serie);
	virtual core::IVisualizer* createClone() const;
    virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info );
    virtual QWidget* createWidget(core::IActionsGroupManager * manager);

    

    virtual QIcon* createIcon();
    virtual const std::string& getName() const;
    virtual void reset();
    virtual QPixmap print() const;
public:
	void resetScene();

private:
    osg::ref_ptr<osg::Group> createFloor();
    void addAction(const std::string& name, QMenu* menu, QActionGroup* group);
    void refillDrawersMaps();
    KinematicSerie* tryGetCurrentSerie();
    void refreshSpinboxes();
    KinematicSerie* getParentSerie(GeodePtr geode);

    osg::Vec3 getEulerFromQuat(const osg::Quat& q);
    osg::Quat getQuatFromEuler(double heading, double attitude, double bank);
    osg::Quat getQuatFromEuler(const osg::Vec3& euler);

    SpinPair createSpinWidget( QWidget* parent, QString name, double step = 0.1, bool visible = true );

    void setTransformMatrix(KinematicSerie* serie, const osg::Matrix& m);

    void setTranslation(KinematicSerie* serie, int index, double d );
    void setRotation( KinematicSerie* serie, int index, double d );
    void setScale(KinematicSerie* serie, int index, double d );

    void setTranslation(KinematicSerie* serie, const osg::Vec3& t  );
    void setRotation( KinematicSerie* serie, const osg::Vec3& r );
    void setRotation( KinematicSerie* serie, const osg::Quat& q);
    void setScale(KinematicSerie* serie, const osg::Vec3& s );

private slots:
    void showTrajectoriesDialog();
    void showSchemeDialog();
    void setActiveSerie(int idx);
    void setActiveSerie(KinematicSerie* serie);

    void shiftX(double d);
    void shiftY(double d);
    void shiftZ(double d);

    void rotateX(double d);
    void rotateY(double d);
    void rotateZ(double d);

    void scaleX(double d);
    void scaleY(double d);
    void scaleZ(double d);

    void resetTransform();


public slots:
	void setLeft();
	void setRight();
	void setFront();
	void setBehind();
	void setTop();
	void setBottom();
    void actionTriggered(QAction* action);
    void draggerTriggered();

public:
    virtual osg::Node* debugGetLocalSceneRoot();
    TransformPtr createIndicator() const;

    void refreshTranslateSpinboxes();
    void refreshRotateSpinboxes();
    void refreshScaleSpinboxes();

private:
    VisualizationSchemePtr scheme;
    QTimer updateTimer;
    std::string name;
    osg::ref_ptr<osg::Group> rootNode;
    TransformPtr transformNode;
	osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator;
    TransformPtr indicatorNode;
    std::map<QAction*, SchemeDrawerContainerPtr> drawersByAction;
    std::map<std::string, SchemeDrawerContainerPtr> drawersByName;
    std::map<std::string, QAction*> actionByName;

    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    OsgSchemeDrawerPtr currentDrawer;
    QAction* actionTrajectories;
    QAction* actionScheme;
    QAction* actionGhost;
    QAction* actionSwitchAxes;

    QAction* pickerAction;
    QAction* translateAction;
    QAction* rotateAction;
    QAction* scaleAction;
    QAction* resetAction;

    TrajectoriesDialog* trajectoriesDialog;
    SchemeDialog* schemeDialog;

    QComboBox* activeSerieCombo;
    int currentSerie;
    std::vector<KinematicSerie*> series;

    SpinPair translateSpinWidgetX;
    SpinPair translateSpinWidgetY;
    SpinPair translateSpinWidgetZ;

    SpinPair rotateSpinWidgetX;
    SpinPair rotateSpinWidgetY;
    SpinPair rotateSpinWidgetZ;

    SpinPair scaleSpinWidgetX;
    SpinPair scaleSpinWidgetY;
    SpinPair scaleSpinWidgetZ;

    DraggerContainerPtr translateDragger;
    DraggerContainerPtr rotationDragger;
    DraggerContainerPtr scaleDragger;
    DraggerContainerPtr currentDragger;
    double lastTime;
};

class KinematicDraggerCallback : public osgManipulator::DraggerCallback
{
public:
    KinematicDraggerCallback(KinematicVisualizer* kv) : visualizer(kv) {}

public:
    virtual bool receive( const osgManipulator::MotionCommand& );
private:
    KinematicVisualizer* visualizer;
};


#endif  //  HEADER_GUARD___KINEMATICVISUALIZER_H__