#include "Factory.h"

#include <core/IModel.h>
#include <core/IC3DModel.h>

#include "Model.h"
#include "C3DModel.h"
#include "FileReader2Motion.h"
#include "ASFAMCParser.h"


//--------------------------------------------------------------------------------------------------
IModel* Factor::CreateModel(std::string pathToMesh, std::string pathToASF, std::vector<std::string> pathToAMCList)
{
    IModel* model;

    model = FileReader2Motion::GetModelFromResources(pathToMesh, pathToASF, pathToAMCList);

    return model;
}

//--------------------------------------------------------------------------------------------------
IC3DModel* Factor::CreateC3DModel(std::string pathToC3DModel)
{
    IC3DModel* c3dModel;

    c3dModel = FileReader2Motion::GetC3DModelFromResources(pathToC3DModel);

    return c3dModel;
}

//--------------------------------------------------------------------------------------------------
IModel* Factor::GetModel(std::string pathToMesh, std::string pathToASF, std::vector<std::string> pathToAMCList)
{
    if(pathToASF.empty())
        return NULL;

    std::string modelPath;

    if(pathToAMCList.size() > 0)
        modelPath = pathToMesh + pathToASF + pathToAMCList[0];
    else
        modelPath = pathToMesh + pathToASF;

    std::map<std::string, IModel* >::iterator i = m_ModelMap.find(modelPath);

    if (i != m_ModelMap.end()) {
        return i->second;
    } else {
        IModel* newModel = CreateModel(pathToMesh, pathToASF, pathToAMCList);
        m_ModelMap.insert(make_pair(modelPath, newModel));

        return newModel;
    }
}

//--------------------------------------------------------------------------------------------------
IC3DModel* Factor::GetC3DModel(std::string c3dObjectPath)
{
    if(c3dObjectPath.empty())
        return NULL;

    std::map<std::string, IC3DModel* >::iterator i = m_C3DModelMap.find(c3dObjectPath);

    if (i != m_C3DModelMap.end()) {
        return i->second;
    } else {
        IC3DModel* newC3DModel = CreateC3DModel(c3dObjectPath);
        m_C3DModelMap.insert(make_pair(c3dObjectPath, newC3DModel));

        return newC3DModel;
    }
}

//--------------------------------------------------------------------------------------------------
void Factor::Clear()
{
    m_ModelMap.clear();
    m_C3DModelMap.clear();
}