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
#include "ISchemeDrawer.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"
#include "PointSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"
#include "GlLineSchemeDrawer.h"
#include "SchemeDrawerContainer.h"

#include "TrajectoriesDialog.h"

class KinematicVisualizer :  public QObject, public core::IVisualizer
{
	// wszystkie mozliwe serie przeniesione do osobnego pliku
	friend class GRFSerie;
	friend class MarkerSerie;
	friend class SkeletonSerie;
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
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
    virtual QWidget* createWidget(std::vector<QObject*>& actions);
    virtual QIcon* createIcon();
    virtual const std::string& getName() const;
    virtual void reset();

public:
    void updateAnimation();
	void resetScene();

private:
    osg::ref_ptr<osg::Group> createFloor();
    void addAction(const std::string& name, QMenu* menu, QActionGroup* group);
    void refillDrawersMaps();

	private slots:
       // void setAxis(bool xyz);
        void showTrajectoriesDialog();

public slots:
	void setLeft();
	void setRight();
	void setFront();
	void setBehind();
	void setTop();
	void setBottom();
    void actionTriggered(QAction* action);

public:
    virtual osg::Node* debugGetLocalSceneRoot();

private:
    SkeletalVisualizationSchemePtr scheme;
    QTimer updateTimer;
    std::string name;
    osg::ref_ptr<osg::Group> rootNode;
    osg::ref_ptr<osg::PositionAttitudeTransform> transformNode;
	osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator;

    std::map<QAction*, SchemeDrawerContainerPtr> drawersByAction;
    std::map<std::string, SchemeDrawerContainerPtr> drawersByName;
    std::map<std::string, QAction*> actionByName;

    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    OsgSchemeDrawerPtr currentDrawer;
    QAction* actionTrajectories;
    QAction* actionGhost;
    QAction* actionSwitchAxes;
    TrajectoriesDialog* trajectoriesDialog;
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