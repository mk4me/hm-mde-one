#pragma once

#include <stdio.h>
#include <tchar.h>

#include <Windows.h>

#include <atlbase.h>
#include <xmllite.h>
#include <strsafe.h>

#include <string>
#include <vector>

using namespace std;

namespace ETextureType
{
	enum TYPE
	{
		BASE = 0,
		DIFFUSE_MAP,
		SPECULAR_MAP,
		NORMAL_BUMP_MAP
	};
};

namespace EFModelList
{
	enum TYPE
	{
		MATERIAL = 0,
		MATERIAL_SET,
		MESH_GROUP,
		ANIMATION
	};
};

struct STexture
{
	wstring				path;
	ETextureType::TYPE	type;
};

struct SMaterial
{
	wstring				name; 
	wstring				shader_name; 
	bool				transparency;

	vector<STexture>	texture_list;
};

struct SMeshGroup
{
	wstring name;
	vector<int>	mesh_id;
};

struct SFModel
{
	wstring					file_name;
	wstring					path;

	vector<SMaterial>		material_list;
	vector<vector<int>>		material_set_list;
	vector<SMeshGroup>		mesh_group_list;
	vector<wstring>			animation_list;

	bool					properly_loaded;

	// get attributes
	HRESULT GetAttributes(IXmlReader* reader, vector<wstring>* attributes);

	// c - tor
	SFModel() : properly_loaded(false) {}
	SFModel(const wstring& address);

	// d - tor
	~SFModel() {}
};