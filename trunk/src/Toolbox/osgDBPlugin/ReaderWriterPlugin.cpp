#include "ReaderWriterPlugin.h"

#include <CustomGroup.h>
#include <AnimationNode.h>
#include <AnimationGroup.h>

//--------------------------------------------------------------------------------------------------
ReaderWriterPlugin::ReaderWriterPlugin()
{
    supportsExtension("tbs","Skeleton format");
}

//--------------------------------------------------------------------------------------------------
// reads file...
template <class T>
bool ReaderWriterPlugin::readAnotherCon(FILE* file, T* ret, const T& anticipated) const
{
    if (fread(ret, sizeof(T), 1, file) != 1)
        return false;

    if (*ret != anticipated)
        return false;
    else
        return true;
}

//--------------------------------------------------------------------------------------------------
// loads skeleton
bool ReaderWriterPlugin::loadSkeleton(SSkeleton* skeleton, FILE* meshFile) const
{
    if (fread(&skeleton->bones_count, INT_SIZE, 1, meshFile) != 1)
        return false;

    if (skeleton->bones_count > 0)
        skeleton->bones = new SBone [skeleton->bones_count];

    for (int b = 0; b < skeleton->bones_count; ++b)
    {	
        int chunk_header = 0;
        int length = 0;

        // another chunk
        if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
            return false;
        if (chunk_header != FMODEL_STRING)
            return false;
        if (fread(&length, INT_SIZE, 1, meshFile) != 1)
            return false;
        if (length > 6)
        {
            // get name of bone
            length -= HEADER_SIZE;
            char* buff = new char [length];
            if (fread(buff, CHAR_SIZE * length, 1, meshFile) != 1)
            {	delete [] buff; return false;	}
            skeleton->bones[b].name = buff;
            delete [] buff;

            // parent id
            if (fread(&skeleton->bones[b].parent_id, INT_SIZE, 1, meshFile) != 1)
                return false;

            // get transformations
            if (fread(&skeleton->bones[b].translation, FLOAT_SIZE * 3, 1, meshFile) != 1)
                return false;
            if (fread(&skeleton->bones[b].quaternion, FLOAT_SIZE * 4, 1, meshFile) != 1)
                return false;
            if (fread(&skeleton->bones[b].bone_space_trans, FLOAT_SIZE * 3, 1, meshFile) != 1)
                return false;
            if (fread(&skeleton->bones[b].bone_space_quate, FLOAT_SIZE * 4, 1, meshFile) != 1)
                return false;

            // child bones
            if (fread(&skeleton->bones[b].n, INT_SIZE, 1, meshFile) != 1)
                return false;

            if (skeleton->bones[b].n > 0)
                skeleton->bones[b].child_bone_id = new int [skeleton->bones[b].n];

            for (int cbi = 0; cbi < skeleton->bones[b].n; ++cbi)
                if (fread(&skeleton->bones[b].child_bone_id [cbi], INT_SIZE, 1, meshFile) != 1)
                    return false;

            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
                return false;
            if (chunk_header != FMODEL_DOF)
                return false;
            if (fread(&length, INT_SIZE, 1, meshFile) != 1)
                return false;

            // dof
            if (length > 6)
                if (fread(&skeleton->bones[b].dof, sizeof(SDof), 1, meshFile) != 1)
                    return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
// loads mesh from .mesh
bool ReaderWriterPlugin::loadMesh(std::wstring* address, SFMesh* fmesh) const
{
    FILE* meshFile = NULL;

    std::string straddress(address->begin(), address->end());
    if (meshFile = fopen(straddress.c_str(), "rb"))
    {
        // read file header
        short header = 0;
        if (!readAnotherCon(meshFile, &header, short(FMODEL_HEADER)))
        {	fclose(meshFile); return false;	}

        // chunk size
        int fileSize = 0;
        if (fread(&fileSize, INT_SIZE, 1, meshFile) != 1)
        {	fclose(meshFile); return false;	}

        // read mesh header
        header = 0;
        if (fread(&header, SHORT_INT_SIZE, 1, meshFile) != 1)
        {	fclose(meshFile); return false;	}

        // chunk size?
        int chunkSize = 0;
        if (fread(&chunkSize, INT_SIZE, 1, meshFile) != 1)
        {	fclose(meshFile); return false;	}

        do 
        {
            switch (header)
            {
            case FMODEL_MESH:
                {
                    SMesh* mesh = new SMesh();

                    // read material index in material set library
                    if (fread(&mesh->i, INT_SIZE, 1, meshFile) != 1)
                    {	delete mesh; fclose(meshFile); return false;	}

                    int chunk_header = 0;

                    do 
                    {
                        // EOF - if there isn't any plugin
                        // EOF - if there is nothing more or file terminated by 0
                        if (feof(meshFile) || (fileSize - ftell(meshFile) < 2))
                        {			
                            fmesh->mesh.push_back(mesh);
                            fclose(meshFile);			
                            return true;
                        }

                        // read header of chunk
                        if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
                        {	delete mesh; fclose(meshFile); return false;	}

                        int length = 0;
                        if (fread(&length, INT_SIZE, 1, meshFile) != 1)
                        {	delete mesh; fclose(meshFile); return false;	}

                        switch (chunk_header)
                        {
                        case FMODEL_STRING:
                            {
                                length -= HEADER_SIZE;

                                char* buff = new char [length];
                                if (fread(buff, CHAR_SIZE * length, 1, meshFile) != 1)
                                {	delete [] buff; delete mesh; fclose(meshFile); return false;	}
                                mesh->name = buff;

                                delete [] buff;
                            }
                            break;
                        case FMODEL_MESH_BUFFER:
                            {
                                if (fread(&mesh->mesh_buffer->n, INT_SIZE, 1, meshFile) != 1)
                                {	delete mesh; fclose(meshFile); return false;	}
                                if (mesh->mesh_buffer->n > 0)
                                {
                                    mesh->mesh_buffer->buffer = new byte [mesh->mesh_buffer->n];

                                    if (fread(mesh->mesh_buffer->buffer, mesh->mesh_buffer->n, 1, meshFile) != 1)
                                    {	delete mesh; fclose(meshFile); return false;	}							
                                }
                                else
                                {	delete mesh; fclose(meshFile); return false;	}


                                if (fread(&mesh->mesh_buffer->element_count, INT_SIZE, 1, meshFile) != 1)
                                {	delete mesh; fclose(meshFile); return false;	}
                                if (mesh->mesh_buffer->element_count > 0)
                                {
                                    mesh->mesh_buffer->vertex_format = new SVertexElement [mesh->mesh_buffer->element_count];

                                    if (fread(mesh->mesh_buffer->vertex_format, mesh->mesh_buffer->element_count * TVERT_DATA_SIZE, 1, meshFile) != 1)
                                    {	delete mesh; fclose(meshFile); return false;	}	
                                }
                                else
                                {	delete mesh; fclose(meshFile); return false;	}
                            }
                            break;	
                        case FMODEL_MESH_INDICES:
                            {
                                if (fread(&mesh->mesh_faces->face_count, INT_SIZE, 1, meshFile) != 1)
                                {	delete mesh; fclose(meshFile); return false;	}
                                if (mesh->mesh_buffer->n > 0)
                                {
                                    mesh->mesh_faces->faces = new SFace [mesh->mesh_faces->face_count];

                                    if (fread(mesh->mesh_faces->faces, mesh->mesh_faces->face_count * sizeof(SFace), 1, meshFile) != 1)
                                    {	delete mesh; fclose(meshFile); return false;	}							
                                }
                                else
                                {	delete mesh; fclose(meshFile); return false;	}
                            }
                            break;
                        case FMODEL_MODIFIER:
                            {
                                if (length >= GUID_SIZE)
                                {
                                    if (fread(&mesh->modifier->clsid, GUID_SIZE, 1, meshFile) != 1)
                                    {	delete mesh; fclose(meshFile); return false;	}

                                    // read header of modifier
                                    if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
                                    {	delete mesh; fclose(meshFile); return false;	}

                                    int length = 0;
                                    if (fread(&length, INT_SIZE, 1, meshFile) != 1)
                                    {	delete mesh; fclose(meshFile); return false;	}

                                    switch (chunk_header)
                                    {
                                    case FMODEL_SKIN:
                                        {
                                            mesh->modifier->skin = new SSkin();

                                            if (fread(&mesh->modifier->skin->n, INT_SIZE, 1, meshFile) != 1)
                                            {	delete mesh; fclose(meshFile); return false;	}

                                            mesh->modifier->skin->skinned_vertices = new SSkinnedVertice [mesh->modifier->skin->n];

                                            for (int i = 0; i < mesh->modifier->skin->n; ++i)
                                            {
                                                if (fread(&mesh->modifier->skin->skinned_vertices[i].vert_id, INT_SIZE, 1, meshFile) != 1)
                                                {	delete mesh; fclose(meshFile); return false;	}

                                                if (fread(&mesh->modifier->skin->skinned_vertices[i].n, INT_SIZE, 1, meshFile) != 1)
                                                {	delete mesh; fclose(meshFile); return false;	}

                                                if (mesh->modifier->skin->skinned_vertices[i].n > 0)
                                                {
                                                    mesh->modifier->skin->skinned_vertices[i].bones = 
                                                        new SAffectingBone [mesh->modifier->skin->skinned_vertices[i].n];

                                                    float sum = 0.0f;
                                                    for (int b = 0; b < mesh->modifier->skin->skinned_vertices[i].n; ++b)
                                                    {
                                                        if (fread(&mesh->modifier->skin->skinned_vertices[i].bones[b].boneID, INT_SIZE, 1, meshFile) != 1)
                                                        {	delete mesh; fclose(meshFile); return false;	}

                                                        if (fread(&mesh->modifier->skin->skinned_vertices[i].bones[b].weight, FLOAT_SIZE, 1, meshFile) != 1)
                                                        {	delete mesh; fclose(meshFile); return false;	}

                                                        sum += mesh->modifier->skin->skinned_vertices[i].bones[b].weight;
                                                    }

                                                    for (int b = 0; b < mesh->modifier->skin->skinned_vertices[i].n; ++b)
                                                        mesh->modifier->skin->skinned_vertices[i].bones[b].weight /= sum;
                                                }
                                            }
                                        }
                                        break;
                                    }
                                }
                                else
                                {	delete mesh; fclose(meshFile); return false;	}
                            }
                            break;
                        case FMODEL_MESH:	// they are allowed...
                        case FMODEL_PLUGIN:
                            break;
                        default:
                            delete mesh; 
                            fclose(meshFile); 
                            return false;	
                        }
                    } 
                    while (chunk_header != FMODEL_MESH && chunk_header != FMODEL_PLUGIN);

                    header = chunk_header;

                    // push back gathered data				
                    fmesh->mesh.push_back(mesh);
                }
                break;
            case FMODEL_PLUGIN:
                {
                    byte clsid [16];

                    if (fread(&clsid, GUID_SIZE, 1, meshFile) != 1)
                    {	fclose(meshFile); return false;	}

                    int chunk_header = 0;

                    // read header of chunk
                    if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
                    {	fclose(meshFile); return false;	}

                    int length = 0;
                    if (fread(&length, INT_SIZE, 1, meshFile) != 1)
                    {	fclose(meshFile); return false;	}
                    if (length > 6)
                    {
                        switch (chunk_header)
                        {
                        case FMODEL_BONES:
                            {
                                SSkeleton* skeleton = new SSkeleton();

                                if (!loadSkeleton(skeleton, meshFile))
                                {	delete skeleton; fclose(meshFile); return false;	}

                                assert(!fmesh->skeleton);
                                fmesh->skeleton = skeleton;

                                // EOF - if there is nothing more or file terminated by 0
                                if (feof(meshFile) || (fileSize - ftell(meshFile) < 2))
                                {		
                                    fclose(meshFile);			
                                    return true;
                                }

                                // get another chunk...
                                if (fread(&chunk_header, SHORT_INT_SIZE, 1, meshFile) != 1)
                                {	fclose(meshFile); return false;	}
                                if (fread(&length, INT_SIZE, 1, meshFile) != 1)
                                {	fclose(meshFile); return false;	}

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
                return false;
            }		
        } 
        while (!feof(meshFile) && (fileSize - ftell(meshFile) > 1));

        fclose(meshFile);			
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
bool ReaderWriterPlugin::pushSkeletonToOsg(SFMesh* fmesh, ref_ptr<osg::Group> root) const
{
    SSkeleton* skeleton = fmesh->skeleton;

    if (fmesh->skeleton && skeleton->bones_count > 0)
    {
        // skeleton

        //ref_ptr<Group> skeleton_group = new Group();
        //skeleton_group->setName("skeleton");

        ref_ptr<Group> root_bone = new SkeletonNode();
        int roots = 0;

        for (int b = 0; b < skeleton->bones_count; ++b)
        {	
            // load skeleton
            // set parents
            if (skeleton->bones[b].parent_id == -1) // handle root
            {
                skeleton->bones[b].node = root_bone;				
                ++roots;
            }
            else
            {
                skeleton->bones[b].node = new SkeletonNode();

                // i assume that bones are in order defined by their hierarchy
                skeleton->bones[skeleton->bones[b].parent_id].node->addChild(skeleton->bones[b].node);
            }

            // get pos and rot
            Vec3d bpos = Vec3(skeleton->bones[b].translation[0], skeleton->bones[b].translation[1],skeleton->bones[b].translation[2]);
            Quat  brot = Quat(Vec4(skeleton->bones[b].quaternion[0], skeleton->bones[b].quaternion[1], 
                skeleton->bones[b].quaternion[2], skeleton->bones[b].quaternion[3]));

            // translation
            ((pPat)skeleton->bones[b].node.get())->setPosition(bpos);

            // quaternion
            ((pPat)skeleton->bones[b].node.get())->setAttitude(brot);

            // name
            skeleton->bones[b].node.get()->setName(skeleton->bones[b].name);

            // id
            ((SkeletonNode*)skeleton->bones[b].node.get())->SetId(b);
        }

        // imho there should be only one root...
        if (roots != 1)   
            return false;

        //skeleton_group->addChild(root_bone);
        //root->addChild(skeleton_group);
        root->addChild(root_bone);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
// push meshes to OSG
bool ReaderWriterPlugin::pushMeshesToOsg(SFMesh* fmodel, ref_ptr<osg::Group> root) const
{
    bool any = false;

    // for every mesh
    // TODO:  update z tego - poniewaz posiada aktualny stan koœci - dorobic premieszczenie vertices i wsio.
    for (vector<SMesh*>::iterator i = fmodel->mesh.begin(); i != fmodel->mesh.end(); ++i)
    {
        SMesh* mesh = *i;

        if (mesh)
        {
            ref_ptr<Geode>		geode	 = new Geode();
            ref_ptr<Geometry>	geometry = new Geometry();

            geode->setName(mesh->name.c_str());
            geode->addDrawable(geometry);	

            // vertices, texcoords, normals
            int        numOfVertices= mesh->mesh_buffer->n / sizeof(SVertice);

            SVertice*  vData        = (SVertice*)(mesh->mesh_buffer->buffer);
            Vec3Array* vertices     = new Vec3Array();
            Vec2Array* texcoords    = new Vec2Array();
            Vec3Array* normals      = new Vec3Array();

            for (int v = 0; v < numOfVertices; ++v)
            {
                // vertex buffer
                vertices->push_back(Vec3(vData[v].position[0], vData[v].position[1], vData[v].position[2]));
                // texcoords
                texcoords->push_back(Vec2(fabs(vData[v].texture_vertex[0]), fabs(vData[v].texture_vertex[1])));
                // normals
                normals->push_back(Vec3(vData[v].normal[0], vData[v].normal[1], vData[v].normal[2]));
            }

            geometry->setVertexArray(vertices);
            geometry->setTexCoordArray(0, texcoords);
            geometry->setNormalArray(normals);

            // faces
            for (int f = 0; f < mesh->mesh_faces->face_count; ++f)
            {
                DrawElementsUInt* face = new DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
                face->push_back(mesh->mesh_faces->faces[f].index[0]);
                face->push_back(mesh->mesh_faces->faces[f].index[1]);
                face->push_back(mesh->mesh_faces->faces[f].index[2]);
                geometry->addPrimitiveSet(face);
            }

            root->addChild(geode);

            any = true;
        }
    }

    return any;	
}

//--------------------------------------------------------------------------------------------------
// is skeletal animation?
bool ReaderWriterPlugin::isSkeletalAnimation(wstring* address) const
{
    FILE* file = NULL;

    std::string straddress(address->begin(), address->end());
    if (file = fopen(straddress.c_str(), "rb"))
    {
        int chunk_header = 0;
        int length = 0;

        // another chunk
        if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}
        if (chunk_header != FMD_HEADER)
        {	fclose(file); return false;	}
        if (fread(&length, INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        if (length > 6)
        {
            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunk_header != FMD_PLUGIN)
            {	fclose(file); return false;	}
            if (fread(&length, INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            if (length > 6)
            {
                unsigned char clsid[16];
                if (fread(clsid, 16, 1, file) != 1)			
                {	fclose(file); return false;	}

                if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                fclose(file);

                if (chunk_header != FMD_SKEL_ANIM)
                    return false;
                else
                    return true;
            }
        }
        fclose(file);		
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
// is mesh animation?
bool ReaderWriterPlugin::isMeshAnimation(wstring* address) const
{
    FILE* file = NULL;

    std::string straddress(address->begin(), address->end());
    if (file = fopen(straddress.c_str(), "rb"))
    {
        int chunk_header = 0;
        int length = 0;

        // another chunk
        if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}
        if (chunk_header != FMD_HEADER)
        {	fclose(file); return false;	}
        if (fread(&length, INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        if (length > 6)
        {
            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunk_header != FMD_PLUGIN)
            {	fclose(file); return false;	}
            if (fread(&length, INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            if (length > 6)
            {
                unsigned char clsid[16];
                if (fread(clsid, 16, 1, file) != 1)			
                {	fclose(file); return false;	}

                if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                fclose(file);

                if (chunk_header != FMD_MESH_ANIM)
                    return false;
                else
                    return true;
            }
        }
        fclose(file);		
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
// load skeletal animation from file
bool ReaderWriterPlugin::loadSkeletalAnimation(wstring* address, SSkeletalAnimation* anim) const
{
    FILE* file = NULL;

    std::string straddress(address->begin(), address->end());
    if (file = fopen(straddress.c_str(), "rb"))
    {
        // chunk header
        int chunkHeader = 0;
        int fileSize = 0;

        // FMD
        if (fread(&chunkHeader, SHORT_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}
        if (fread(&fileSize, INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        //CLSID + FMD_PLUGIN
        byte* header = new byte [22];
        if (fread(header, 22, 1, file) != 1)			
        {	fclose(file); return false;	}
        delete [] header;

        // n * FMD_SKEL_ANIM
        do 
        {
            int length = 0;

            if (fread(&chunkHeader, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunkHeader != FMD_SKEL_ANIM)
            {	fclose(file); return false;	}
            if (fread(&length, INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            // FMD_STRING - animation name
            if (fread(&chunkHeader, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunkHeader != FMD_STRING)
            {	fclose(file); return false;	}
            if (fread(&length, INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            length -= HEADER_SIZE;
            char* buff = new char [length];
            if (fread(buff, CHAR_SIZE * length, 1, file) != 1)
            {	delete [] buff; fclose(file); return false;	}
            anim->name = string(buff);
            delete [] buff;

            // bones count		
            if (fread(&anim->bones_count, INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            anim->bones = new SSkeletalAnimationBone [anim->bones_count];

            // bones...
            for (int b = 0; b < anim->bones_count; ++b)
            {
                // bone id
                if (fread(&anim->bones[b].bone_id, INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                // key count
                if (fread(&anim->bones[b].n, INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                anim->bones[b].frames = new SKeyFrame [anim->bones[b].n];

                // for every key
                for (int k = 0; k < anim->bones[b].n; ++k)
                {
                    if (fread(&anim->bones[b].frames[k], sizeof(SKeyFrame), 1, file) != 1)
                    {	fclose(file); return false;	}					
                }
            }

        } while (!feof(file) && (fileSize - ftell(file) > 1));

        fclose(file);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
bool ReaderWriterPlugin::createArrayHoldingBones	
(std::vector<SkeletonNode*>* bones, ref_ptr<osg::Group> root) const
{
    SkeletonNode* skeleteon = NULL;

    // find root bone
    int childNum = root->getNumChildren();
    for (int i = 0; i < childNum; ++i)
    {		
        skeleteon = dynamic_cast<SkeletonNode*>(root->getChild(i));
        if (skeleteon)
            break;
    }
    if (!skeleteon) return false; // no skeleton?!

    // get every bone & put them to list/whatever where they are sorted by id
    struct Obj
    {
        static void createArrayOfBones(SkeletonNode* bone, 
            vector<SkeletonNode*>* bones)
        {
            bones->push_back(bone);
            for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
                createArrayOfBones((SkeletonNode*)bone->getChild(b), bones);
        }
    };
    Obj::createArrayOfBones(skeleteon, bones);

    return true;
}



//--------------------------------------------------------------------------------------------------
// push animation to OSG
bool ReaderWriterPlugin::pushSkeletalAnimationToOSG	
(wstring* name, SSkeletalAnimation* anim, std::vector<SkeletonNode*>* bones)	const
{
    wstring animationName = name->substr(0, name->find_first_of('.'));

    // for every animated (?) bone...
    for (int b = 0; b < anim->bones_count; ++b)
    {
        // attach new animation to bone
        ref_ptr<CAnimationGroup> animation = new CAnimationGroup(anim->bones[b].n);
        animation->setName(string(animationName.begin(), animationName.end()));

        // for every frame
        double animLength = 0.0;
        for (int f = 0; f < anim->bones[b].n; ++f)
        {
            ref_ptr<CAnimationNode> frame = new CAnimationNode((double)anim->bones[b].frames[f].time);

            animLength = animLength < anim->bones[b].frames[f].time ? anim->bones[b].frames[f].time : animLength;

            frame->setPosition(Vec3(anim->bones[b].frames[f].trans[0], anim->bones[b].frames[f].trans[1],
                anim->bones[b].frames[f].trans[2]));									// translation
            frame->setAttitude(Quat(anim->bones[b].frames[f].quat[0], anim->bones[b].frames[f].quat[1],
                anim->bones[b].frames[f].quat[2], anim->bones[b].frames[f].quat[3]));	// attitude

            animation->addChild(frame);
        }

        // length of entire animation...

        // Moze sie zdazyc tak i z animacja rozpocznie sie od pewnego momentu - wtedy dlugosc poczatkowych wezlow bedzie rowna zera - moze sie rozsypac program. 
        // TODO:  cos z tym zrobic,  stworzycadtwarzanie animacji uwzgledaniajac ten przyapdek
        //	if (animLength > 0.0)
        //	{
        animation->setLength(animLength);
        (*bones)[anim->bones[b].bone_id]->AddAnimation(animation);	
        //	}
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
// load animation
bool ReaderWriterPlugin::loadAnimation(SFModel* fmodel, SFAnimation* animations, ref_ptr<osg::Group> root) const
{
    vector<SkeletonNode*> bones;
    createArrayHoldingBones(&bones, root);

    for (vector<wstring>::iterator i = fmodel->animation_list.begin(); i != fmodel->animation_list.end(); ++i)
    {
        wstring address = fmodel->path + L'/' + *i;

        if		(isSkeletalAnimation(&address) && !bones.empty())
        {
            SSkeletalAnimation* animation = new SSkeletalAnimation();
            if (!loadSkeletalAnimation(&address, animation))
            {
                delete animation;
                return false;
            }

            animations->skeletal_animation.insert(make_pair<wstring, SSkeletalAnimation*>(*i, animation));
            pushSkeletalAnimationToOSG(&(*i), animation, &bones);

        }
        else if (isMeshAnimation(&address))
        {
            // TODO:
            // na razie zakldam, ze nie jest to potrzebne
            // aczkolwiek...
            assert(false);
        }
        else
            return false;
    }

    return true;
}


//--------------------------------------------------------------------------------------------------
// read node
osgDB::ReaderWriter::ReadResult ReaderWriterPlugin::readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const
{
    bool hr = true;

    wstring fmodel(file.begin(), file.end());

    // TODO:
    // sprawdzic czy to usuwam - mam wrazenie, ze cos pozmienialem...
    SFModel* fmodel_file = new SFModel(fmodel);
    if (!fmodel_file->properly_loaded)
        return NULL;

    fmodel_file->path = fmodel.substr(0, fmodel.find_last_of(L'/'));

    // model - root
    ref_ptr<CCustomGroup> root = new CCustomGroup();
    root->setName(file.substr(file.find_last_of('/') + 1, file.length()));
    root->setFModel(fmodel_file);

    if (hr)
    {
        //--------------------------------------------------------------------------------------------------
        // model - mesh & skeleton

        // path of file containing mesh
        wstring model_name = fmodel_file->path + L'/' + fmodel_file->file_name;

        // loading it separately from pushing elements to OSG
        // because of too long & unreadable code

        SFMesh* fmesh = new SFMesh();
        root->setFMesh(fmesh);

        // 19.04.2010 - poprzednio by³o to co ponizej zakomentowane.  - testy dla ³adowania modelu bez mesha
        loadMesh(&model_name, fmesh);

        //if (!loadMesh(&model_name, fmesh))
        //	return NULL;
        // push meshes
        //	if (!pushMeshesToOsg(fmesh, root))
        //		return NULL;

        // push skeleton (reason why i'm doing it separately is above)
        if (!pushSkeletonToOsg(fmesh, root))
            return NULL;

        // handle animations - create container and assign him to root
        SFAnimation* animations = new SFAnimation();
        root->setAnimations(animations);

        // load animations
        if (!loadAnimation(fmodel_file, animations, root))
            return NULL;
    }

    return (osgDB::ReaderWriter::ReadResult)root;
}

