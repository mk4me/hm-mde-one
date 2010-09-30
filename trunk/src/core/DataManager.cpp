#include "DataManager.h"
#include <tinyxml.h>



//--------------------------------------------------------------------------------------------------
DataManager::DataManager(std::string address)
{
    std::string add(address.begin(), address.end());
    TiXmlDocument doc(add.c_str());

    // load file
    if (doc.LoadFile())
    {
        // get root
        TiXmlNode* node = doc.RootElement();

        // is it fmodel at all?
        if (node && !node->ValueStr().compare("fmd") && node->FirstChild())
        {
            node = node->FirstChild();

            do 
            {
                if(!node->ValueStr().compare("VideoList")&& node->FirstChild())
                {
                    TiXmlNode* video = node->FirstChild();
                    if (!video->ValueStr().compare("Video"))
                    {
                        // get all materials
                        do 
                        {
                            // get all attributes
                            TiXmlAttribute* att = video->ToElement()->FirstAttribute();
                            if (att)
                            {
                                // material attributes
                                do
                                {
                                    if (!att->NameTStr().compare("FileName"))
                                    {
                                        std::string val = att->Value();
                                        m_VideoFilePathList.push_back(std::string(val.begin(), val.end()));
                                    }
                                    else
                                        return;		

                                } 
                                while(att = att->Next());
                            }
                            else return;
                        } 
                        while (video = video->NextSibling());
                    } 
                }
            } 
            while (node = node->NextSibling());
        }	
    }
}


//--------------------------------------------------------------------------------------------------
DataManager::~DataManager()
{
    Clear();
}

//--------------------------------------------------------------------------------------------------
void DataManager::Clear()
{
    m_VideoFilePathList.clear();
}

//--------------------------------------------------------------------------------------------------
void DataManager::GetVideoFile()
{

}

//--------------------------------------------------------------------------------------------------
std::string DataManager::GetVideoFilePath(int i)
{
    if(m_VideoFilePathList.size() > i)
        return m_VideoFilePathList[i];

    return "ERROR";
}

//--------------------------------------------------------------------------------------------------
int DataManager::GetFilePathCount()
{
    return m_VideoFilePathList.size();
}