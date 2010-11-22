#include "DataManager.h"

#include <core/IModel.h>
#include <tinyxml.h>



// TODO: z optymalizowaæ kod.
//--------------------------------------------------------------------------------------------------
DataManager::DataManager(std::string address, IModel* model, IC3DModel* c3dModel)
{
    m_pC3dModel = c3dModel;
    m_pFileName = address;
    m_pModel = model;

    std::string add(address.begin(), address.end());
    TiXmlDocument doc(add.c_str());

    std::string path = address.substr(0, address.find_last_of("/")+1);

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
                                        std::string val = path + att->Value();
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
                if(!node->ValueStr().compare("SkeletonList")&& node->FirstChild())
                {
                    TiXmlNode* skeleton = node->FirstChild();
                    if (!skeleton->ValueStr().compare("Skeleton"))
                    {
                        // get all materials
                        do 
                        {
                            // get all attributes
                            TiXmlAttribute* att = skeleton->ToElement()->FirstAttribute();
                            if (att)
                            {
                                // material attributes
                                do
                                {
                                    if (!att->NameTStr().compare("FileName"))
                                    {
                                        std::string val = path + att->Value();
                                        m_SkeletonFilePathList.push_back(std::string(val.begin(), val.end()));
                                    }
                                    else
                                        return;		

                                } 
                                while(att = att->Next());
                            }
                            else return;
                        } 
                        while (skeleton = skeleton->NextSibling());
                    }
                }
                if(!node->ValueStr().compare("AnimationList")&& node->FirstChild())
                {
                    TiXmlNode* animation = node->FirstChild();
                    if (!animation->ValueStr().compare("Animation"))
                    {
                        // get all materials
                        do 
                        {
                            // get all attributes
                            TiXmlAttribute* att = animation->ToElement()->FirstAttribute();
                            if (att)
                            {
                                // material attributes
                                do
                                {
                                    if (!att->NameTStr().compare("FileName"))
                                    {
                                        std::string val = path + att->Value();
                                        m_AnimationFilePathList.push_back(std::string(val.begin(), val.end()));
                                    }
                                    else
                                        return;		

                                } 
                                while(att = att->Next());
                            }
                            else return;
                        } 
                        while (animation = animation->NextSibling());
                    }
                }
                if(!node->ValueStr().compare("Model") && !node->FirstChild())
                {
                    // get all attributes
                    TiXmlAttribute* att = node->ToElement()->FirstAttribute();
                    if (att)
                    {
                        // material attributes
                        do
                        {
                            if (!att->NameTStr().compare("FileName"))
                            {
                                std::string val = path + att->Value();
                                m_MeshFilePathList.push_back(std::string(val.begin(), val.end()));
                            }
                            else
                                return;		

                        } 
                        while(att = att->Next());
                    }

                }
                if(!node->ValueStr().compare("c3dFileList")&& node->FirstChild())
                {
                    TiXmlNode* c3dFile = node->FirstChild();
                    if (!c3dFile->ValueStr().compare("File"))
                    {
                        // get all materials
                        do 
                        {
                            // get all attributes
                            TiXmlAttribute* att = c3dFile->ToElement()->FirstAttribute();
                            if (att)
                            {
                                // material attributes
                                do
                                {
                                    if (!att->NameTStr().compare("FileName"))
                                    {
                                        std::string val = path + att->Value();
                                        m_c3dFilePathList.push_back(std::string(val.begin(), val.end()));
                                    }
                                    else
                                        return;		

                                } 
                                while(att = att->Next());
                            }
                            else return;
                        } 
                        while (c3dFile = c3dFile->NextSibling());
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
    m_SkeletonFilePathList.clear();
    m_AnimationFilePathList.clear();
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
int DataManager::GetVideoFilePathCount()
{
    return m_VideoFilePathList.size();
}

//--------------------------------------------------------------------------------------------------
IModel* DataManager::GetModel()
{
    if(m_pModel)
        return m_pModel;

    return NULL;
}

//--------------------------------------------------------------------------------------------------
IC3DModel* DataManager::GetC3DModel()
{
    if(m_pC3dModel)
        return m_pC3dModel;

    return NULL;
}

//--------------------------------------------------------------------------------------------------
std::string DataManager::GetFileName()
{
    return m_pFileName;
}

//--------------------------------------------------------------------------------------------------
std::string DataManager::GetSkeletonFilePath( int i )
{
    if(m_SkeletonFilePathList.size() > i)
        return m_SkeletonFilePathList[i];

    return NULL;
}

//--------------------------------------------------------------------------------------------------
int DataManager::GetSkeletonFilePathCount()
{
    return m_SkeletonFilePathList.size();
}

//--------------------------------------------------------------------------------------------------
std::string DataManager::GetAnimationFilePath( int i )
{
    if(m_AnimationFilePathList.size() > i)
        return m_AnimationFilePathList[i];

    return NULL;
}

//--------------------------------------------------------------------------------------------------
int DataManager::GetAnimationFilePathCount()
{
    return m_AnimationFilePathList.size();
}

//--------------------------------------------------------------------------------------------------
std::string DataManager::GetMeshFilePathPath( int i )
{
    if(m_MeshFilePathList.size() > i)
        return m_MeshFilePathList[i];

    return NULL;
}

//--------------------------------------------------------------------------------------------------
int DataManager::GetMeshFilePathCount()
{
    return m_MeshFilePathList.size();
}

//--------------------------------------------------------------------------------------------------
std::string DataManager::GetC3dFilePath( int i )
{
    if(m_c3dFilePathList.size() > i)
        return m_c3dFilePathList[i];

    return NULL;
}

//--------------------------------------------------------------------------------------------------
int DataManager::GetC3dFilePathCount()
{
    return m_c3dFilePathList.size();
}
