#ifndef FILE_READER_2_MOTION_H
#define FILE_READER_2_MOTION_H

#include <string>
#include <vector>
#include <core/dae2Motion.h>

class ASFAMCParser;

class C3DModel;
class Model;
class SkeletonNode;
class DataManager;
class Skeleton;
class C3D_Data;

struct SSkeletonAnimation;
struct SFModel;
struct SSkeleton;

struct Bone;

class FileReader2Motion
{
public:
	static bool LoadMesh(std::string address, Model* model);
    static bool LoadAnimationFromFmodel(SFModel* fmodel, Model* model);
    static bool LoadAnimationFromAcclaim(std::string animationName, ASFAMCParser* acclaimObject, Model* model);
    static bool ParseC3DFile2EDR(C3D_Data* c3d, C3DModel* c3dModel);

    static bool LoadAnimationFromFAnnimation(std::string address, Model* model);


    static Model* GetModelFromResources(std::string meshPath, std::string asfPath, std::vector<std::string> amcPathList);
    static C3DModel* GetC3DModelFromResources(std::string c3dPath);

private:
    static void ReadFrmDAEFile(const std::string& file, Model* model);
    static void ReadFromTBSFile(DataManager *dataManager);
    static void ReadAcclaimFile(const std::string& file, Model* model);

    static void LoadMeshFromDAE(Model* model, SDea2Motion* io_data);
    static void ParserAcclaimFile2EDR(Model* model, ASFAMCParser* acclaimObject);

    static bool CreateArrayHoldingBones(std::vector<SkeletonNode*>* bones, Model* model);
    static bool IsSkeletalAnimation(std::wstring* address);
    static bool LoadSkeletalAnimationFromFile(std::wstring* address, SSkeletonAnimation* anim);
    static bool InicializeSkeletalAnimation(std::wstring* name, SSkeletonAnimation* anim, std::vector<SkeletonNode*>* bones);
    static bool IsMeshAnimation(std::wstring* address);
    static bool LoadSkeleton(Model* model);

	static bool Mapping(Model *model, SSkeleton *mesh_skeleton);
    static bool MappingFromFmesh( Model *model, SSkeleton *mesh_skeleton);

    static void CreateHierarchyFmesh(Skeleton* temp);
    static void CalculateMatrixAndDataFmesh(Bone *bone);

    static void MirrorSkeleton(Skeleton* skeleton);
    static void ChangePlaces(Skeleton* skeleton, int from, int destination);

    static C3D_Data* ReadC3DFile(std::string filePath);
};

#endif //FILE_READER_2_MOTION_H