#ifndef F_MODEL_H
#define F_MODEL_H

#include <string>
#include <vector>
#include <map>

#include "tinyXML/tinyxml.h"

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

typedef map<ETextureType::TYPE, wstring> texmap;

struct SMaterial
{
	wstring		name; 
	wstring		shader_name; 
	bool		transparency;

	texmap		texture_list;
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
	vector<vector<int> >		material_set_list;
	vector<SMeshGroup>		mesh_group_list;
	vector<wstring>			animation_list;

	bool					properly_loaded;

	SFModel() : properly_loaded(false) {}
	SFModel(const wstring& address);

	~SFModel() {}
};

#endif //F_MODEL_H
