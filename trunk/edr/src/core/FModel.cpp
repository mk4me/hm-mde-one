#include "CorePCH.h"
#include "FModel.h"

using namespace std;

//--------------------------------------------------------------------------------------------------
SFModel::SFModel(const std::wstring &address)
: properly_loaded(false)
{
	string add(address.begin(), address.end());
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
			    if (!node->ValueStr().compare("Model") && node->ToElement()->FirstAttribute())
				{
					// get file name
					TiXmlAttribute* fname = node->ToElement()->FirstAttribute();
					string name = string(fname->Value());
					file_name = wstring(name.begin(), name.end());
				}
				else if (!node->ValueStr().compare("MaterialList") && node->FirstChild())
				{
					// get all materials
					TiXmlNode* material = node->FirstChild();
					do 
					{
						// get all attributes
						TiXmlAttribute* att = material->ToElement()->FirstAttribute();
						if (att)
						{
							SMaterial mat;

							// material attributes
							do
							{
								if (!att->NameTStr().compare("Name"))
								{
									string val = att->Value();
									mat.name = wstring(val.begin(), val.end());
								}
								else if (!att->NameTStr().compare("ShaderName"))
								{
									string val = att->Value();
									mat.shader_name = wstring(val.begin(), val.end());
								}
								else if (!att->NameTStr().compare("Transparency"))
								{
									string val = att->Value();
									if (!val.compare("True"))
										mat.transparency = true;
									else
										mat.transparency = false;
								}
								else
									return;								
							} while(att = att->Next());

							material_list.push_back(mat);
						}
						else return;

						// get textures
						if (material->FirstChild() && material->FirstChild()->FirstChild())
						{
							TiXmlNode* texture = material->FirstChild()->FirstChild();

							do 
							{
								// get all attributes
								TiXmlAttribute* att = texture->ToElement()->FirstAttribute();
								if (att)
								{								
									STexture tex;
									do 
									{
										if (!att->NameTStr().compare("Name"))
										{
											string val = att->Value();
											tex.path = wstring(val.begin(), val.end());
										}
										else if (!att->NameTStr().compare("Type"))
										{
											string val = att->Value();

											if		(!val.compare("DiffuseMap"))
												tex.type = ETextureType::DIFFUSE_MAP;
											else if (!val.compare("SpecularMap"))
												tex.type = ETextureType::SPECULAR_MAP;
											else if (!val.compare("NormalBumpMap"))
												tex.type = ETextureType::NORMAL_BUMP_MAP;
											else
												tex.type = ETextureType::BASE;
										}							
									} while (att = att->Next());

									material_list[material_list.size() - 1].
										texture_list.insert(make_pair<ETextureType::TYPE, wstring>(tex.type, tex.path));
								}
							} while (texture = texture->NextSibling());							
						}
					} 
					while (material = material->NextSibling());
				}
				else if (!node->ValueStr().compare("MaterialSetList") && node->FirstChild())
				{
					// for every mesh
					TiXmlNode* mesh = node->FirstChild();
					do 
					{
						vector<int> v;

						TiXmlNode* material = mesh->FirstChild();
						if (material)
						{
							do 
							{
								v.push_back(material->ToElement()->FirstAttribute()->IntValue());
							} while (material = material->NextSibling());
						}

						material_set_list.push_back(v);							
					} while (mesh = mesh->NextSibling());
				}
				else if (!node->ValueStr().compare("MeshGroupList") && node->FirstChild())
				{
					// TODO:
					// niesprawdzone czy dziala - nie mialem modelu, ktory by z tego korzystal...

					// for every mesh
					TiXmlNode* meshGroup = node->FirstChild();
					do 
					{
						SMeshGroup mg;
					
						string name = string(meshGroup->ToElement()->FirstAttribute()->Value());
						mg.name = wstring(name.begin(), name.end());				

						TiXmlNode* mesh = meshGroup->FirstChild();
						if (mesh)
						{
							do 
							{
								mg.mesh_id.push_back(mesh->ToElement()->FirstAttribute()->IntValue());
							} while (mesh = mesh->NextSibling());
						}

						mesh_group_list.push_back(mg);							
					} while (meshGroup = meshGroup->NextSibling());
				}
				else if (!node->ValueStr().compare("AnimationList") && node->FirstChild())
				{
					TiXmlNode* animation = node->FirstChild();
					do 
					{
						string name = string(animation->ToElement()->FirstAttribute()->Value());
						animation_list.push_back(wstring(name.begin(), name.end()));
					} while (animation = animation->NextSibling());
				}

			} while (node = node->NextSibling());

			properly_loaded = true;
		}	
	}
}
