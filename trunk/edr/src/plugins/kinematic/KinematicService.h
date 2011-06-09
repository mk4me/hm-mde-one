#ifndef __HEADER_GUARD__KINEMATICSERVICE_H__
#define __HEADER_GUARD__KINEMATICSERVICE_H__

#include <core/LineChartSerie.h>
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
    class KinematicVisualizerSerie : public SerieBase
    {
    public:
        KinematicVisualizerSerie(KinematicVisualizer * visualizer)
            : visualizer(visualizer)
        {

        }

    protected:
        virtual void setSerieName(const std::string & name)
        {
            //TODO
            //obecnie nazwy serii nie sa obslugiwane ale musimy pomyslec o tym i ewentualnie dodac!!
        }

        virtual void setSerieData(const core::ObjectWrapperConstPtr & data)
        {
            visualizer->resetScene();
            visualizer->refillDrawersMaps();

            SkeletalVisualizationSchemeConstPtr scheme = data->get();

            if (scheme) {        
                if (scheme->getKinematicModel()->getSkeleton()) {
                    SchemeDrawerContainerPtr skeleton = visualizer->drawersByName["Skeleton"];
                    skeleton->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 3, 0.015f, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f))));
                    skeleton->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 10, 0.05f)));
                    skeleton->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawSkeleton, 2, 0.08f)));

                    SchemeDrawerContainerPtr rectangular = visualizer->drawersByName["Rectangular"];
                    rectangular->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 4, 0.25f, osg::Vec4(0, 1, 0, 0.3f))));
                    visualizer->actionByName["Skeleton"]->setVisible(true);
                    visualizer->actionByName["Rectangular"]->setVisible(true);
                } else {
                    visualizer->actionByName["Skeleton"]->setVisible(false);
                    visualizer->actionByName["Rectangular"]->setVisible(false);
                }

                if (scheme->getKinematicModel()->getMarkers()) {
                    SchemeDrawerContainerPtr markers = visualizer->drawersByName["Markers"];
                    markers->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawMarkers, 3, 0.08f)));
                    markers->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawMarkers, 10, 0.02f)));
                    visualizer->currentDrawer = markers;
                    visualizer->actionByName["Markers"]->setVisible(true);
                } else {
                    visualizer->actionByName["Markers"]->setVisible(false);
                }

                if (scheme->getKinematicModel()->getMarkers() && scheme->getKinematicModel()->getSkeleton()) {
                    SchemeDrawerContainerPtr both = visualizer->drawersByName["Both"];
                    both->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 3, 0.015f, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f))));
                    both->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 10, 0.05f)));
                    both->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawSkeleton, 2, 0.08f)));
                    both->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawMarkers, 3, 0.08f)));
                    both->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawMarkers, 10, 0.02f)));
                    visualizer->currentDrawer = both;
                    visualizer->actionByName["Both"]->setVisible(true);
                } else {
                    visualizer->actionByName["Both"]->setVisible(false);
                }

                for (auto it = visualizer->drawersByName.begin(); it != visualizer->drawersByName.end(); it++) {
                    (it->second)->init(scheme);
                }
                visualizer->transformNode->addChild(visualizer->currentDrawer->getNode());
            }
        }

    private:
        KinematicVisualizer * visualizer;
        
    };

public:
    KinematicVisualizer();

public:
    virtual void update( double deltaTime );
    virtual void setUp( core::IObjectSource* source );

    void resetScene();

    virtual int getMaxDataSeries() const;
    virtual core::IVisualizer::SerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());
    virtual void removeSerie(core::IVisualizer::SerieBase *serie);
    
    virtual core::IVisualizer* createClone() const;
    virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info );
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

    virtual void finalize() {};

    virtual QWidget* getSettingsWidget(std::vector<QObject*>& actions)
    {
        return nullptr;
    }

    virtual QWidget* getWidget(std::vector<QObject*>& actions) 
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