#ifndef CONFIGURATIONFILE_SERVICE_H
#define CONFIGURATIONFILE_SERVICE_H

#include <core/SimpleFunctors.h>

#include "IConfigManager.h"
#include "ConfigBuffer.h"
#include "ConfigurationGroup.h"
#include "../ServiceManager.h"
#include "FileReader.h"

//--------------------------------------------------------------------------------------------------
// ConfigurationFile Service
//--------------------------------------------------------------------------------------------------
class ConfigurationFileService: public core::IService, public IConfigManager
{
public:

    ConfigurationFileService();

    /// Virtual destructor that destroys the Configuration Service and releases all references to
    /// any messages and targets that may still exist.
    virtual ~ConfigurationFileService();
    virtual void OnTick(double delta); 
    virtual void OnAdded(core::IServiceManager* serviceManager); 

    virtual bool IsTrue(const std::string& strName, bool defaultValue = false) const;
    virtual bool IsFalse(const std::string& strName, bool defaultValue = true) const;

    void SetParameter(std::string cGroupName, std::string pName, std::string pValue);
    void SetParameter(std::string pName, std::string pValue);

    void AddNewConfigurationGroup(std::string name);
    void AddNewConfigurationGroup(ConfigurationGroup& cGroup);
    void AddNewConfigurationGroup(ConfigurationGroup *cGroup);
    void ClearBuffer();
    void Save();

    bool LoadConfig(std::string fileName);
    int GetIntParameter(std::string pName);
    float GetFloatParameter(std::string pName);



    std::string trim(std::string str);
    ConfigurationGroup* GetConfigurationGroup(std::string groupName);
    std::list<ConfigurationGroup*>& GetConfigurationGroupList();

private: 
    void CheckLine(std::string line);
    bool m_isConfigFileLoad;
    bool isConfigurationGroup;

    std::string m_configFileName;
    FileReader *m_File;

    //  std::ifstream m_File;
    ConfigBuffer *m_buffer;

    ServiceManager* m_pServiceManager;
};

#endif //CONFIGURATIONFILE_SERVICE_H