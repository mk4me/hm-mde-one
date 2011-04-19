#ifndef __HEADER_GUARD__KINEMATICSERVICE_H__
#define __HEADER_GUARD__KINEMATICSERVICE_H__

#include <QtCore/QTimer>
#include <core/IService.h>
#include <plugins/timeline/Stream.h>
#include <core/IVisualizer.h>
#include <osg/Geode>
#include <plugins/timeline/Stream.h>

class IModel;
class IWidget;

/**
 * 
 */
class KinematicTimeline : public timeline::Stream
{
private:
    SkeletalVisualizationSchemePtr scheme;

public:
  KinematicTimeline(SkeletalVisualizationSchemePtr visualizationScheme) : 
      scheme(visualizationScheme)
  {}
  //! \see Stream::setTime
  virtual void setTime(double time)
  {
      if (scheme && scheme->getKinematicModel()) {
        scheme->setTime(time);
      }
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

class KinematicVisualizer : public core::IVisualizer
{
    UNIQUE_ID('KINE', 'VISU');
public:
    KinematicVisualizer();

public:
    virtual void update( double deltaTime );
    virtual void setUp( core::IObjectSource* source );
    virtual core::IVisualizer* create() const;
    virtual void getSlotInfo( int source, std::string& name, core::ObjectWrapper::Types& types );
    virtual QWidget* createWidget(std::vector<QObject*>& actions);
    virtual QIcon* createIcon();
    virtual const std::string& getName() const;

public:
    void updateAnimation();

private:
    GeodePtr createFloor();
    
private:
    SkeletalVisualizationSchemePtr scheme;
    QTimer updateTimer;
    std::string name;
    osg::ref_ptr<osg::Group> rootNode;
    //MainWindow* widget;
    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    std::vector<OsgSchemeDrawerPtr> drawers;
};


class KinematicNotifier : public osg::NodeCallback
{
private:
    KinematicVisualizer* vis;

public:
    //! \param w Początkowa szerokość.
    //! \param h Początkowa wysokość.
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
    UNIQUE_ID('KINE','SRVC');

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