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
#include "DataManager.h"

#include <core/Vec3.h>
#include <core/Skeleton.h>

#include <core/ASFAMCParser.h>
#include <core/Frame.h>

#include <osg/Matrix>


#define SCALE 1
#define TIMERMULTIPLAY 0.0055

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
void FileReader2Motion::ReadFile(DataManager *dataManager)
{
    std::string file = dataManager->GetFileName();
    Model *model = dynamic_cast<Model* >(dataManager->GetModel());
    //TODO: poprawic wizualnie i zrzuwaæ koñcówke na ma³e litery
    if(file.substr(file.length() - 3, file.length()) == "DAE")
        ReadFrmDAEFile(file, model);
    else if(file.substr(file.length() - 3, file.length()) == "tbs")
        ReadFromTBSFile(dataManager);
    else if(file.substr(file.length() - 3, file.length()) == "asf")
        ReadAcclaimFile(file, model);
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::ReadFromTBSFile(DataManager *dataManager)
{
    ASFAMCParser* object = new ASFAMCParser();
    if(dataManager->GetSkeletonFilePathCount() > 0 && dataManager->GetAnimationFilePathCount() > 0)
    {
        if(object->ReadASFFile(dataManager->GetSkeletonFilePath(0)))
            ParserAcclaimFile2EDR(dynamic_cast<Model*>(dataManager->GetModel()), object);

        for(int i = 0; i < dataManager->GetAnimationFilePathCount(); i++)
        {
            if(object->ReadAMCFile(dataManager->GetAnimationFilePath(i)))
                LoadAnimation(object, dynamic_cast<Model*>(dataManager->GetModel()));
        }
    }

    if(dataManager->GetMeshFilePathCount() > 0)
    {
        //LoadMesh(dataManager->GetMeshFilePathPath(0), dynamic_cast<Model* >(dataManager->GetModel()));
        //dataManager->GetModel()->InicializeMesh();
    }

    //wstring fmodel(file.begin(), file.end());
    // 
    //SFModel* fmodel_file = new SFModel(fmodel);
    //if (!fmodel_file->properly_loaded)
    //    return;
    // 
    //fmodel_file->path = fmodel.substr(0, fmodel.find_last_of(L'/'));
    // 
    //// path of file containing mesh
    //wstring model_name = fmodel_file->path + L'/' + fmodel_file->file_name;

    //LoadMesh(&model_name, model);
    //model->InicializeMesh();

    // TODO : sparsowac w przyszlosci szkielet animacji z tbs do nowej struktury animacji na potrzeby acclaim file
    //if (!LoadSkeleton(model))
    //    return;

    // TODO : sparsowac w przyszlosci szkielet animacji z tbs do nowej struktury animacji na potrzeby acclaim file
    // LoadAnimation(fmodel_file, model);
    //model->ApplyMaterial(&fmodel_file->material_list, fmodel_file->path);
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

    m_Fs.seekg(0,ios::beg);
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
    Skeleton* skeleton = new Skeleton();

    int boneCount = acclaimObject->getNumberOfJoints();
    Channel* joint = acclaimObject->getCurrentJointPointer();

    skeleton->m_pBoneList.clear();
	skeleton->m_pBoneList.resize(boneCount);
    for(int i = 0; i < boneCount; i++)
    {
        Bone* bone = new Bone();
		bone->initialPosition = osg::Vec3d(0.f,0.f,0.f);
		bone->isInitialPosition = false;
        bone->parent = NULL;

        bone->idx = joint->getID();
        bone->dir[0] = joint->getDirX();
        bone->dir[1] = joint->getDirY();
        bone->dir[2] = joint->getDirZ();

        bone->length = joint->getLength();
        joint->getAxis(bone->axis_x, bone->axis_y, bone->axis_z);

        bone->name = joint->getName();
        bone->parentName = joint->getParent();

        int numeOfChild = joint->numOfChildren();
        for(int i = 0; i < numeOfChild; i++)
        {
            bone->children.push_back(joint->getChild(i));
        }

        skeleton->m_pBoneList[joint->getID()] = bone;
        joint++;
    }

    skeleton->m_pRootBone = skeleton->m_pBoneList[0];

    for(std::vector<Bone*>::iterator it = skeleton->m_pBoneList.begin(); it != skeleton->m_pBoneList.end(); it++)
    {
        std::string parent = (*it)->parentName;

        for(int i = 0; i < boneCount; i++)
        {
            if(skeleton->m_pBoneList[i]->name == parent)
            {
                (*it)->parent = skeleton->m_pBoneList[i];
                break;
            }
        }

        int childNumber = (*it)->children.size();
        for(int i = 0; i < childNumber; i++)
        {
            std::string child = (*it)->children[i];
            for(int j = 0; j < boneCount; j++)
            {
                if(skeleton->m_pBoneList[j]->name == child)
                {
                    (*it)->child.push_back(skeleton->m_pBoneList[j]);
                    break;
                }
            }
        }
    }

    model->SetSkeleton(skeleton);
    LoadSkeleton(model);
//    LoadAnimation(acclaimObject, model);
}

//--------------------------------------------------------------------------------------------------
void calculateChildMatrix(Bone *bone)
{
    // C - macierz obrotów (axis) plik ASF
    // Cinv - inversja maierzy C
    // B - macierz pozycji bezwzglêdnej: offfset = direction*length - plik ASF

    osg::Matrixd C, Cinv, B, tmp, tmp2;
    float rx, ry, rz;
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->axis_x*M_PI/180.;
    ry=-bone->axis_y*M_PI/180.;
    rz=-bone->axis_z*M_PI/180.;

    // zmieniona rotacja ry = rz, rz = ry - prawdopodobnie osie w osg sa lewoskrêtne ??  czy¿by?
    C.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght * SCALE;
    y = bone->dir[1] * lenght * SCALE;
    z = bone->dir[2] * lenght * SCALE;

    B.makeTranslate(x, z, y);
    Cinv = osg::Matrixd::inverse(C);

    // L = CinvMCB - wzór na animacjie AMC file- jednakze osg ma macierz transponowane !!!!
    // nie posiadamy macierzy M tak jak ma to miejsce w przypadku animacji, wiêc wczytanie szkieletu jest bezpieczne.
    // mo¿na mnozyæ macierz nie achowuj¹c w³asnoœc transpozycji - i tak wczyta sie poprawni

    tmp = Cinv * C;
    tmp2 = tmp * B;
    *bone->matrix = tmp2 * (*bone->parent->matrix);
    //bone->matrix->postMultScale(osg::Vec3d(SCALE, SCALE, SCALE));

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x();
    bone->positiony = trans.y();
    bone->positionz = trans.z();


    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrix(bone->child[i]);
    }
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadSkeleton(Model* model)
{
    if(!model->GetSkeleton())
        return false;

    // OBLICZANIE ROOTA.

    Skeleton* skeleton = model->GetSkeleton();
    Bone *bone = skeleton->m_pRootBone;

    // C - macierz obrotów (axis) plik ASF
    // Cinv - inversja maierzy C
    // B - macierz pozycji bezwzglêdnej: offfset = direction*length - plik ASF

    osg::Matrixd C, Cinv, B, tmp, tmp2;
    float rx, ry, rz;
    // minusowe wartoœci dla uzyskania po³o¿enia wzglêdem œwiata + zamiana na radiany
    rx=-bone->axis_x*M_PI/180.;
    ry=-bone->axis_y*M_PI/180.;
    rz=-bone->axis_z*M_PI/180.;

    // zmieniona rotacja ry = rz, rz = ry - prawdopodobnie osie w osg sa lewoskrêtne ??  czy¿by?
    C.makeRotate(rx, osg::Vec3f(1.0f, 0.0f, 0.0f), ry, osg::Vec3f(0.0f, 0.0f, 1.0f), rz, osg::Vec3f(0.0f, 1.0f, 0.0f));

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght * SCALE;
    y = bone->dir[1] * lenght * SCALE;
    z = bone->dir[2] * lenght * SCALE;

    B.makeTranslate(x, y, z);
    Cinv = osg::Matrixd::inverse(C);

    // L = CinvMCB - wzór na animacjie AMC file- jednakze osg ma macierz transponowane !!!!
    // nie posiadamy macierzy M tak jak ma to miejsce w przypadku animacji, wiêc wczytanie szkieletu jest bezpieczne.
    // mo¿na mnozyæ macierz nie achowuj¹c w³asnoœc transpozycji - i tak wczyta sie poprawni
    
    tmp = Cinv * C;
    *bone->matrix = tmp * B;
    bone->matrix->postMultScale(osg::Vec3d(SCALE, SCALE, SCALE));

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x();
    bone->positiony = trans.y();
    bone->positionz = trans.z();


    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        calculateChildMatrix(bone->child[i]);
    }

    return true;
}


