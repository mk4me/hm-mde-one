#include "Factory.h"

#include <core/IModel.h>
#include <core/IC3DModel.h>

#include "Model.h"
#include "C3DModel.h"
#include "FileReader2Motion.h"
#include "ASFAMCParser.h"


//--------------------------------------------------------------------------------------------------
IModel* Factor::CreateModel(std::string pathToMesh, std::string pathToASF, std::string pathToAMC)
{
    // TODO sprawdzenie wycieku pamiêci.

//     ASFAMCParser* object = new ASFAMCParser();
// 
//     if(!pathToASF.empty())
//     {
//         if(object->ReadASFFile(pathToASF))
//             FileReader2Motion->ParserAcclaimFile2EDR(NULL, object);
// 
//         for(int i = 0; i < dataManager->GetAnimationFilePathCount(); i++)
//         {
//             if(object->ReadAMCFile(dataManager->GetAnimationFilePath(i)))
//                 LoadAnimationFromAcclaim(dataManager->GetAnimationFilePath(i), object, dynamic_cast<Model*>(dataManager->GetModel()));
//         }
//     }
// 
// 
//     if(dataManager->GetMeshFilePathCount() > 0)
//     {
//         LoadMesh(dataManager->GetMeshFilePathPath(0), dynamic_cast<Model* >(dataManager->GetModel()));
//         dataManager->GetModel()->InicializeMesh();
//     }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
IC3DModel* Factor::CreateC3DModel(std::string pathToC3DModel)
{
//     for (int i = 0; i < dataManager->GetC3dFilePathCount(); i++)
//     {
//         C3D_Data *c3d = ReadC3DFile(dataManager->GetC3dFilePath(i));
// 
//         if(c3d)
//         {
//             C3DModel* c3dModel = new C3DModel();
//             std::string name = dataManager->GetC3dFilePath(i);
// 
//             c3dModel->SetName(name.substr(name.find_last_of("/")+1, name.length()));
//             ParseC3DFile2EDR(c3d, c3dModel);
//             dataManager->AddC3DModel(c3dModel);
//         }
//     }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
IModel* Factor::GetModel(std::string pathToMesh, std::string pathToASF, std::string pathToAMC)
{
    std::string modelPath = pathToMesh + pathToASF + pathToAMC;

    std::map<std::string, IModel* >::iterator i = m_ModelMap.find(modelPath);

    if (i != m_ModelMap.end()) {
        return i->second;
    } else {
        IModel* newModel = CreateModel(pathToMesh, pathToASF, pathToAMC);
        m_ModelMap.insert(make_pair(modelPath, newModel));

        return newModel;
    }
}

//--------------------------------------------------------------------------------------------------
IC3DModel* Factor::GetC3DModel(std::string c3dObjectPath)
{
    std::map<std::string, IC3DModel* >::iterator i = m_C3DModelMap.find(c3dObjectPath);

    if (i != m_C3DModelMap.end()) {
        return i->second;
    } else {
        IC3DModel* newC3DModel = CreateC3DModel(c3dObjectPath);
        m_C3DModelMap.insert(make_pair(c3dObjectPath, newC3DModel));

        return newC3DModel;
    }
}