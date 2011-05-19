#ifndef __HEADER_GUARD__KINEMATICSERVICE_H__
#define __HEADER_GUARD__KINEMATICSERVICE_H__

#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <core/IService.h>
#include <plugins/timeline/Stream.h>
#include <core/IVisualizer.h>
#include <osg/Geode>
#include <osgui/QOsgWidgets.h>
#include <plugins/timeline/Stream.h>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QWidget>
#include "ISchemeDrawer.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"
#include "PointSchemeDrawer.h"
#include "KinematicService.h"
#include "GlPointSchemeDrawer.h"
#include "GlLineSchemeDrawer.h"
#include "SchemeDrawerContainer.h"

class IModel;
class IWidget;

/**
 * 
 */
class KinematicTimeline : public timeline::Stream
{
private:
    SkeletalVisualizationSchemePtr scheme;
    double temptime;

public:
  KinematicTimeline(SkeletalVisualizationSchemePtr visualizationScheme) : 
      scheme(visualizationScheme),
      temptime(-1.0)
  {}
  //! \see Stream::setTime
  virtual void setTime(double time)
  {
      if (temptime != time && scheme && scheme->getKinematicModel() && scheme->getTime() != time) {
        scheme->setTime(time);
      }
      temptime = time;
  }
  //! \see Stream::getTime
  virtual double getTime() const
  {
      if (scheme && scheme->getKinematicModel()) {
        return scheme->getTime();
      }
      return 0.0;
  }
  //! \see Stream::getLength
  virtual double getLength() const
  {
      if (scheme && scheme->getKinematicModel()) {
            return scheme->getDuration();
      }

      return 0.0;
  }
};

class KinematicVisualizer :  public QObject, public core::IVisualizer
{
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    Q_OBJECT;
    UNIQUE_ID("{E8B5DEB2-5C57-4323-937D-1FFD288B65B9}", "Kinematic visualizer");
public:
    KinematicVisualizer();

public:
    virtual void update( double deltaTime );
    virtual void setUp( core::IObjectSource* source );

    void resetScene();
    
    virtual core::IVisualizer* create() const;
    virtual void getInputInfo( int source, core::IInputDescription::InputInfo& info );
    virtual QWidget* createWidget(std::vector<QObject*>& actions);
    virtual QIcon* createIcon();
    virtual const std::string& getName() const;

public:
    void updateAnimation();

    int number;

    int getNumber() const { return number; }
    void setNumber(int val) { number = val; }

private:
    GeodePtr createFloor();
    void addAction(std::vector<QObject*>& actions, const std::string& name, QMenu* menu, QActionGroup* group);
    void refillDrawersMaps();

private slots:
    void setAxis(bool xyz);
    void actionTriggered(QAction* action);

public:
    virtual osg::Node* debugGetLocalSceneRoot();


private:
    SkeletalVisualizationSchemePtr scheme;
    QTimer updateTimer;
    std::string name;
    osg::ref_ptr<osg::Group> rootNode;
    osg::ref_ptr<osg::PositionAttitudeTransform> transformNode;

    std::map<QAction*, SchemeDrawerContainerPtr> drawersByAction;
    std::map<std::string, SchemeDrawerContainerPtr> drawersByName;
    std::map<std::string, QAction*> actionByName;

    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    OsgSchemeDrawerPtr currentDrawer;
    QAction* actionSwitchAxes;
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



class KinematicService : public core::IService
{
    UNIQUE_ID("{62C6BDB0-2A27-4714-8BF5-7F6064EA1FB1}", "Kinematic Service");

public:
    KinematicService();

    virtual QWidget* getSettingsWidget()
    {
        return nullptr;
    }

    virtual QWidget* getWidget() 
    { 
        return nullptr;        
    }
    virtual const std::string& getName() const
    {
        return name;
    }
   
    virtual void loadData(core::IServiceManager* serviceManager, core::IDataManager* dataManager);

private:
    std::string name;
    //MainWindow* widget;
   // SkeletonViewerLogicPtr logic;
    //KinematicVisualizer viz;
    
    std::vector<timeline::StreamPtr> stream;
};


#endif  // __HEADER_GUARD__KINEMATICSERVICE_H__