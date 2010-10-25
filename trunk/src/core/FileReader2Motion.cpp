#include "CorePCH.h"
#include "Frame.h"
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
#include "Skeleton.h"

#include <core/ASFAMCParser.h>
#include <core/Matrix.h>
#include "Transform.h"


#include <core/Bone.h>


#define TIMERMULTIPLAY 0.02


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
	for(int i = 0; i < boneCount; i++)
	{
		Bone* bone = new Bone();

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

		skeleton->m_pBoneList.push_back(bone);
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
     LoadAnimation(acclaimObject, model);
}

//--------------------------------------------------------------------------------------------------
void calculateMatrix(Bone *bone)
{
    double C[4][4];
    double Cinv[4][4];
    double B[4][4];

    int i;
    double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp2[4][4];

    rotationZ(Rz, -bone->axis_z);
    rotationY(Ry, -bone->axis_y);
    rotationX(Rx, -bone->axis_x);
    matrix_mult(Rz, Ry, tmp);
    matrix_mult(tmp, Rx, C);

    //matrix_transpose(tmp2, C);   

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght;
    y = bone->dir[1] * lenght;
    z = bone->dir[2] * lenght;

    LoadFromTranslationVec(x,z,y,B);
    //matrix_transpose(tmp, B);   

    //CopyMatrix(Cinv,C);

    for ( int i = 0; i < 4; i++ )
        for ( int j = 0; j < 4; j++ ) 
        {
            Cinv[i][j] = C[i][j];
        }

        // 		osg::Matrixf mtrix = osg::Matrixf::inverse(osg::Matrixf(C[0][0], C[0][1], C[0][2], C[0][3],
        // 																C[1][0], C[1][1], C[1][2], C[1][3],
        // 																C[2][0], C[2][1], C[2][2], C[2][3],
        // 																C[3][0], C[3][1], C[3][2], C[3][3]));

        Invert(Cinv);

        SetToProduct(Cinv, C, tmp);
        SetToProduct(tmp, B, tmp2);

        SetToProduct(bone->parent->matrix, tmp2, bone->matrix);

        bone->positionx = bone->matrix[0][3];
        bone->positiony = bone->matrix[1][3];
        bone->positionz = bone->matrix[2][3];

        int childrenCount = bone->child.size();
        for(int i = 0; i<childrenCount; i++)
        {
            calculateMatrix(bone->child[i]);
        }
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadSkeleton(Model* model)
{
    if(!model->GetSkeleton())
        return false;


    Skeleton* skeleton = model->GetSkeleton();


    Bone *bone = skeleton->m_pRootBone;

    double C[4][4];
    double Cinv[4][4];
    double B[4][4];

    int i;
    double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp2[4][4];

    rotationZ(Rz, bone->axis_z);
    rotationY(Ry, bone->axis_y);
    rotationX(Rx, bone->axis_x);
    matrix_mult(Rz, Ry, tmp);
    matrix_mult(tmp, Rx, C);

    //matrix_transpose(tmp2, C);   

    float x,y,z;
    float lenght = bone->length;
    x = bone->dir[0] * lenght;
    y = bone->dir[1] * lenght;
    z = bone->dir[2] * lenght;

    LoadFromTranslationVec(x,z,y,B);
    //matrix_transpose(tmp, B);   

    //CopyMatrix(Cinv,C);

    for ( int i = 0; i < 4; i++ )
        for ( int j = 0; j < 4; j++ ) 
        {
            Cinv[i][j] = C[i][j];
        }

        // 		osg::Matrixf mtrix = osg::Matrixf::inverse(osg::Matrixf(C[0][0], C[0][1], C[0][2], C[0][3],
        // 																C[1][0], C[1][1], C[1][2], C[1][3],
        // 																C[2][0], C[2][1], C[2][2], C[2][3],
        // 																C[3][0], C[3][1], C[3][2], C[3][3]));

        Invert(Cinv);

        SetToProduct(Cinv, C, tmp);
        SetToProduct(tmp, B, bone->matrix);

        bone->positionx = bone->matrix[0][3];
        bone->positiony = bone->matrix[1][3];
        bone->positionz = bone->matrix[2][3];

        int childrenCount = bone->child.size();
        for(int i = 0; i<childrenCount; i++)
        {
            calculateMatrix(bone->child[i]);
        }


        return true;
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
    Frame* animations = new Frame();

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
	Skeleton* skeleton = model->GetSkeleton();
	Channel* joint = acclaimObject->getCurrentJointPointer();
	int boneCount = acclaimObject->getNumberOfJoints();

	for(int i = 0; i < boneCount; i++)
	{
		std::vector<Frame*> frames;

		for(int j = 0; j< acclaimObject->getNumberOfFrames(); j++)
		{
			Frame* frame =new Frame();
			frame->m_time = j/TIMERMULTIPLAY;
			joint->getRotation(j+1, frame->rotx, frame->roty, frame->rotz);
			joint->getTranslation(j+1, frame->translationx, frame->translationy, frame->translationz);
			frames.push_back(frame);
		}
		
		for(int b = 0; b <boneCount; b++)
		{
			if(joint->getID()== skeleton->m_pBoneList[b]->idx)
			{
				skeleton->m_pBoneList[b]->frame = frames;
			}
		}


		joint++;
	}

//     std::wstring animationName = L"testing";
//     SFAnimation* animations = new SFAnimation();
//     Channel* jointPointer = acclaimObject->getCurrentJointPointer();
// 
//     vector<SkeletonNode*> bones;
//     CreateArrayHoldingBones(&bones, model);
// 
//     SSkeletonAnimation* animation = new SSkeletonAnimation();
// 
//     animation->name = std::string(animationName.begin(), animationName.end());
//     animation->bones_count = acclaimObject->getNumberOfJoints();
//     animation->bones = new SSkeletalAnimationBone[animation->bones_count];
// 
//     for(int b = 0; b < animation->bones_count; b++)
//     {
//         animation->bones[b].bone_id = jointPointer->getID();
//         animation->bones[b].n = acclaimObject->getNumberOfFrames();
// 
//         animation->bones[b].frames = new SKeyFrame[animation->bones[b].n];
// 
//         //for every key
//         for(int k = 0; k < animation->bones[b].n; k++)
//         {
//             animation->bones[b].frames[k].time = TIMERMULTIPLAY * k;
// 
//             //     jointPointer->getOffset(animation->bones[b].frames[k].trans[0], animation->bones[b].frames[k].trans[1], animation->bones[b].frames[k].trans[2]);
//             //      jointPointer->getQuaternionFromEuler(k+1, animation->bones[b].frames[k].quat[3], animation->bones[b].frames[k].quat[0], animation->bones[b].frames[k].quat[1], animation->bones[b].frames[k].quat[2]);               
// 
//             if(jointPointer->getType() == CHANNEL_TYPE::ROOT)
//             {
//                 jointPointer->getQuaternionFromEuler(k+1,animation->bones[b].frames[k].quat[3], animation->bones[b].frames[k].quat[0], animation->bones[b].frames[k].quat[1], animation->bones[b].frames[k].quat[2]);
//            
//                 jointPointer->getPosition(animation->bones[b].frames[k].trans[0], animation->bones[b].frames[k].trans[1], animation->bones[b].frames[k].trans[2]);
//                 //jointPointer->getOffset(animation->bones[b].frames[k].trans[0], animation->bones[b].frames[k].trans[1], animation->bones[b].frames[k].trans[2]);
//             }
//             else
//             {
//                 // get quaternion
//                 jointPointer->getQuaternionFromEuler(k+1,animation->bones[b].frames[k].quat[3], animation->bones[b].frames[k].quat[0], animation->bones[b].frames[k].quat[1], animation->bones[b].frames[k].quat[2]);
//            
//                 // get translation
//                 jointPointer->getPosition(animation->bones[b].frames[k].trans[0], animation->bones[b].frames[k].trans[1], animation->bones[b].frames[k].trans[2]);
//             }
//         }
// 
//         jointPointer++;
//     }
// 
//     animations->m_skeletonAanimation.insert(make_pair<wstring, SSkeletonAnimation*>(animationName, animation));
// 
//     InicializeSkeletalAnimation(&animationName, animation, &bones);
// 
// 
// //     jointPointer = acclaimObject->getCurrentJointPointer();
// // 
// //     for(int b = 0; b < animation->bones_count; b++)
// //     {
// //         animation->bones[b].bone_id = jointPointer->getID();
// //         animation->bones[b].n = acclaimObject->getNumberOfFrames();
// // 
// //         animation->bones[b].frames = new SKeyFrame[animation->bones[b].n];
// // 
// //         // attach new animation to bone
// //         CAnimationGroup* anim = new CAnimationGroup(animation->bones[b].n);
// //         anim->setName(string(animationName.begin(), animationName.end()));
// // 
// //         // for every frame
// //         double animLength = 0.0;
// // 
// //         //for every key
// //         for(int k = 0; k < animation->bones[b].n; k++)
// //         {
// // 
// //             animation->bones[b].frames[k].time = TIMERMULTIPLAY * k;
// // 
// //             CAnimationNode* frame = new CAnimationNode((double)animation->bones[b].frames[k].time);
// // 
// //             animLength = animLength < animation->bones[b].frames[k].time ? animation->bones[b].frames[k].time : animLength;
// // 
// //             float translation      [3]; // bone translation vector 
// //             float quaternion       [4]; // bone quaternion 
// // 
// //             if(jointPointer->getType() == CHANNEL_TYPE::ROOT)
// //             {
// //                 jointPointer->getTranslation(k+1, translation[0], translation[1], translation[2]);
// //                 jointPointer->getQuaternionFromEuler(k+1, quaternion[3], quaternion[0], quaternion[1], quaternion[2]);
// // 
// //             }
// //             else
// //             {
// //                 // get translation
// //                 jointPointer->getOffset(translation[0], translation[1], translation[2]);
// // 
// //                 // get quaternion
// //                 jointPointer->getQuaternionFromEuler(k+1, quaternion[3], quaternion[0], quaternion[1], quaternion[2]);
// //             }
// // 
// //             frame->setPosition(osg::Vec3(translation[0], translation[1], translation[2]));	
// //             frame->setAttitude(osg::Quat(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));	// attitude
// // 
// //             anim->addChild(frame);
// //         }
// // 
// //         anim->setLength(animLength);
// // 
// //         for(std::vector<SkeletonNode* >::iterator it = bones.begin(); it != bones.end(); it++)
// //         {
// //             if((*it)->GetId() == animation->bones[b].bone_id)
// //             {
// //                 (*it)->AddAnimation(anim);	
// //             }
// //         }
// // 
// //         //bones[animation->bones[b].bone_id]->AddAnimation(anim);	
// // 
// // 
// //         jointPointer++;
// //     }
// 
// 
//     //    if(IsMeshAnimation(&address))
//     //    {
//     //         // TODO:
//     //         // na razie zakladam, ze nie jest to potrzebne
//     //         //aczkolwiek...
//     //         assert(false);
//     //     }
//     //     else
//     //         return false;
//     //     }
// 
//     model->SetAnimation(animations);
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

