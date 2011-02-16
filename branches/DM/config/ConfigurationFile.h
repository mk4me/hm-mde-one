#pragma once

#ifndef CONFIGURATIONFILE_H
#define CONFIGURATIONFILE_H

#include "IConfigManager.h"
#include "ConfigBuffer.h"
#include "ConfigurationGroup.h"
#include "FileReader.h"


class  ConfigurationFile : public IConfigManager
{

public:

    /// Virtual destructor that destroys the Configuration Service and releases all references to
    /// any messages and targets that may still exist.
    virtual ~ConfigurationFile();


    static ConfigurationFile &getInstance(){
        static ConfigurationFile my_configManager;
        return my_configManager;
    }


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
    ConfigurationFile();
    ConfigurationFile(const ConfigurationFile &ConfManager);
    ConfigurationFile &operator = (const ConfigurationFile &ConfManager);
    void CheckLine(std::string line);

    std::string m_configFileName;
    bool m_isConfigFileLoad;

    FileReader *m_File;

  //  std::ifstream m_File;
    ConfigBuffer *m_buffer;

    bool isConfigurationGroup;
};


#endif //CONFIGURATIONFILE_H




