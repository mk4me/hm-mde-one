#include "CorePCH.h"
#include "FileReader2Motion.h"
#include "FileChunkReader.h"
#include "Model.h"
#include "Mesh.h"

#include "FModel.h"
#include "FAnimation.h"
#include "AnimationNode.h"
#include "AnimationGroup.h"
#include "SkeletonNode.h"

#include <core/Vec3.h>

#include <core/ASFAMCParser.h>


// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*

#define POSITION 0
#define NORMALS  1

std::ifstream m_Fs;

using namespace std;

FileChunkReader *m_pFileReader;


unsigned int Read(VOID* pData, unsigned int numBytes)
{
    m_Fs.read((char* )pData, numBytes);

    if (m_Fs.gcount() == numBytes)
        return m_Fs.gcount();

    // cout << "Nie uda³o siê odczytaæ" << numBytes << "pliku, odczytano:  "<<  m_Fs.gcount() << std::endl;
    return 0;
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::ReadFile( const std::string& file, Model* model )
{
    //TODO: poprawic wizualnie i zrzuwaæ koñcówke na ma³e litery
    if(file.substr(file.length() - 3, file.length()) == "DAE")
        ReadFrmDAEFile(file, model);
    else if(file.substr(file.length() - 3, file.length()) == "tbs")
        ReadFromTBSFile(file, model);
    else if(file.substr(file.length() - 3, file.length()) == "asf")
        ReadAcclaimFile(file, model);
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::ReadFromTBSFile( const std::string& file, Model* model )
{
    wstring fmodel(file.begin(), file.end());

    SFModel* fmodel_file = new SFModel(fmodel);
    if (!fmodel_file->properly_loaded)
        return;

    fmodel_file->path = fmodel.substr(0, fmodel.find_last_of(L'/'));

    // path of file containing mesh
    wstring model_name = fmodel_file->path + L'/' + fmodel_file->file_name;

    LoadMesh(&model_name, model);
    model->InicializeMesh();

    if (!LoadSkeleton(model))
        return;

    LoadAnimation(fmodel_file, model);
    model->ApplyMaterial(&fmodel_file->material_list, fmodel_file->path);
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::ReadFrmDAEFile(const std::string& file, Model* model )
{
    // opcjnalnie ob³ugujemy tylko format TBS;
    m_Fs.open(file.c_str(), ios::binary);

    std::string buffer = "";

    // Jesli jest podany filepath otwieramy lokalny plik
    if (file != "")
    {
        // Musimy odczytaæ ca³y plik
        CHAR temp[256];
        ZeroMemory(temp,256);
        while(Read((VOID*)temp,255) == 255)
        {
            temp[255] = '\0';
            buffer += temp;
            ZeroMemory(temp,256);
        }
        buffer += temp;
        buffer += "\0";
    }

    if (buffer != "")
    {
        SDea2Motion* io_data = new SDea2Motion();
        std::cout << file << std::endl;
        io_data->in_fileName = file;
        io_data->in_fileName += '\0';
        std::cout << io_data->in_fileName;
        io_data->in_buffer = new char[buffer.size()+1];
        strcpy_s(io_data->in_buffer,buffer.size()+1,buffer.c_str());
        io_data->in_length = buffer.size();
        Convert2Motion(io_data);

        std::cout << "Wczytano siatek: " << io_data->out_meshes.size() << std::endl;

        LoadMeshFromDAE(model, io_data);
        model->InicializeMesh();
    }

    m_Fs.close();
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::ReadAcclaimFile( const std::string& file, Model* model )
{
    ASFAMCParser* object = new ASFAMCParser();
    object->readAcclaimFiles(file, file.substr(0, file.length() - 3) + "amc");

    ParserAcclaimFile2EDR(model, object);
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::LoadMeshFromDAE( Model* model, SDea2Motion* io_data )
{
    for(unsigned int i=0; i<io_data->out_meshes.size(); i++)
    {
        IIMesh* mesh = io_data->out_meshes[i];
        unsigned int subMeshes = mesh->GetSubMeshCount();

        for(unsigned int j=0; j<subMeshes; j++)
        {
            Mesh* mMesh = new Mesh();

            float* dataTransform = mesh->GetTranslation();
            IISubMesh* subMesh = mesh->GetSubMesh(j);

            int vertexCount = subMesh->GetVertexCount();
            float* vertexData = subMesh->GetPositions();
            float* normalData = subMesh->GetNormals();
            CVec3 *vertex = new CVec3[vertexCount];
            CVec3 *normal = new CVec3[vertexCount];

            CVec3 transform(dataTransform[0], dataTransform[1], dataTransform[2]);

            // wczytywanie danych i konwersja do naszej struktury
            // TODO: zrobiæ refactoring kodu
            for(int nTri =0; nTri <= vertexCount;)
            {
                vertex[nTri/3]._v[0] = vertexData[nTri];
                vertex[nTri/3]._v[1] = vertexData[nTri+1];
                vertex[nTri/3]._v[2] = vertexData[nTri+2];

                vertex[nTri/3] += transform;

                normal[nTri/3]._v[0] = normalData[nTri];
                normal[nTri/3]._v[1] = normalData[nTri+1];
                normal[nTri/3]._v[2] = normalData[nTri+2];

                nTri += 3;
            }   

            // £adowanie danych
            mMesh->SetTrisCount(subMesh->GetIndicesCount());
            mMesh->SetTris(subMesh->GetIndices());
            mMesh->SetVertexCount(vertexCount/3);
            mMesh->CalculateFaceNormals();
            mMesh->SetVertNormals(normal);
            mMesh->SetVertex(vertexData);
            mMesh->SetVerts(vertex);
            model->AddMesh(mMesh);
        }
    }
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::ParserAcclaimFile2EDR(Model *model, ASFAMCParser *acclaimObject )
{
    int bonesCount = acclaimObject->getNumberOfJoints();
    Channel* joint = acclaimObject->getCurrentJointPointer();
    vectorOfJoints* Joints = acclaimObject->getJoints();

    SSkeleton* skeleton = new SSkeleton();
    SBone* bone = new SBone[bonesCount];

    skeleton->bones_count = bonesCount;
    for(int i = 0; i < bonesCount; i++)
    {
        bone[i].name = joint->getName();
        bone[i].id = joint->getID();
        bone[i].parent_id = acclaimObject->getJoint(joint->getParent())->getID();

        // get translation
        joint->getTranslation(1, bone[i].translation[0], bone[i].translation[1], bone[i].translation[2]);

        // get quaternion
        joint->getAngleAndAxis(1,bone[i].quaternion[3], bone[i].quaternion[0], bone[i].quaternion[1], bone[i].quaternion[2]);

        // number of children
        bone[i].n = joint->numOfChildren();

        // childs_id
        bone[i].child_bone_id = new int[joint->numOfChildren()];
        for(int j = 0; j < joint->numOfChildren(); j++)
        {
            bone[i].child_bone_id[j] = acclaimObject->getJoint(joint->getChild(i))->getID();
        }

        joint++;
    }

    LoadSkeleton(model);
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadMesh(std::wstring* address, Model* model)
{
    FILE* meshFile = NULL;
    std::string straddress(address->begin(), address->end());

    m_pFileReader = new FileChunkReader(straddress);
    m_pFileReader->LoadMesh(model);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadAnimation( SFModel* fmodel, Model* model )
{
    SFAnimation* animations = new SFAnimation();

    vector<SkeletonNode*> bones;
    CreateArrayHoldingBones(&bones, model);

    for(vector<std::wstring>::iterator i = fmodel->animation_list.begin(); i != fmodel->animation_list.end(); ++i)
    {
        wstring address = fmodel->path + L'/' + *i;

        if(IsSkeletalAnimation(&address) && !bones.empty())
        {
            SSkeletonAnimation* animation = new SSkeletonAnimation();
            if (!LoadSkeletalAnimationFromFile(&address, animation))
            {
                delete animation;
                return false;
            }

            animations->m_skeletonAanimation.insert(make_pair<wstring, SSkeletonAnimation*>(*i, animation));
            InicializeSkeletalAnimation(&(*i), animation, &bones);

        }
        else if(IsMeshAnimation(&address))
        {
            // TODO:
            // na razie zakldam, ze nie jest to potrzebne
            // aczkolwiek...
            assert(false);
        }
        else
            return false;
    }

    model->SetAnimation(animations);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::CreateArrayHoldingBones	
(std::vector<SkeletonNode*>* bones, Model* model)
{
    SkeletonNode* skeleteon = NULL;

    // find root bone
    int childNum = model->getNumChildren();
    for (int i = 0; i < childNum; ++i)
    {		
        skeleteon = dynamic_cast<SkeletonNode*>(model->getChild(i));
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
bool FileReader2Motion::IsSkeletalAnimation(std::wstring* address)
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
// load skeletal animation from file
bool FileReader2Motion::LoadSkeletalAnimationFromFile(std::wstring* address, SSkeletonAnimation* anim)
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
// push animation to OSG
bool FileReader2Motion::InicializeSkeletalAnimation	
(std::wstring* name, SSkeletonAnimation* anim, std::vector<SkeletonNode*>* bones)
{
    std::wstring animationName = name->substr(0, name->find_first_of('.'));

    // for every animated (?) bone...
    for (int b = 0; b < anim->bones_count; ++b)
    {
        // attach new animation to bone
        CAnimationGroup* animation = new CAnimationGroup(anim->bones[b].n);
        animation->setName(string(animationName.begin(), animationName.end()));

        // for every frame
        double animLength = 0.0;
        for (int f = 0; f < anim->bones[b].n; ++f)
        {
            CAnimationNode* frame = new CAnimationNode((double)anim->bones[b].frames[f].time);

            animLength = animLength < anim->bones[b].frames[f].time ? anim->bones[b].frames[f].time : animLength;

            frame->setPosition(osg::Vec3(anim->bones[b].frames[f].trans[0], anim->bones[b].frames[f].trans[1],
                anim->bones[b].frames[f].trans[2]));									// translation
            frame->setAttitude(osg::Quat(anim->bones[b].frames[f].quat[0], anim->bones[b].frames[f].quat[1],
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
// is mesh animation?
bool FileReader2Motion::IsMeshAnimation(std::wstring* address)
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
bool FileReader2Motion::LoadSkeleton(Model* model)
{
    if(!model->GetSkeleton())
        return false;

    SSkeleton* skeleton = model->GetSkeleton();

    if (model->GetSkeleton() && skeleton->bones_count > 0)
    {
        // skeleton

        //ref_ptr<Group> skeleton_group = new Group();
        //skeleton_group->setName("skeleton");

        osg::ref_ptr<osg::Group> root_bone = new SkeletonNode();
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
            osg::Vec3d bpos = osg::Vec3(skeleton->bones[b].translation[0], skeleton->bones[b].translation[1],skeleton->bones[b].translation[2]);
            osg::Quat  brot = osg::Quat(osg::Vec4(skeleton->bones[b].quaternion[0], skeleton->bones[b].quaternion[1], 
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
        model->addChild(root_bone);
    }

    return true;
}
