#include "StdAfx.h"

FModel::FModel(void)
{
}

FModel::~FModel(void)
{
}

void FModel::Save( fm::string fileName, fm::string outputDir, VisualScene *scene, FMesh *mesh, FAnimation *animation )
{
  fm::string animationFileName = fileName;
  animationFileName.append(".fanimation");

  fileName.append(".tbs"); 
  fm::map<size_t, size_t> meshMaterialPairMap; 

  CCoInitialize init;   //must be declared before any IXMLDOM objects
  CComPtr<IXMLDOMDocument>  pXMLDoc;
  CComPtr<IXMLDOMNode> pRoot;	
  CComPtr<IXMLDOMNode> fmdNode;
  CComPtr<IXMLDOMNode> modelNode;
  CComPtr<IXMLDOMNode> materialListNode;
  CComPtr<IXMLDOMNode> materialSetListNode;

  bool succeeded = SUCCEEDED(init) && SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pXMLDoc));

  pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pRoot);

  fm::string buf;

  struct tm *newtime = new tm();
  time_t aclock;
  time( &aclock );
  localtime_s(newtime, &aclock);

  char todayChar[256]; 
  asctime_s(todayChar, 255, newtime);	// The date string has a \n appended.

  fm::string today = todayChar; 
  today.erase(today.length()-1);		// Remove the \n

  // Przygotowanie wêz³ów xml
  // Tworzymy g³ówny wêze³ pliku fmd
  CreateXMLNode(pXMLDoc, pRoot, _T("fmd"), &fmdNode);
  TCHAR fmodelVersionBuffer[128]; 
  sprintf_s(fmodelVersionBuffer, "%1.2f", FMODEL_VERSION/100.0f); 
  AddXMLAttribute(fmdNode, _T("Version"), fmodelVersionBuffer);

  char acUserName[100];
  DWORD nUserName = sizeof(acUserName);
  if (GetUserName(acUserName, &nUserName))
  {
    AddXMLAttribute(fmdNode, _T("Author"), _T(acUserName));
  }

  AddXMLAttribute(fmdNode, _T("AuthoringTool"), _T("COLLADA_to_FModel_Converter"));
  AddXMLAttribute(fmdNode, _T("AuthoringToolVersion"), _T(COLLADA_CONVERTER_VERSION) );

  AddXMLAttribute(fmdNode, _T("Date"), today.c_str());

  // Tworzymy wêze³ <Model>
  CreateXMLNode(pXMLDoc,fmdNode,_T("Model"),&modelNode);
  {
    AddXMLAttribute(modelNode,_T("Filename"), _T(mesh->FileName().c_str()) );
    //if(modelName) delete modelName; 
  }
  // Tworzymy wêze³ <MaterialList>
  CreateXMLNode(pXMLDoc,fmdNode,_T("MaterialList"),&materialListNode);
  {
    fm::vector<MaterialInstance*> materials;
    materials.clear(); 
    size_t meshInstanceCount = scene->GetMeshInstanceCount(); 
    size_t materialIndex = 0; 
    for(size_t i=0; i<meshInstanceCount; ++i)
    {
      MeshInstance *meshInstance = scene->GetMeshInstance(i); 
      size_t materialCount = meshInstance->GetMaterialCount(); 
      if(materialCount>0)
      {
        // Zapisujemy dopasowanie materia³-siatka! 
        // TODO: Aktualnie zapisujemy tylko 0!! materia³!!
        bool save = true; 
        size_t mat = materials.size(); 
        MaterialInstance *material = meshInstance->GetMaterial(0); 
        for(size_t j=0; j<materials.size(); ++j)
        {
          MaterialInstance *m = materials[j]; 
          if(m->compare(material))
          {
            mat = j; 
            save = false; 
            break; 
          }
        }
        meshMaterialPairMap.insert(i, mat); 

        if(save)
        {
          materials.push_back(material);

          CComPtr<IXMLDOMNode> materialNode;      
          // Tworzymy ten materia³ w material set'ie
          CreateXMLNode(pXMLDoc,materialListNode,_T("Material"),&materialNode);
          {
            CComPtr<IXMLDOMNode> textureMapsNode;
            AddXMLAttribute(materialNode, _T("Name"), _T(material->Name().c_str()) ); 
            if(material->GetNormalBumpMapCount()>0)
            {
              AddXMLAttribute(materialNode, _T("ShaderName"), _T(DEFAULT_SHADER)); 
            }
            else
            {
              AddXMLAttribute(materialNode, _T("ShaderName"), _T(DEFAULT_SHADER_UNBUMPED)); 
            }
            
            AddXMLAttribute(materialNode, _T("Transparency"), "False"); 
            CreateXMLNode(pXMLDoc, materialNode, _T("TextureList"), &textureMapsNode);
            {
              CComPtr<IXMLDOMNode> diffuseMapNode;
              CComPtr<IXMLDOMNode> specularMapNode;
              
              if(material->DiffuseMap() != "")
              {
                CreateXMLNode(pXMLDoc, textureMapsNode, _T("Texture"), &diffuseMapNode);
                AddXMLAttribute(diffuseMapNode, _T("Name"), _T(material->DiffuseMap().c_str()) ); 
                AddXMLAttribute(diffuseMapNode, _T("Type"), "DiffuseMap"); 
              }

              if(material->SpecularMap() != "")
              {
                CreateXMLNode(pXMLDoc, textureMapsNode, _T("Texture"), &specularMapNode);
                AddXMLAttribute(specularMapNode, _T("Name"), _T(material->SpecularMap())); 
                AddXMLAttribute(specularMapNode, _T("Type"), "SpecularMap"); 
              }

              size_t normalBumpCount = material->GetNormalBumpMapCount(); 
              for(size_t j=0; j<normalBumpCount; ++j)
              {
                CComPtr<IXMLDOMNode> bumpMapNode;
                CreateXMLNode(pXMLDoc, textureMapsNode, _T("Texture"), &bumpMapNode);
                AddXMLAttribute(bumpMapNode, _T("Name"), _T(material->GetNormalBumpMap(j).c_str()) );
                AddXMLAttribute(bumpMapNode, _T("Type"), "NormalBumpMap"); 
              }
            }
          }
        }
      }
    }
  }

  CreateXMLNode(pXMLDoc,fmdNode,_T("MaterialSetList"),&materialSetListNode);
  {
    //size_t materialMestPairCount = materialMeshPairs.size(); 
    size_t meshCount = scene->GetMeshInstanceCount(); 
    //for(size_t i=0; i<materialMestPairCount; ++i)
    for(size_t i=0; i<meshCount; ++i)
    {
      CComPtr<IXMLDOMNode> mesh;  
      // Tworzymy ten mesh
      CreateXMLNode(pXMLDoc,materialSetListNode,_T("Mesh"),&mesh);
      {
        if(meshMaterialPairMap.find(i) != meshMaterialPairMap.end())
        {
          CComPtr<IXMLDOMNode> material;  
          CreateXMLNode(pXMLDoc,mesh,_T("Material"),&material);
          TCHAR idBuffer[32]; 
          sprintf_s(idBuffer, "%d", meshMaterialPairMap.find(i)->second);
          AddXMLAttribute(material, _T("Id"), idBuffer); 
        }
      }
    }
  }

  // TODO: Dodaæ dopisywania fragmentu z animacjami 

  //21.04.2010 R.Z
  CComPtr<IXMLDOMNode> AnimationListNode;
  CComPtr<IXMLDOMNode> Animation;
  CreateXMLNode(pXMLDoc,fmdNode,_T("AnimationList"),&AnimationListNode);
  {
      CreateXMLNode(pXMLDoc, AnimationListNode, _T("Animation"), &Animation);
      AddXMLAttribute(Animation, _T("FileName"), animationFileName); 
  }

  // Zapisujemy plik
  fm::string fullname = outputDir; 
  fullname.append(fileName); 
  WriteXMLFile(fullname.c_str(), pXMLDoc);
  /**/
}
