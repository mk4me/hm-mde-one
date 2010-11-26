#include "CorePCH.h"
#include "FileReader2Motion.h"
#include "FileChunkReader.h"
#include "C3DModel.h"
#include "Marker.h"
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
#include <core/IMarker.h>
#include <core/Frame.h>

#include <osg/Matrix>

// FIX: Piotr Gwiazdowski, ¿eby siê kompilowa³o
#include <core/c3dParser.h>
#include <core/C3D_Data.h>

#define SCALE 1
#define TIMERMULTIPLAY 0.0055

// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*

#define POSITION 0
#define NORMALS  1

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

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
	//TODO: pobiera nazwe pierwszego pliku tbs z zasobow [Marek Daniluk 23.11.10]
//	std::string file = dataManager->getMeshes().begin()->second;
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
    // TODO sprawdzenie wycieku pamiêci.

    ASFAMCParser* object = new ASFAMCParser();

    if(dataManager->GetSkeletonFilePathCount() > 0 && dataManager->GetAnimationFilePathCount() > 0)
    {
        if(object->ReadASFFile(dataManager->GetSkeletonFilePath(0)))
            ParserAcclaimFile2EDR(dynamic_cast<Model*>(dataManager->GetModel()), object);

        for(int i = 0; i < dataManager->GetAnimationFilePathCount(); i++)
        {
            if(object->ReadAMCFile(dataManager->GetAnimationFilePath(i)))
                LoadAnimationFromAcclaim(dataManager->GetAnimationFilePath(i), object, dynamic_cast<Model*>(dataManager->GetModel()));
        }
    }


	// TODO: ladowanie pierwszego pliku c3d jesli jakis znajduje sie w zasobach [Marek Daniluk 23.11.10]
	std::map<std::string, std::string>::const_iterator c3ds_iterator = dataManager->getC3Ds().begin();
// 	if(c3ds_iterator != dataManager->getC3Ds().end())
// 	{
// 		C3D_Data *c3d = ReadC3DFile((*c3ds_iterator).second);
// 		if(c3d)
// 		{
// 			ParseC3DFile2EDR(c3d, dynamic_cast<C3DModel*>(dataManager->GetC3DModel()));
// 		}
// 	}

    for (int i = 0; i < dataManager->GetC3dFilePathCount(); i++)
    {
        C3D_Data *c3d = ReadC3DFile(dataManager->GetC3dFilePath(i));

        if(c3d)
        {
            C3DModel* c3dModel = new C3DModel();
            std::string name = dataManager->GetC3dFilePath(i);

            c3dModel->SetName(name.substr(name.find_last_of("/")+1, name.length()));
            ParseC3DFile2EDR(c3d, c3dModel);
            dataManager->AddC3DModel(c3dModel);
        }
    }

	// TODO: ladowanie pierwszego mesha jesli jakis znajduje sie w zasobach [Marek Daniluk 23.11.10]
// 	std::map<std::string, std::string>::const_iterator meshes_iterator = dataManager->getMeshes().begin();
// 	if(meshes_iterator != dataManager->getMeshes().end())
// 	{
// 		LoadMesh((*meshes_iterator).second, dynamic_cast<Model* >(dataManager->GetModel()));
// 		dataManager->GetModel()->InicializeMesh();
// 	}
    if(dataManager->GetMeshFilePathCount() > 0)
    {
         LoadMesh(dataManager->GetMeshFilePathPath(0), dynamic_cast<Model* >(dataManager->GetModel()));
         dataManager->GetModel()->InicializeMesh();
    }
}

