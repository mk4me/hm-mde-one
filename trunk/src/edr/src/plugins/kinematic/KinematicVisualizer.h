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
#include <osgui/QOsgWidgets.h>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
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
        matrixTransform(new osg::MatrixTransform())
    {
        matrixTransform->addChild(transformNode);
    }

    MatrixTransformPtr getMatrixTransformNode() { return matrixTransform; }
    osg::Vec3 getPivot() const 
    {
        auto matrix = matrixTransform->getMatrix();
        auto t1 = matrix.getTrans();
        matrix.setTrans(osg::Vec3());
        return getLocalPivot() * matrix + t1; 
    }

     virtual void setEvents(EventsCollectionConstPtr val) {
         events = val;
     }

    double getTime() const { return time; }
    virtual void setTime(double val) { time = val; setLocalTime(val); }

protected:
    virtual osg::Vec3 getLocalPivot() const { return transformNode->getPosition(); }
    virtual void setLocalTime(double time) = 0;

protected:
    TransformPtr transformNode;
    EventsCollectionConstPtr events;

private:
    MatrixTransformPtr matrixTransform;
    double time;
};

class KinematicVisualizer :  public QObject, public core::IVisualizer
{
	// wszystkie mozliwe serie przeniesione do osobnego pliku
	friend class GRFSerie;
	friend class MarkerSerie;
	friend class SkeletonSerie;
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
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
    void updateAnimation();
	void resetScene();

private:
    osg::ref_ptr<osg::Group> createFloor();
    void addAction(const std::string& name, QMenu* menu, QActionGroup* group);
    void refillDrawersMaps();
    KinematicSerie* tryGetCurrentSerie();
    void refreshSpinboxes();
    KinematicSerie* getParentSerie(GeodePtr geode);

private slots:
    void showTrajectoriesDialog();
    void showSchemeDialog();
    void setActiveSerie(int idx);
    void setActiveSerie(KinematicSerie* serie);
    void shiftLeft();
    void shiftRight();
    void shiftX(double d);
    void shiftY(double d);
    void shiftZ(double d);

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

private:
    SkeletalVisualizationSchemePtr scheme;
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

    TrajectoriesDialog* trajectoriesDialog;
    SchemeDialog* schemeDialog;

    QComboBox* activeSerieCombo;
    int currentSerie;
    std::vector<KinematicSerie*> series;

    QDoubleSpinBox* spinX;
    QDoubleSpinBox* spinY;
    QDoubleSpinBox* spinZ;

    DraggerContainerPtr translateDragger;
    DraggerContainerPtr rotationDragger;
    DraggerContainerPtr scaleDragger;
    DraggerContainerPtr currentDragger;
    double lastTime;
};


class KinematicNotifier : public osg::NodeCallback
{
private:
    KinematicVisualizer* vis;

public:
    //! \param w Pocz¹tkowa szerokoœæ.
    //! \param h Pocz¹tkowa wysokoœæ.
    KinematicNotifier(KinematicVisualizer* visualizer) :
      vis(visualizer)
      {}

      //! \param node
      //! \param nv
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
          vis->updateAnimation();
          traverse(node, nv);
      }
};

#endif  //  HEADER_GUARD___KINEMATICVISUALIZER_H__