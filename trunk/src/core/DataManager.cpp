#include "DataManager.h"

#include <core/IModel.h>
#include <tinyxml.h>



// TODO: z optymalizowaæ kod.
//--------------------------------------------------------------------------------------------------
DataManager::DataManager(std::string address, IModel* model, IC3DModel* c3dModel)
{
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
	clear();
	loadResources();
}


//--------------------------------------------------------------------------------------------------
DataManager::~DataManager()
{
	this->Clear();
    this->clear();
}

//--------------------------------------------------------------------------------------------------
void DataManager::Clear()
{
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
IC3DModel* DataManager::GetC3DModel(int i)
{
    if(m_pC3dModelList.size() > i)
        return m_pC3dModelList[i];

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
int DataManager::GetC3DModelCount()
{
    return m_pC3dModelList.size();
}

//--------------------------------------------------------------------------------------------------
void DataManager::AddC3DModel( IC3DModel *c3dmodel )
{
    m_pC3dModelList.push_back(c3dmodel);
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


DataManager::DataManager(const std::string& meshesDir, const std::string& shadersDir, const std::string& trialsDir) : shadersDir(shadersDir), meshesDir(meshesDir), trialsDir(trialsDir)
{
	setDirSlashes(this->shadersDir);
	setDirSlashes(this->meshesDir);
	setDirSlashes(this->trialsDir);
	clear();
	loadResources();
}

void DataManager::loadResources()
{
#if defined(__WIN32__)
	HANDLE file;
	WIN32_FIND_DATA data_find;
	bool more_files = true;

	//szukaj shaderow
	std::string file_mask = this->shadersDir;
	file_mask.append("*.frag");

	file = ::FindFirstFile(file_mask.c_str(), &data_find);
	while(file != INVALID_HANDLE_VALUE && more_files)
	{
		std::string file_path(this->shadersDir);
		file_path.append(data_find.cFileName);
		this->shadersPaths[data_find.cFileName] = file_path;
		// czy dalej? (dziwna postaæ ¿eby pozbyæ siê warninga)
		more_files = (::FindNextFile(file, &data_find) == BOOL(TRUE));
	}

	//szukaj meshy
	more_files = true;
	file_mask = this->meshesDir;
	file_mask.append("*.tbs");

	file = ::FindFirstFile(file_mask.c_str(), &data_find);
	while(file != INVALID_HANDLE_VALUE && more_files)
	{
		std::string file_path(this->meshesDir);
		file_path.append(data_find.cFileName);
		this->meshesPaths[data_find.cFileName] = file_path;
		more_files = (::FindNextFile(file, &data_find) == BOOL(TRUE));
	}

	//przeszukaj foldery trialowe
	more_files = true;
	file_mask = this->trialsDir;
	file_mask.append("*");
	std::vector<std::string> trial_dirs;

	file = ::FindFirstFile(file_mask.c_str(), &data_find);
	while(file != INVALID_HANDLE_VALUE && more_files)
	{
		if(data_find.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && data_find.cFileName[0] != '.')
		{
			trial_dirs.push_back(data_find.cFileName);
		}
		more_files = (::FindNextFile(file, &data_find) == BOOL(TRUE));
	}

	for(std::vector<std::string>::iterator it = trial_dirs.begin(); it != trial_dirs.end(); ++it)
	{
		//szukaj video
		more_files = true;
		file_mask = this->trialsDir;
		file_mask.append((*it)).append("/*.avi");

		file = ::FindFirstFile(file_mask.c_str(), &data_find);
		while(file != INVALID_HANDLE_VALUE && more_files)
		{
			std::string file_path(this->trialsDir);
			file_path.append((*it)).append("/").append(data_find.cFileName);
			this->videosPaths[data_find.cFileName] = file_path;
			more_files = (::FindNextFile(file, &data_find) == BOOL(TRUE));
		}
	}

	for(std::vector<std::string>::iterator it = trial_dirs.begin(); it != trial_dirs.end(); ++it)
	{
		//szukaj c3d
		more_files = true;
		file_mask = this->trialsDir;
		file_mask.append((*it)).append("/*.c3d");

		file = ::FindFirstFile(file_mask.c_str(), &data_find);
		while(file != INVALID_HANDLE_VALUE && more_files)
		{
			std::string file_path(this->trialsDir);
			file_path.append((*it)).append("/").append(data_find.cFileName);
			this->c3dsPaths[data_find.cFileName] = file_path;
			more_files = (::FindNextFile(file, &data_find) == BOOL(TRUE));
		}
	}
#elif defined(__UNIX__)
	//FIX: nie znam api linuksa, na te chwile niech dziala w win
#endif
}

void DataManager::setDirSlashes(std::string& dir)
{
	if(dir[dir.size() - 1] != '/')
	{
		dir.append("/");
	}
}

void DataManager::clear() {
	this->shadersPaths.clear();
	this->meshesPaths.clear();
	this->videosPaths.clear();
	this->c3dsPaths.clear();

//    m_VideoFilePathList.clear();
    m_SkeletonFilePathList.clear();
    m_AnimationFilePathList.clear();
}

const std::string& DataManager::getShader(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->shadersPaths.find(name);
	if(it != this->shadersPaths.end())
	{
		return (*it).second;
	}
	throw std::ios_base::failure("Shader not found.");
}

const std::map<std::string, std::string>& DataManager::getShaders()
{
	return this->shadersPaths;
}

const std::string& DataManager::getMesh(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->meshesPaths.find(name);
	if(it != this->meshesPaths.end())
	{
		return (*it).second;
	}
	throw std::ios_base::failure("Mesh not found.");
}

const std::map<std::string, std::string>& DataManager::getMeshes()
{
	return this->meshesPaths;
}

const std::string& DataManager::getVideo(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->videosPaths.find(name);
	if(it != this->videosPaths.end())
	{
		return (*it).second;
	}
	throw std::ios_base::failure("Video not found.");
}

const std::map<std::string, std::string>& DataManager::getVideos()
{
	return this->videosPaths;
}

const std::string& DataManager::getC3D(const std::string& name)
{
	std::map<std::string, std::string>::iterator it = this->c3dsPaths.find(name);
	if(it != this->c3dsPaths.end())
	{
		return (*it).second;
	}
	throw std::ios_base::failure("C3D file not found.");
}

const std::map<std::string, std::string>& DataManager::getC3Ds()
{
	return this->c3dsPaths;
}