//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadMesh(std::string address, Model* model)
{
    FILE* meshFile = NULL;

    m_pFileReader = new FileChunkReader(address);
    SSkeleton* skeleton = new SSkeleton;
	m_pFileReader->LoadMesh(model, skeleton);

	Mapping(model, skeleton);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadAnimation( SFModel* fmodel, Model* model )
{
    SkeletonAnimationList* animations = new SkeletonAnimationList();

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

            //TODO R.Zowal naprawic porem
            //       animations->m_skeletonAanimation.insert(make_pair<wstring, SSkeletonAnimation*>(*i, animation));
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
bool FileReader2Motion::LoadAnimation(ASFAMCParser* acclaimObject, Model* model )
{
    SkeletonAnimationList *skeletonAnimationList = model->GetAnimation();
    SkeletonAnimation *skeletonAnimation = new SkeletonAnimation();


    Skeleton* skeleton = model->GetSkeleton();
    Channel* joint = acclaimObject->getCurrentJointPointer();
    int boneCount = acclaimObject->getNumberOfJoints();

    for(int i = 0; i < boneCount; i++)
    {
        std::vector<Frame*> frames;

        for(int j = 0; j< acclaimObject->getNumberOfFrames(); j++)
        {
            Frame* frame =new Frame();
            frame->m_time = j*TIMERMULTIPLAY;
            joint->getRotation(j+1, frame->rotx, frame->roty, frame->rotz);
            joint->getTranslation(j+1, frame->translationx, frame->translationy, frame->translationz);
            frames.push_back(frame);
        }

        BoneAnimation *boneAnmation = new BoneAnimation();
        boneAnmation->m_frames = frames;
        boneAnmation->idx = joint->getID();

        skeletonAnimation->m_boneAnimationList.push_back(boneAnmation);

        for(int b = 0; b <boneCount; b++)
        {
            if(joint->getID()== skeleton->m_pBoneList[b]->idx)
            {
                skeleton->m_pBoneList[b]->frame = boneAnmation->m_frames;
            }
        }

        joint++;
    }

    skeletonAnimationList->m_SkeletonAnimationList.push_back(skeletonAnimation);

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
        // TODO:  cos z tym zrobic,  stworzyc odtwarzanie animacji uwzgledaniajac ten przypadek
        //	if (animLength > 0.0)
        //	{

        animation->setLength(animLength);

        for(std::vector<SkeletonNode* >::iterator it = (*bones).begin(); it != (*bones).end(); it++)
        {
            if((*it)->GetId() == anim->bones[b].bone_id)
            {
                (*it)->AddAnimation(animation);	
            }
        }

        //   (*bones)[anim->bones[b].bone_id]->AddAnimation(animation);	
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
bool FileReader2Motion::Mapping( Model *model, SSkeleton *mesh_skeleton )
{
	Skeleton* model_skeleton = model->GetSkeleton();
	Skeleton* temp = new Skeleton();

	temp->m_pBoneList.resize(model_skeleton->m_pBoneList.size());

	for(int b = 0; b < mesh_skeleton->bones_count; b++)
	{
		std::string nazwa = mesh_skeleton->bones[b].name;

		for(int i = 0; i < model_skeleton->m_pBoneList.size(); i++)
		{
			if(nazwa == model_skeleton->m_pBoneList[i]->name)
			{
				temp->m_pBoneList[b] = model_skeleton->m_pBoneList[i];

                float *bone_space_trans = mesh_skeleton->bones[b].bone_space_trans;
                float *bone_space_quat = mesh_skeleton->bones[b].bone_space_quate;

                float *bone_trans = mesh_skeleton->bones[b].translation;
                float *bone_quat = mesh_skeleton->bones[b].quaternion;

                temp->m_pBoneList[b]->boneSpace_translation = osg::Vec3f(bone_space_trans[0], bone_space_trans[1], bone_space_trans[2]);
                temp->m_pBoneList[b]->boneSpace_quaternion = osg::Quat(bone_space_quat[0], bone_space_quat[1], bone_space_quat[2], bone_space_quat[3]);

                temp->m_pBoneList[b]->mesh_bone_translation = osg::Vec3f(bone_trans[0], bone_trans[1], bone_trans[2]);
                temp->m_pBoneList[b]->mesh_bone_quaternion = osg::Quat(bone_quat[0], bone_quat[1], bone_quat[2], bone_quat[3]);

				break;
			}
		}
	}

	temp->m_pRootBone = temp->m_pBoneList[0];

	model_skeleton = temp;
	model->SetSkeleton(model_skeleton);

	return true;
}