//--------------------------------------------------------------------------------------------------
C3DModel* FileReader2Motion::ReadFromC3DFile( std::string c3dPath )
{
    C3DModel* c3dModel = new C3DModel();
    if(!c3dPath.empty())
    {
        C3D_Data *c3d = ReadC3DFile(c3dPath);

        if(c3d)
        {
            c3dModel->SetName(c3dPath.substr(c3dPath.find_last_of("/")+1, c3dPath.length()));
            ParseC3DFile2EDR(c3d, c3dModel);
            return c3dModel;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
Model* FileReader2Motion::ReadFromTBSFile( std::string meshPath, std::string asfPath, std::vector<std::string> amcPathList)
{
    // TODO sprawdzenie wycieku pamiêci.

    ASFAMCParser* object = new ASFAMCParser();
    Model* model = new Model();

    if(!asfPath.empty())
    {
        if(object->ReadASFFile(asfPath))
            ParserAcclaimFile2EDR(model, object);
    }

    if(amcPathList.size() > 0)
    {
        for(int i = 0; i < amcPathList.size(); i++)
        {
            if(object->ReadAMCFile(amcPathList[i]))
                LoadAnimationFromAcclaim(amcPathList[i], object, model);
        }
    }

    if(!meshPath.empty())
    {
        LoadMesh(meshPath, model);
        model->InicializeMesh();
    }

    return model;
}
//--------------------------------------------------------------------------------------------------
C3D_Data* FileReader2Motion::ReadC3DFile( std::string filePath)
{
        // FIX: Piotr Gwiazdowski, ¿eby siê kompilowa³o
    c3dParser* parser = 0;
    parser = new c3dParser();

    C3D_Data* c3d = parser->parseData(filePath);

    if(c3d != NULL)
    {
        return c3d;
    }
    else
    {
        std::cout << "Wystapil blad odczytu.\n";
        delete c3d;

        if (parser) 
            delete parser;
    }

       return NULL;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::ParseC3DFile2EDR(C3D_Data* c3d, C3DModel* c3dModel)
{
    // ******************************************************************************* //
    // FIX: Piotr Gwiazdowski, ¿eby siê kompilowa³o

    std::vector<IMarker* > markerList;
    markerList.resize(c3d->getHeader()->getNumberOfC3DPoints());

    // alokacja pamiêci w liœcie
    for(int i = 0; i < c3d->getHeader()->getNumberOfC3DPoints(); i++)
        markerList[i] = new Marker();

    for(int i=0; i < (c3d->getData())->getNumberOfFrames(); i++)
    {

        float time = i*TIMERMULTIPLAY;

        for(int marker=0; marker < c3d->getHeader()->getNumberOfC3DPoints(); marker++)
        {
            MarkerFrame *markerFrame = new MarkerFrame();

            markerFrame->m_time = time;
            markerFrame->m_position.x() = c3d->getData()->getData()[i*c3d->getHeader()->getNumberOfC3DPoints()*4 + marker*4 + 0];
            markerFrame->m_position.z() = c3d->getData()->getData()[i*c3d->getHeader()->getNumberOfC3DPoints()*4 + marker*4 + 1];
            markerFrame->m_position.y() = c3d->getData()->getData()[i*c3d->getHeader()->getNumberOfC3DPoints()*4 + marker*4 + 2];

            markerList[marker]->AddFrame(markerFrame);
        }

    }

    // tymczasowe rozwi¹zanie
    for(int m = 0; m < markerList.size(); m++)
    {
        markerList[m]->SetActualPossition(markerList[m]->GetAnimationList()[0]->m_position);
    }

    c3dModel->SetMarkerList(markerList);

    std::cout << "saving finished !\n";

    return TRUE;
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

    // ustawianie hierarchi koœci
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

   // bone->bonespace = osg::Matrixd::inverse(*bone->matrix);

//    bone->bonespace = osg::Matrixd(bone->boneSpace_quaternion);
//    bone->bonespace.postMultTranslate( bone->boneSpace_translation);

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

   // bone->bonespace = osg::Matrixd::inverse(*bone->matrix);

//     bone->bonespace = osg::Matrixd(bone->boneSpace_quaternion);
//     bone->bonespace.postMultTranslate( bone->boneSpace_translation);

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

	if(model->GetSkeleton() && m_pFileReader->LoadMesh(model, skeleton))
    {
        Mapping(model, skeleton);
        return true;
    }

    printf("ERROR: The file %s was not opened\n", address.c_str());
    return false;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadAnimationFromFmodel(SFModel* fmodel, Model* model)
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
bool FileReader2Motion::LoadAnimationFromAcclaim(std::string animationName, ASFAMCParser* acclaimObject, Model* model )
{
    SkeletonAnimationList *skeletonAnimationList = model->GetAnimation();
    SkeletonAnimation *skeletonAnimation = new SkeletonAnimation();
    skeletonAnimation->m_animationName = animationName.substr(animationName.find_last_of("/")+1, animationName.length());


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

        for(int b = 0; b <skeleton->m_pBoneList.size(); b++)
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
        if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        if (length > 6)
        {
            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunk_header != FMD_PLUGIN)
            {	fclose(file); return false;	}
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
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
        if (fread(&fileSize, SIZES_INT_SIZE, 1, file) != 1)
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
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            // FMD_STRING - animation name
            if (fread(&chunkHeader, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunkHeader != FMD_STRING)
            {	fclose(file); return false;	}
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            length -= HEADER_SIZE;
            char* buff = new char [length];
            if (fread(buff, SIZES_CHAR_SIZE * length, 1, file) != 1)
            {	delete [] buff; fclose(file); return false;	}
            anim->name = string(buff);
            delete [] buff;

            // bones count		
            if (fread(&anim->bones_count, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            anim->bones = new SSkeletalAnimationBone [anim->bones_count];

            // bones...
            for (int b = 0; b < anim->bones_count; ++b)
            {
                // bone id
                if (fread(&anim->bones[b].bone_id, SIZES_INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                // key count
                if (fread(&anim->bones[b].n, SIZES_INT_SIZE, 1, file) != 1)
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
        animation->setLength(animLength);

        for(std::vector<SkeletonNode* >::iterator it = (*bones).begin(); it != (*bones).end(); it++)
        {
            if((*it)->GetId() == anim->bones[b].bone_id)
            {
                (*it)->AddAnimation(animation);	
            }
        }
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
        if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        if (length > 6)
        {
            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunk_header != FMD_PLUGIN)
            {	fclose(file); return false;	}
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
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
void FileReader2Motion::ChangePlaces( Skeleton* skeleton, int from, int destination )
{
    Bone* tmpBone = new Bone();

    tmpBone = skeleton->m_pBoneList[from];
    skeleton->m_pBoneList[from] = skeleton->m_pBoneList[destination];
    skeleton->m_pBoneList[destination] = tmpBone;
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::MirrorSkeleton( Skeleton* skeleton )
{
    // Acclaim robi mirror kosci ??
    ChangePlaces(skeleton, 3,8);
    ChangePlaces(skeleton, 4,9);
    ChangePlaces(skeleton, 5,10);
    ChangePlaces(skeleton, 6,11);
    ChangePlaces(skeleton, 7,12);

    ChangePlaces(skeleton, 15,19);
    ChangePlaces(skeleton, 16,20);
    ChangePlaces(skeleton, 17,21);
    ChangePlaces(skeleton, 18,22);
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

        // spradzanie po nazwie TODO: poprawiæ, chyba ze mamy tylko jeden asf file i on sie nie zmienia
		nazwa = nazwa.substr(nazwa.find_first_of("HOM_") + 4, nazwa.length());

		for(int i = 0; i < model_skeleton->m_pBoneList.size(); i++)
		{
			if(nazwa == model_skeleton->m_pBoneList[i]->name)
			{
				temp->m_pBoneList[b] = model_skeleton->m_pBoneList[i];

				for(int c = 0; c < mesh_skeleton->bones[b].n; c++)
				{
					temp->m_pBoneList[b]->childBoneId.push_back(mesh_skeleton->bones[b].child_bone_id[c]);
				}

				temp->m_pBoneList[b]->rot = osg::Quat(mesh_skeleton->bones[b].quaternion[0], mesh_skeleton->bones[b].quaternion[1], mesh_skeleton->bones[b].quaternion[2], mesh_skeleton->bones[b].quaternion[3]);
				temp->m_pBoneList[b]->trans = osg::Vec3f(mesh_skeleton->bones[b].translation[0], mesh_skeleton->bones[b].translation[1], mesh_skeleton->bones[b].translation[2]);

				float *bone_space_trans = mesh_skeleton->bones[b].bone_space_trans;
				float *bone_space_quat = mesh_skeleton->bones[b].bone_space_quate;

                // rigujemy mesha do szkieletu
                temp->m_pBoneList[b]->bonespace = osg::Matrixd::inverse(*temp->m_pBoneList[b]->matrix);

				break;
			}
		}
	}

    //proste uzupe³nienie.
    int sizeTemp = mesh_skeleton->bones_count;
    bool isInTemp = false;

    for(int b = 0; b < model_skeleton->m_pBoneList.size(); b++)
    {
        isInTemp = false;

        for(int t = 0; t < temp->m_pBoneList.size(); t++)
        {
            if(temp->m_pBoneList[t] == model_skeleton->m_pBoneList[b])
            {
                isInTemp = true;
                break;
            }
        }

        if(!isInTemp)
        {
            temp->m_pBoneList[sizeTemp] = model_skeleton->m_pBoneList[b];
            sizeTemp++;
        }
    }

    MirrorSkeleton(temp);

	temp->m_pRootBone = temp->m_pBoneList[0];

	model_skeleton = temp;
	model->SetSkeleton(model_skeleton);

	return true;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::MappingFromFmesh( Model *model, SSkeleton *mesh_skeleton )
{
    Skeleton* model_skeleton = model->GetSkeleton();
    Skeleton* temp = new Skeleton();

    temp->m_pBoneList.resize(mesh_skeleton->bones_count);

    for(int b = 0; b < mesh_skeleton->bones_count; b++)
    {
        std::string nazwa = mesh_skeleton->bones[b].name;
        Bone* bone = new Bone();
        temp->m_pBoneList[b] = bone;

        temp->m_pBoneList[b]->name = strdup( nazwa.c_str());
        temp->m_pBoneList[b]->idx = b;

        temp->m_pBoneList[b]->rot = osg::Quat(mesh_skeleton->bones[b].quaternion[0], mesh_skeleton->bones[b].quaternion[1], mesh_skeleton->bones[b].quaternion[2], mesh_skeleton->bones[b].quaternion[3]);
        temp->m_pBoneList[b]->trans = osg::Vec3f(mesh_skeleton->bones[b].translation[0], mesh_skeleton->bones[b].translation[1], mesh_skeleton->bones[b].translation[2]);

        temp->m_pBoneList[b]->matrix->setRotate(osg::Quat(mesh_skeleton->bones[b].quaternion[0], mesh_skeleton->bones[b].quaternion[1], mesh_skeleton->bones[b].quaternion[2], mesh_skeleton->bones[b].quaternion[3]));
        temp->m_pBoneList[b]->matrix->setTrans(osg::Vec3f(mesh_skeleton->bones[b].translation[0], mesh_skeleton->bones[b].translation[1], mesh_skeleton->bones[b].translation[2]));

        for(int c = 0; c < mesh_skeleton->bones[b].n; c++)
        {
            temp->m_pBoneList[b]->childBoneId.push_back(mesh_skeleton->bones[b].child_bone_id[c]);
        }

        temp->m_pBoneList[b]->parent_id = mesh_skeleton->bones[b].parent_id;

        float *bone_space_trans = mesh_skeleton->bones[b].bone_space_trans;
        float *bone_space_quat = mesh_skeleton->bones[b].bone_space_quate;

        temp->m_pBoneList[b]->boneSpace_translation = osg::Vec3f(bone_space_trans[0], bone_space_trans[1], bone_space_trans[2]);
        temp->m_pBoneList[b]->boneSpace_quaternion = osg::Quat(bone_space_quat[0], bone_space_quat[1], bone_space_quat[2], bone_space_quat[3]);

        temp->m_pBoneList[b]->bonespace.setRotate(temp->m_pBoneList[b]->boneSpace_quaternion);
        temp->m_pBoneList[b]->bonespace.setTrans(temp->m_pBoneList[b]->boneSpace_translation);
    }
    
    temp->m_pRootBone = temp->m_pBoneList[0];

    CreateHierarchyFmesh(temp);

    osg::Vec3d trans = temp->m_pRootBone->matrix->getTrans();

    temp->m_pRootBone->positionx = trans.x();
    temp->m_pRootBone->positiony = trans.y();
    temp->m_pRootBone->positionz = trans.z();


    for(int i =0; i < temp->m_pRootBone->child.size(); i++)
        CalculateMatrixAndDataFmesh(temp->m_pRootBone->child[i]);

    model_skeleton = temp;

    model->SetSkeleton(model_skeleton);

    return true;
}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::CreateHierarchyFmesh( Skeleton* temp )
{
    for(int b = 0; b< temp->m_pBoneList.size(); b++)
    {
        Bone* bone = temp->m_pBoneList[b];

        if(bone->parent_id != -1)
        {
            bone->parent = temp->m_pBoneList[bone->parent_id];
        }

        for(int i = 0; i < bone->childBoneId.size(); i++)
        {
            bone->child.push_back(temp->m_pBoneList[bone->childBoneId[i]]);
        }
    }

}

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::CalculateMatrixAndDataFmesh( Bone *bone)
{
    *bone->matrix =  (*bone->matrix) * (*bone->parent->matrix);

    osg::Vec3d trans = bone->matrix->getTrans();

    bone->positionx = trans.x();
    bone->positiony = trans.y();
    bone->positionz = trans.z();


    int childrenCount = bone->child.size();
    for(int i = 0; i<childrenCount; i++)
    {
        CalculateMatrixAndDataFmesh(bone->child[i]);
    }
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadAnimationFromFAnnimation(std::string address, Model* model)
{
    SSkeletonAnimation* animation = new SSkeletonAnimation();
    if (!LoadSkeletalAnimationFromFile(&std::wstring(address.begin(), address.end()), animation))
    {
        delete animation;
        return false;
    }


    SkeletonAnimationList *skeletonAnimationList = model->GetAnimation();
    SkeletonAnimation *skeletonAnimation = new SkeletonAnimation();

    Skeleton* skeleton = model->GetSkeleton();

    for(int i = 0; i < animation->bones_count; i++)
    {
        std::vector<Frame*> frames;

        // for every frame
        double animLength = 0.0;
        for (int f = 0; f < animation->bones[i].n; ++f)
        {
            Frame* frame =new Frame();
            frame->m_time = (double)animation->bones[i].frames[f].time;

            frame->rotation = osg::Quat(animation->bones[i].frames[f].quat[0], animation->bones[i].frames[f].quat[1], animation->bones[i].frames[f].quat[2], animation->bones[i].frames[f].quat[3]);
            frame->translation = osg::Vec3(animation->bones[i].frames[f].trans[0], animation->bones[i].frames[f].trans[1], animation->bones[i].frames[f].trans[2]);

            frames.push_back(frame);
        }

        BoneAnimation *boneAnmation = new BoneAnimation();
        boneAnmation->m_frames = frames;
        boneAnmation->idx = animation->bones[i].bone_id;

        skeletonAnimation->m_boneAnimationList.push_back(boneAnmation);

        for(int b = 0; b <animation->bones_count; b++)
        {
            if(animation->bones[i].bone_id == skeleton->m_pBoneList[b]->idx)
            {
                skeleton->m_pBoneList[b]->frame = boneAnmation->m_frames;
            }
        }

    }

    skeletonAnimationList->m_SkeletonAnimationList.push_back(skeletonAnimation);

    return true;
}
