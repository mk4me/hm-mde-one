#ifndef __HEADER_GUARD__KINEMATICSERVICE_H__
#define __HEADER_GUARD__KINEMATICSERVICE_H__

#include <QtCore/QTimer>
#include <core/IService.h>
#include <plugins/timeline/Stream.h>

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
    std::vector<timeline::StreamPtr> stream;
};


#endif  // __HEADER_GUARD__KINEMATICSERVICE_H__