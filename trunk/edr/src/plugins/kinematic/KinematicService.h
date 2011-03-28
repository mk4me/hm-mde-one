#ifndef __HEADER_GUARD__KINEMATICSERVICE_H__
#define __HEADER_GUARD__KINEMATICSERVICE_H__

#include <QtCore/QTimer>
#include <core/IService.h>
#include <plugins/timeline/Stream.h>
#include <core/IVisualizer.h>
#include <osg/Geode>

class IModel;
class IWidget;


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
    virtual QWidget* createWidget();
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



class KinematicService : public IService
{
    UNIQUE_ID('KINE','SRVC');

public:
    KinematicService();

    virtual IWidget* getSettingsWidget()
    {
        return reinterpret_cast<IWidget*>(widget); 
    }

    virtual IWidget* getWidget() 
    { 
        return reinterpret_cast<IWidget*>(nullptr);        
    }
    virtual const std::string& getName() const
    {
        return name;
    }
   
    virtual AsyncResult loadData(IServiceManager* serviceManager, core::IDataManager* dataManager);

    virtual AsyncResult update(double time, double timeDelta);

private:
    std::string name;
    MainWindow* widget;
    SkeletonViewerLogicPtr logic;
    //KinematicVisualizer viz;
};


#endif  // __HEADER_GUARD__KINEMATICSERVICE_H__