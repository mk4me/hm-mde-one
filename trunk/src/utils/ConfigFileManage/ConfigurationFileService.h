#ifndef CONFIGURATIONFILE_SERVICE_H
#define CONFIGURATIONFILE_SERVICE_H

#include "GlobalServicesIDs.h"
#include "SimpleFunctors.h"

#include "IConfigManager.h"
#include "ConfigBuffer.h"
#include "ConfigurationGroup.h"
#include "FileReader.h"

//--------------------------------------------------------------------------------------------------
// ConfigurationFile Service
//--------------------------------------------------------------------------------------------------
class ConfigurationFileService: public IBaseService, public IConfigManager
{
public:

    ConfigurationFileService();
    /// Virtual destructor that destroys the Configuration Service and releases all references to
    /// any messages and targets that may still exist.
    virtual ~ConfigurationFileService();


    // update
    virtual AsyncResult OnTick(double delta); 
    // 
    virtual AsyncResult OnAdded(); 

    virtual bool IsTrue(const std::string& strName, bool defaultValue = false) const;
    virtual bool IsFalse(const std::string& strName, bool defaultValue = true) const;


    bool LoadConfig(std::string fileName);
    int GetIntParameter(std::string pName);
    float GetFloatParameter(std::string pName);

    void SetParameter(std::string cGroupName, std::string pName, std::string pValue);
    void SetParameter(std::string pName, std::string pValue);

    void AddNewConfigurationGroup(std::string name);
    void AddNewConfigurationGroup(ConfigurationGroup& cGroup);
    void AddNewConfigurationGroup(ConfigurationGroup *cGroup);

    std::string trim(std::string str);

    ConfigurationGroup* GetConfigurationGroup(std::string groupName);
    std::list<ConfigurationGroup*>& GetConfigurationGroupList();
    void Save();
    void ClearBuffer();

protected: 
    // Overridden virtual functions inherit base documentation and thus are not documented here.
    // virtual void AddConfigSourceInternal(IConfigSource *pSource);

private: 
    M_DECLARE_CLASS(); 

    void CheckLine(std::string line);

    std::string m_configFileName;
    bool m_isConfigFileLoad;

    FileReader *m_File;

    //  std::ifstream m_File;
    ConfigBuffer *m_buffer;

    bool isConfigurationGroup;

};

#endif //CONFIGURATIONFILE_SERVICE_H