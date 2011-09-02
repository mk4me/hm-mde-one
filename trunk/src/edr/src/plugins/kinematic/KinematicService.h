#ifndef __HEADER_GUARD__KINEMATICSERVICE_H__
#define __HEADER_GUARD__KINEMATICSERVICE_H__

#include <QtCore/QTimer>
#include <core/IService.h>

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
//    std::vector<timeline::StreamPtr> stream;
};


#endif  // __HEADER_GUARD__KINEMATICSERVICE_H__