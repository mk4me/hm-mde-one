#include "ReaderWriterPlugin.h"

//////////////////////////////////////////////////////////////////////////
// c  - tor
ReaderWriterPlugin::ReaderWriterPlugin()
{
	supportsExtension("tbs","Skeleton format");
}

//////////////////////////////////////////////////////////////////////////
// reads file...
template <class T>
HRESULT ReaderWriterPlugin::ReadAnotherCon(FILE* file, T* ret, const T& anticipated) const
{
	if (fread(ret, sizeof(T), 1, file) != 1)
		return E_FAIL;

	if (*ret != anticipated)
		return E_FAIL;
	else
		return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// loads skeleton
HRESULT ReaderWriterPlugin::LoadSkeleton(SSkeleton* skeleton, FILE* meshFile) const
{
	if (fread(&skeleton->bones_count, INT_SIZE, 1, meshFile) != 1)
		return E_FAIL;

	if (skeleton->bones_count > 0)
		skeleton->bones = new SBone [skeleton->bones_count];

	for (int b = 0; b < skeleton->bones_count; ++b)
	{	
		int chunk_header = 0;
		int length = 0;

		// another chunk
		if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
			return E_FAIL;
		if (chunk_header != FMODEL_STRING)
			return E_FAIL;
		if (fread(&length, INT_SIZE, 1, meshFile) != 1)
			return E_FAIL;
		if (length > 6)
		{
			// get name of bone
			length -= HEADER_SIZE;
			char* buff = new char [length];
			if (fread(buff, CHAR_SIZE * length, 1, meshFile) != 1)
			{	delete [] buff; return E_FAIL;	}
			skeleton->bones[b].name = buff;
			delete [] buff;

			// parent id
			if (fread(&skeleton->bones[b].parent_id, INT_SIZE, 1, meshFile) != 1)
				return E_FAIL;

			// get transformations
			if (fread(&skeleton->bones[b].translation, FLOAT_SIZE * 3, 1, meshFile) != 1)
				return E_FAIL;
			if (fread(&skeleton->bones[b].quaternion, FLOAT_SIZE * 4, 1, meshFile) != 1)
				return E_FAIL;
			if (fread(&skeleton->bones[b].bone_space_trans, FLOAT_SIZE * 3, 1, meshFile) != 1)
				return E_FAIL;
			if (fread(&skeleton->bones[b].bone_space_quate, FLOAT_SIZE * 4, 1, meshFile) != 1)
				return E_FAIL;

			// child bones
			if (fread(&skeleton->bones[b].n, INT_SIZE, 1, meshFile) != 1)
				return E_FAIL;

			if (skeleton->bones[b].n > 0)
				skeleton->bones[b].child_bone_id = new int [skeleton->bones[b].n];

			for (int cbi = 0; cbi < skeleton->bones[b].n; ++cbi)
				if (fread(&skeleton->bones[b].child_bone_id [cbi], INT_SIZE, 1, meshFile) != 1)
					return E_FAIL;

			// another chunk
			if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
				return E_FAIL;
			if (chunk_header != FMODEL_DOF)
				return E_FAIL;
			if (fread(&length, INT_SIZE, 1, meshFile) != 1)
				return E_FAIL;

			// dof
			if (length > 6)
				if (fread(&skeleton->bones[b].dof, sizeof(SDof), 1, meshFile) != 1)
					return E_FAIL;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// loads mesh from .mesh
HRESULT ReaderWriterPlugin::LoadMesh(std::wstring* address, SFMesh* fmesh) const
{
	FILE* meshFile = NULL;

	if (meshFile = _wfopen(address->c_str(), L"rb"))
	{
		// read file header
		short header = 0;
		if (FAILED(ReadAnotherCon(meshFile, &header, short(FMODEL_HEADER))))
		{	fclose(meshFile); return E_FAIL;	}

		// chunk size
		int fileSize = 0;
		if (fread(&fileSize, INT_SIZE, 1, meshFile) != 1)
		{	fclose(meshFile); return E_FAIL;	}

		// read mesh header
		header = 0;
		if (fread(&header, SHORT_INT_SIZE, 1, meshFile) != 1)
		{	fclose(meshFile); return E_FAIL;	}

		// chunk size?
		int chunkSize = 0;
		if (fread(&chunkSize, INT_SIZE, 1, meshFile) != 1)
		{	fclose(meshFile); return E_FAIL;	}
		
		do 
		{
			switch (header)
				{
				case FMODEL_MESH:
					{
						SMesh* mesh = new SMesh();

						// read material index in material set library
						if (fread(&mesh->i, INT_SIZE, 1, meshFile) != 1)
						{	delete mesh; fclose(meshFile); return E_FAIL;	}

						int chunk_header = 0;

						do 
						{
							// EOF - if there isn't any plugin
							// EOF - if there is nothing more or file terminated by 0
							if (feof(meshFile) || (fileSize - ftell(meshFile) < 2))
							{			
								if (fmesh->mesh) assert(__LINE__);
								fmesh->mesh = mesh;
								fclose(meshFile);			
								return S_OK;
							}

							// read header of chunk
							if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
							{	delete mesh; fclose(meshFile); return E_FAIL;	}

							int length = 0;
							if (fread(&length, INT_SIZE, 1, meshFile) != 1)
							{	delete mesh; fclose(meshFile); return E_FAIL;	}

							switch (chunk_header)
								{
								case FMODEL_STRING:
									{
										length -= HEADER_SIZE;

										char* buff = new char [length];
										if (fread(buff, CHAR_SIZE * length, 1, meshFile) != 1)
										{	delete [] buff; delete mesh; fclose(meshFile); return E_FAIL;	}
										mesh->name = buff;

										delete [] buff;
									}
									break;
								case FMODEL_MESH_BUFFER:
									{
										if (fread(&mesh->mesh_buffer->n, INT_SIZE, 1, meshFile) != 1)
										{	delete mesh; fclose(meshFile); return E_FAIL;	}
										if (mesh->mesh_buffer->n > 0)
										{
											mesh->mesh_buffer->buffer = new byte [mesh->mesh_buffer->n];

											if (fread(mesh->mesh_buffer->buffer, mesh->mesh_buffer->n, 1, meshFile) != 1)
											{	delete mesh; fclose(meshFile); return E_FAIL;	}							
										}
										else
										{	delete mesh; fclose(meshFile); return E_FAIL;	}


										if (fread(&mesh->mesh_buffer->element_count, INT_SIZE, 1, meshFile) != 1)
										{	delete mesh; fclose(meshFile); return E_FAIL;	}
										if (mesh->mesh_buffer->element_count > 0)
										{
											mesh->mesh_buffer->vertex_format = new SVertexElement [mesh->mesh_buffer->element_count];

											if (fread(mesh->mesh_buffer->vertex_format, mesh->mesh_buffer->element_count * TVERT_DATA_SIZE, 1, meshFile) != 1)
											{	delete mesh; fclose(meshFile); return E_FAIL;	}	
										}
										else
										{	delete mesh; fclose(meshFile); return E_FAIL;	}
									}
									break;	
								case FMODEL_MESH_INDICES:
									{
										if (fread(&mesh->mesh_faces->face_count, INT_SIZE, 1, meshFile) != 1)
										{	delete mesh; fclose(meshFile); return E_FAIL;	}
										if (mesh->mesh_buffer->n > 0)
										{
											mesh->mesh_faces->faces = new SFace [mesh->mesh_faces->face_count];

											if (fread(mesh->mesh_faces->faces, mesh->mesh_faces->face_count * sizeof(SFace), 1, meshFile) != 1)
											{	delete mesh; fclose(meshFile); return E_FAIL;	}							
										}
										else
										{	delete mesh; fclose(meshFile); return E_FAIL;	}
									}
									break;
								case FMODEL_MODIFIER:
									{
										if (length >= GUID_SIZE)
										{
											if (fread(&mesh->modifier->clsid, GUID_SIZE, 1, meshFile) != 1)
											{	delete mesh; fclose(meshFile); return E_FAIL;	}

											// read header of modifier
											if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
											{	delete mesh; fclose(meshFile); return E_FAIL;	}

											int length = 0;
											if (fread(&length, INT_SIZE, 1, meshFile) != 1)
											{	delete mesh; fclose(meshFile); return E_FAIL;	}

											switch (chunk_header)
											{
											case FMODEL_SKIN:
												{
													mesh->modifier->skin = new SSkin();

													if (fread(&mesh->modifier->skin->n, INT_SIZE, 1, meshFile) != 1)
													{	delete mesh; fclose(meshFile); return E_FAIL;	}

													mesh->modifier->skin->skinned_vertices = new SSkinnedVertice [mesh->modifier->skin->n];

													for (int i = 0; i < mesh->modifier->skin->n; ++i)
													{
														if (fread(&mesh->modifier->skin->skinned_vertices[i].vert_id, INT_SIZE, 1, meshFile) != 1)
														{	delete mesh; fclose(meshFile); return E_FAIL;	}

														if (fread(&mesh->modifier->skin->skinned_vertices[i].n, INT_SIZE, 1, meshFile) != 1)
														{	delete mesh; fclose(meshFile); return E_FAIL;	}

														if (mesh->modifier->skin->skinned_vertices[i].n > 0)
														{
															mesh->modifier->skin->skinned_vertices[i].bones = 
																new SAffectingBone [mesh->modifier->skin->skinned_vertices[i].n];

															for (int b = 0; b < mesh->modifier->skin->skinned_vertices[i].n; ++b)
															{
																if (fread(&mesh->modifier->skin->skinned_vertices[i].bones[b].boneID, INT_SIZE, 1, meshFile) != 1)
																{	delete mesh; fclose(meshFile); return E_FAIL;	}

																if (fread(&mesh->modifier->skin->skinned_vertices[i].bones[b].weight, FLOAT_SIZE, 1, meshFile) != 1)
																{	delete mesh; fclose(meshFile); return E_FAIL;	}
															}
														}
													}
												}
												break;
											}
										}
										else
										{	delete mesh; fclose(meshFile); return E_FAIL;	}
									}
									break;
								case FMODEL_MESH:	// they are allowed...
								case FMODEL_PLUGIN:
									break;
								default:
									delete mesh; 
									fclose(meshFile); 
									return E_FAIL;	
							}
						} 
						while (chunk_header != FMODEL_MESH && chunk_header != FMODEL_PLUGIN);

						header = chunk_header;

						// push back gathered data
						if (fmesh->mesh) assert(__LINE__);
						fmesh->mesh = mesh;
					}
					break;
				case FMODEL_PLUGIN:
					{
						byte clsid [16];

						if (fread(&clsid, GUID_SIZE, 1, meshFile) != 1)
						{	fclose(meshFile); return E_FAIL;	}

						int chunk_header = 0;

						// read header of chunk
						if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
						{	fclose(meshFile); return E_FAIL;	}

						int length = 0;
						if (fread(&length, INT_SIZE, 1, meshFile) != 1)
						{	fclose(meshFile); return E_FAIL;	}
						if (length > 6)
						{
							switch (chunk_header)
							{
							case FMODEL_BONES:
								{
									SSkeleton* skeleton = new SSkeleton();

									if (FAILED(LoadSkeleton(skeleton, meshFile)))
									{	delete skeleton; fclose(meshFile); return E_FAIL;	}

									if (fmesh->skeleton) assert(__LINE__);
									fmesh->skeleton = skeleton;
							
									// EOF - if there is nothing more or file terminated by 0
									if (feof(meshFile) || (fileSize - ftell(meshFile) < 2))
									{		
										fclose(meshFile);			
										return S_OK;
									}

									// get another chunk...
									if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
									{	fclose(meshFile); return E_FAIL;	}
									if (fread(&length, INT_SIZE, 1, meshFile) != 1)
									{	fclose(meshFile); return E_FAIL;	}
							
									header = chunk_header;
									chunkSize = length;									
								}
								break;
							}
						}
					}
					break;
				default:
					fclose(meshFile);
					return E_FAIL;
			}		
		} 
		while (!feof(meshFile) && (fileSize - ftell(meshFile) > 1));
	
		fclose(meshFile);			
		return S_OK;
	}

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
//
HRESULT ReaderWriterPlugin::PushSkeletonToOsg(SSkeleton* skeleton, osg::Group* root) const
{
	if (skeleton->bones_count > 0)
	{
		Group* skeleton_group = new Group();
		skeleton_group->setName("skeleton");

		Group* root_bone = new PositionAttitudeTransform();

		for (int b = 0; b < skeleton->bones_count; ++b)
		{	
			//////////////////////////////////////////////////////////////////////////
			// load skeleton

			// set parents
			if (skeleton->bones[b].parent_id == -1) // handle root
			{
				skeleton->bones[b].node = root_bone;
			}
			else
			{
				skeleton->bones[b].node = new PositionAttitudeTransform();

				// i assume that bones are in order defined by their hierarchy
				skeleton->bones[skeleton->bones[b].parent_id].node->addChild(skeleton->bones[b].node);
			}

			// get pos and rot
			Vec3d bpos = Vec3(skeleton->bones[b].translation[0], skeleton->bones[b].translation[1],skeleton->bones[b].translation[2]);
			Quat  brot = Quat(Vec4(skeleton->bones[b].quaternion[0], skeleton->bones[b].quaternion[1], 
				skeleton->bones[b].quaternion[2], skeleton->bones[b].quaternion[3]));

			// translation
			((pPat)skeleton->bones[b].node)->setPosition(bpos);

			// quaternion
			((pPat)skeleton->bones[b].node)->setAttitude(brot);

			// name
			skeleton->bones[b].node->setName(skeleton->bones[b].name);
		}
		
		skeleton_group->addChild(root_bone);
		root->addChild(skeleton_group);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
HRESULT ReaderWriterPlugin::CreateGeometry(SMesh* mesh, osg::Group* root) const
{
	// create geode
	mesh->geode = new Geode();
	mesh->geode->setName("mesh");

	// create geometry
	Geometry* meshGeometry = new Geometry();
	mesh->geode->addDrawable(meshGeometry);	

	// vertices, texcoords, normals
	int        numOfVertices = mesh->mesh_buffer->n / sizeof(SVertice);
	SVertice*   vData        = (SVertice*)mesh->mesh_buffer->buffer;
	Vec3Array* vertices      = new Vec3Array();
	Vec2Array* texcoords     = new Vec2Array();
	Vec3Array* normals       = new Vec3Array();

	for (int v = 0; v < numOfVertices; ++v)
	{
		// vertex buffer
		vertices->push_back(Vec3(vData[v].position[0], vData[v].position[1], vData[v].position[2]));
		// texcoords
		texcoords->push_back(Vec2(fabs(vData[v].texture_vertex[0]), fabs(vData[v].texture_vertex[1])));
		// normals
		normals->push_back(Vec3(vData[v].normal[0], vData[v].normal[1], vData[v].normal[2]));
	}

	meshGeometry->setVertexArray(vertices);
	meshGeometry->setTexCoordArray(0, texcoords);
	meshGeometry->setNormalArray(normals);

	// faces
	for (int f = 0; f < mesh->mesh_faces->face_count; ++f)
	{
		DrawElementsUInt* face = new DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
		face->push_back(mesh->mesh_faces->faces[f].index[0]);
		face->push_back(mesh->mesh_faces->faces[f].index[1]);
		face->push_back(mesh->mesh_faces->faces[f].index[2]);
		meshGeometry->addPrimitiveSet(face);
	}

	// add to root
	root->addChild(mesh->geode);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
HRESULT ReaderWriterPlugin::GetMaterials(SMesh* mesh, SFModel* fmodel, osg::Group* root) const
{
	// TODO
	// at the moment - take first texture and load it

	if (fmodel->material_list.size() && fmodel->material_list[0].texture_list.size())
	{
		Texture2D* tex = new Texture2D();
		wstring path = fmodel->path + L"/" + fmodel->material_list[0].texture_list[0].path;
		tex->setDataVariance(osg::Object::DYNAMIC); 
		osg::Image* klnFace = osgDB::readImageFile(string(path.begin(), path.end()));
		if (!klnFace)
			return E_FAIL;
		tex->setImage(klnFace);

		StateSet* stateOne = new StateSet();
		stateOne->setTextureAttributeAndModes(0, tex, StateAttribute::ON);

		mesh->geode->setStateSet(stateOne);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// read node
osgDB::ReaderWriter::ReadResult ReaderWriterPlugin::readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const
{
	HRESULT hr = S_OK;

	wstring fmodel(file.begin(), file.end());

	SFModel fmodel_file(fmodel);
	if (!fmodel_file.properly_loaded)
		hr = E_FAIL;

	fmodel_file.path = fmodel.substr(0, fmodel.find_last_of(L'/'));

	// model - root
	Group* root = new Group();
	root->setName(file.substr(file.find_last_of('/') + 1, file.length()));

	if (SUCCEEDED(hr))
	{
		//////////////////////////////////////////////////////////////////////////
		// model - mesh & skeleton

		// path of file containing mesh
		wstring model_name = fmodel_file.path + L'/' + fmodel_file.file_name;

		// loading it separately from pushing elements to OSG
		// because of too long & unreadable code
		SFMesh fmesh;
		if (FAILED(LoadMesh(&model_name, &fmesh)))
		{	MessageBox(0, L"Failed to load mesh!", L"error", MB_OK); root->releaseGLObjects(); return NULL; }

		// push skeleton (reason why i'm doing it separately is above)
		if (FAILED(PushSkeletonToOsg(fmesh.skeleton, root)))
		{	MessageBox(0, L"Failed to create skeleton!", L"error", MB_OK); root->releaseGLObjects(); return NULL; }

		// load geometry
		if (FAILED(CreateGeometry(fmesh.mesh, root)))
		{	MessageBox(0, L"Failed to create geometry!", L"error", MB_OK); root->releaseGLObjects(); return NULL; }

		// materials
		if (FAILED(GetMaterials(fmesh.mesh, &fmodel_file, root)))
		{	MessageBox(0, L"Failed to load materials!", L"error", MB_OK); root->releaseGLObjects(); return NULL; }
	}

	return root;
}

