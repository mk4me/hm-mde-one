#include "FModel.h"

HRESULT SFModel::GetAttributes(IXmlReader* reader, vector<wstring>* attributes) 
{
	const WCHAR* valueStr;
	HRESULT hr = reader->MoveToFirstAttribute();

	if (S_FALSE == hr)
		return hr;
	if (S_OK != hr) 
		return -1;
	else 
	{
		while (TRUE) 
		{
			if (!reader->IsDefault()) 
			{
				if (FAILED(hr = reader->GetValue(&valueStr, NULL)))
					return -1;
				attributes->push_back(valueStr);			
			}

			if (S_OK != reader->MoveToNextAttribute())
				break;
		}
	}

	return hr;
}

SFModel::SFModel(const std::wstring &address)
: properly_loaded(false)
{
	HRESULT					hr;
	CComPtr<IStream>		fileStream;
	CComPtr<IXmlReader>		reader;
	XmlNodeType				nodeType;
	const WCHAR*			localName;
	const WCHAR*			valueStr;

	// Open read-only input stream
	if (FAILED(hr = SHCreateStreamOnFile(address.c_str(), STGM_READ, &fileStream))) 
		return;
	if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &reader, NULL))) 
		return;
	if (FAILED(hr = reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit))) 
		return;
	if (FAILED(hr = reader->SetInput(fileStream))) 
		return;

	EFModelList::TYPE node;	

	// read until there are no more nodes
	while (S_OK == (hr = reader->Read(&nodeType))) 
	{
		switch (nodeType) 
		{
		case XmlNodeType_Element:
			{		
				// get element
				if (FAILED(hr = reader->GetLocalName(&localName, NULL)))						
					return;

				wprintf(L"Element: %s\n", localName);

				// get attributes
				vector<wstring> att;
				if (FAILED(hr = GetAttributes(reader, &att)))
					return;
				for (vector<wstring>::iterator i = att.begin(); i != att.end(); ++i)
					wprintf(L"Attr: %s\n", i->c_str());	

				// Model
				if      (!wcscmp(localName, L"Model"))
				{
					if (att.size() != 1)
						return;
					file_name = att[0];
				}
				// MaterialList
				else if (!wcscmp(localName, L"MaterialList"))
				{
					node = EFModelList::MATERIAL;
				}
				// MaterialSetList
				else if (!wcscmp(localName, L"MaterialSetList"))
				{
					node = EFModelList::MATERIAL_SET;
				}
				// MeshGroupList
				else if (!wcscmp(localName, L"MeshGroupList"))
				{
					node = EFModelList::MESH_GROUP;
				}
				// Material
				else if (!wcscmp(localName, L"Material") && node == EFModelList::MATERIAL)
				{
					if (att.size() != 3)
						return;

					SMaterial mat;
					mat.name		= att[0];
					mat.shader_name = att[1];
					if (!wcscmp(att[2].c_str(), L"True"))
						mat.transparency = true;
					else
						mat.transparency = false;

					material_list.push_back(mat);
				}
				// Texture
				else if (!wcscmp(localName, L"Texture"))
				{
					if (att.size() != 2 || !material_list.size())
						return;

					STexture tex;
					tex.path = att[0];

					if		(!wcscmp(att[1].c_str(), L"DiffuseMap"))
						tex.type = ETextureType::DIFFUSE_MAP;
					else if (!wcscmp(att[1].c_str(), L"SpecularMap"))
						tex.type = ETextureType::SPECULAR_MAP;
					else if (!wcscmp(att[1].c_str(), L"NormalBumpMap"))
						tex.type = ETextureType::NORMAL_BUMP_MAP;
					else
						tex.type = ETextureType::BASE;

					material_list[material_list.size() - 1].texture_list.push_back(tex);
				}
				// MaterialSetList
				else if (!wcscmp(localName, L"Mesh"))
				{
					vector<int> v;
					material_set_list.push_back(v);
				}
				// material
				else if (!wcscmp(localName, L"Material") && node == EFModelList::MATERIAL_SET)
				{
					if (att.size() != 1 || !material_set_list.size())
						return;

					int id = _wtoi(att[0].c_str());
					material_set_list[material_set_list.size() - 1].push_back(id);
				}
				// MeshGroup
				else if (!wcscmp(localName, L"MeshGroup"))
				{
					if (att.size() != 1)
						return;

					SMeshGroup mg;
					mg.name = att[0];
					mesh_group_list.push_back(mg);
				}
				// Mesh
				else if (!wcscmp(localName, L"Mesh") && node == EFModelList::MESH_GROUP)
				{
					if (att.size() != 1 || !mesh_group_list.size())
						return;

					int id = _wtoi(att[0].c_str());
					mesh_group_list[mesh_group_list.size() - 1].mesh_id.push_back(id);
				}
				// Animation
				else if (!wcscmp(localName, L"Animation"))
				{
					if (att.size() != 1) 
						return;

					animation_list.push_back(att[0]);
				}

				// move to element...
				if (FAILED(hr = reader->MoveToElement()))
					return;
			}
			break;

		case XmlNodeType_Text:
		case XmlNodeType_Whitespace:
			{
				if (FAILED(hr = reader->GetValue(&valueStr, NULL)))
					return;
				wprintf(L"\n");
			}
			break;
		}
	}

	properly_loaded = true;
}