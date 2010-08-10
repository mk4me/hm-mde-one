#include "ConfigurationFileService.h"

#include <algorithm>
#include <stdlib.h>


#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

M_DECLARED_CLASS(ConfigurationFileService, kCLASSID_ConfigurationFileService);

// to mo¿e  nie jest pamiêciowo optymalne ale dzia³a - TODO - siê zoptymaliuje potem.
// chwilowo nie u¿ywane 

/** Macro TRIM_CMP( x ) - checks if char is ' ' or '\t' */
#define TRIM_CMP( x ) ( 0[x] == ' ' || 0[x] == '\t' || 0[x] == 0 )

// void char_trim( char * sz_string, char ** pp_output ) {
//
//    /** Length of string */
//    const int len = strlen( sz_string );
//
//    /** Pointers for left and right side of string */
//    char * p_left   = sz_string,
//        * p_right  = (char*)( sz_string + len );
//
//    /** Calculate p_left and p_right */
//    while ( p_left < p_right &&
//        ( TRIM_CMP( p_left += ( TRIM_CMP( p_left ) ? 1 : 0 ) ) ||
//        TRIM_CMP( p_right -= ( TRIM_CMP( p_right) ? 1 : 0 ) ) ) );
//
//    /** Allocate and copy output string */
//    *pp_output = (char*)malloc( p_right <= p_left ? 0 : p_right - p_left + 2 );
//    strncpy( *pp_output, p_left, p_right <= p_left ? 0 : p_right - p_left + 1 );
//}

//------------------------------------------------------------------------------------------------
std::string ConfigurationFileService::trim(std::string str) 
{
    while(str.at(0) == ' ' || str.at(0) == '\t' || str.at(0) == '\n') 
        str = str.substr(1);
    while(str.at(str.length() - 1) == ' ' || str.at(str.length() - 1) == '\t' || str.at(str.length() - 1) == '\n') 
        str = str.substr(0, str.length()-1);
    return str;
}

//------------------------------------------------------------------------------------------------
ConfigurationFileService::ConfigurationFileService()
{
    /**
    Constructor that creates the Configuration Service and creates the default command-line
    and default INI file sources.

    @param configFile Optional path and file name of the config file.
    @param argcInit Number of command line arguments passed to the application.
    @param argvInit Array of strings containing each of the command line arguments.
    @param processEnvVars True to create an Environment Variables source (EnvVarSource) and
    read in all the environment variables into the configuration.
    */

    //   ConfigManager(
    //       const std::string& configFile,
    //       int argcInit,
    //       char **argvInit,
    //       bool processEnvVars = false);

    m_configFileName = "";
    m_isConfigFileLoad = false;

    m_File = NULL;
    m_buffer = new ConfigBuffer();
}

//------------------------------------------------------------------------------------------------
AsyncResult ConfigurationFileService::OnTick( double delta )
{

    return AsyncResult_Complete; 
}

//------------------------------------------------------------------------------------------------
AsyncResult ConfigurationFileService::OnAdded()
{

    std::cout << "ConfigurationFileService ADDED!" << std::endl; 
    return AsyncResult_Complete; 
}

//------------------------------------------------------------------------------------------------
ConfigurationFileService::~ConfigurationFileService()
{
    m_File = NULL;
    ClearBuffer();
    m_buffer = NULL;
}

//------------------------------------------------------------------------------------------------
int FileExists (const char* fileName)
{
    /* Sprawdzenie dostêpu do pliku (je¿eli takowy istnieje, musi istnieæ plik) */
#ifdef WIN32
    return !access(fileName, 0);    /* w przypadku Win32 */
#else
    return !access(fileName, F_OK);    /* w przypadku UNIX'a */
#endif
}

//------------------------------------------------------------------------------------------------
bool ConfigurationFileService::LoadConfig(std::string fileName)
{
    if(!FileExists(fileName.c_str()))
        return false;

    ClearBuffer();
    m_File = new FileReader(fileName.c_str());
    std::string line;

    while(!m_File->getFile()->eof())
    {
        *m_File >> line;
        CheckLine(line);

        line.clear();
    }


    m_File->Close();
    delete m_File;
    m_File = 0;

    m_configFileName = fileName;
    m_isConfigFileLoad = true;

    return true;
}

//------------------------------------------------------------------------------------------------
int ConfigurationFileService::GetIntParameter(std::string pName)
{
    std::list<ConfigurationGroup*> cGroup = m_buffer->GetConfigurationGroupList();
    std::string pValue = "";
    for(std::list<ConfigurationGroup*>::iterator it = cGroup.begin(); it != cGroup.end(); it++)
    {
        if((*it)->GetParametrVal(pName, pValue))
        {
            return atoi(pValue.c_str());
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------------------------
float ConfigurationFileService::GetFloatParameter(std::string pName)
{
    std::list<ConfigurationGroup*> cGroup = m_buffer->GetConfigurationGroupList();
    std::string pValue = "";
    for(std::list<ConfigurationGroup*>::iterator it = cGroup.begin(); it != cGroup.end(); it++)
    {
        if((*it)->GetParametrVal(pName, pValue))
        {
            return strtod(pValue.c_str(), NULL);
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::SetParameter(std::string cGroupName, std::string pName, std::string pValue)
{
    std::list<ConfigurationGroup*> cGroup = m_buffer->GetConfigurationGroupList();
    for(std::list<ConfigurationGroup*>::iterator it = cGroup.begin(); it != cGroup.end(); it++)
    {
        if((*it)->GetName() == cGroupName)
        {
            (*it)->SetParametrVal(pName,pValue);
        }
    }
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::SetParameter( std::string pName, std::string pValue )
{
    std::list<ConfigurationGroup*> cGroup = m_buffer->GetConfigurationGroupList();
    for(std::list<ConfigurationGroup*>::iterator it = cGroup.begin(); it != cGroup.end(); it++)
    {
        if((*it)->IsExistParameter(pName))
        {
            (*it)->SetParametrVal(pName, pValue);
        }
    }
}
//------------------------------------------------------------------------------------------------
ConfigurationGroup* ConfigurationFileService::GetConfigurationGroup( std::string groupName )
{
    std::list<ConfigurationGroup*> cGroup = m_buffer->GetConfigurationGroupList();
    for(std::list<ConfigurationGroup*>::iterator it = cGroup.begin(); it != cGroup.end(); it++)
    {
        if((*it)->GetName() == groupName)
        {
            return (*it);
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::Save()
{
    if(m_configFileName == "")
        return;

    isConfigurationGroup = false;

    std::list<ConfigurationGroup*> cGroupList = m_buffer->GetConfigurationGroupList();
    std::list<Parameter*> parameterList;

    fstream stream_writer;

    if(FileExists(m_configFileName.c_str()))
    {
        m_File = new FileReader(m_configFileName.c_str());
        std::string streamBuff = "";
        std::string line;

        while(!m_File->getFile()->eof())
        {
            *m_File >> line;

            if(line.find_first_of(kSemicolonConfigurationGroupChar) != 0xFFFFFFFF || line.empty())
            {
                // stream_writer << line << endl;
                streamBuff += line + "\n";
                continue;
            }

            static std::string configurationGroupName;

            int indexStart = line.find_first_of(kOpenConfigurationGroupChar);
            if(indexStart != 0xFFFFFFFF) 
            {
                indexStart++;
                int indexEnd = line.find_first_of(kCloseConfigurationGroupChar);
                configurationGroupName = line.substr(indexStart, (indexEnd - indexStart));

                //stream_writer << line <<endl;
                streamBuff += line + "\n";
                isConfigurationGroup = true;

            }
            else if((line.find_first_of(kSemicolonConfigurationGroupChar) == 0xFFFFFFFF && line.length()>1) && (isConfigurationGroup))
            {
                int indexEnd = line.find_first_of(kNameValueSeperator);

                std::string pName = trim(line.substr(0, indexEnd));
                std::string pValue = trim(line.substr(indexEnd+1, line.length()-1));

                for(std::list<ConfigurationGroup*>::iterator it = cGroupList.begin(); it != cGroupList.end(); it++)
                {
                    if((*it)->GetName() == configurationGroupName)
                    {
                        parameterList = (*it)->GetParametersList();

                        for(std::list<Parameter*>::iterator it = parameterList.begin(); it != parameterList.end(); it++)
                        {
                            // stream_writer << (*it)->GetName() + " = " + (*it)->GetValue() << endl;
                            streamBuff += (*it)->GetName() + " = " + (*it)->GetValue() + "\n";
                            isConfigurationGroup = false;
                        }

                        cGroupList.erase(it);
                        break;
                    }
                }
            }
            line.clear();
        }

        if(cGroupList.size() > 0)
        {
            for(std::list<ConfigurationGroup*>::iterator it = cGroupList.begin(); it != cGroupList.end(); it++)
            {
                // stream_writer << "[" + (*it)->GetName() + "]" << endl;
                streamBuff += "[" + (*it)->GetName() + "]" + "\n";

                std::list<Parameter*> parameterList = (*it)->GetParametersList();
                for(std::list<Parameter*>::iterator it = parameterList.begin(); it != parameterList.end(); it++)
                {
                    // stream_writer << (*it)->GetName() + " = " + (*it)->GetValue() << endl;
                    streamBuff += (*it)->GetName() + " = " + (*it)->GetValue() + "\n";
                }
                // stream_writer << endl;
                streamBuff += "\n";
            }
        }
        m_File->Close();
        delete m_File;
        m_File = 0;

        stream_writer.open(m_configFileName.c_str(),ios::out);
        stream_writer << streamBuff;
        stream_writer.close();
        streamBuff = "";
    }
    else
    {
        stream_writer.open(m_configFileName.c_str(),ios::out);
        std::list<ConfigurationGroup*> cGroupList = m_buffer->GetConfigurationGroupList();
        for(std::list<ConfigurationGroup*>::iterator it = cGroupList.begin(); it != cGroupList.end(); it++)
        {
            stream_writer << "[" + (*it)->GetName() + "]" << endl;

            std::list<Parameter*> parameterList = (*it)->GetParametersList();
            for(std::list<Parameter*>::iterator it = parameterList.begin(); it != parameterList.end(); it++)
            {
                stream_writer << (*it)->GetName() + " = " + (*it)->GetValue() << endl;
            }
            stream_writer << endl;
        }

        stream_writer.close();
    }
    m_buffer->Clear();
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::CheckLine(std::string line)
{
    //   isConfigurationGroup = false;
    static std::string configurationGroupName;

    int indexStart = line.find_first_of(kOpenConfigurationGroupChar);
    if(indexStart != 0xFFFFFFFF) 
    {
        indexStart++;
        int indexEnd = line.find_first_of(kCloseConfigurationGroupChar);
        configurationGroupName = line.substr(indexStart, (indexEnd - indexStart));
        m_buffer->AddNewConfigurationGroup(configurationGroupName);
    }
    else if(line.find_first_of(kSemicolonConfigurationGroupChar) == 0xFFFFFFFF && line.length()>1)
    {
        int indexEnd = line.find_first_of(kNameValueSeperator);

        std::string pName = trim(line.substr(0, indexEnd));
        std::string pValue = trim(line.substr(indexEnd+1, line.length()-1));

        m_buffer->GetConfigurationGroup(configurationGroupName)->AddNewParametr(pName, pValue);
    }
}

//------------------------------------------------------------------------------------------------
std::list<ConfigurationGroup*>& ConfigurationFileService::GetConfigurationGroupList()
{
    return m_buffer->GetConfigurationGroupList();
}

//------------------------------------------------------------------------------------------------
bool ConfigurationFileService::IsTrue( const std::string& strName, bool defaultValue /*= false*/ ) const
{
    return true;
}

//------------------------------------------------------------------------------------------------
bool ConfigurationFileService::IsFalse( const std::string& strName, bool defaultValue /*= true*/ ) const
{
    return true;
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::AddNewConfigurationGroup(std::string name)
{
    ConfigurationGroup* cGroup = new ConfigurationGroup(name);

    m_buffer->AddNewConfigurationGroup(*cGroup);
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::AddNewConfigurationGroup(ConfigurationGroup& cGroup)
{
    m_buffer->AddNewConfigurationGroup(cGroup);
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::AddNewConfigurationGroup( ConfigurationGroup *cGroup )
{
    m_buffer->AddNewConfigurationGroup(cGroup);
}

//------------------------------------------------------------------------------------------------
void ConfigurationFileService::ClearBuffer()
{
    m_buffer->Clear();
}
