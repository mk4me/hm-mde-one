#ifndef FILE_READER_2_MOTION_H
#define FILE_READER_2_MOTION_H

#include <string>
#include <vector>
#include <core/dae2Motion.h>

class ASFAMCParser;

class Model;
class SkeletonNode;
class DataManager;
class Skeleton;

struct SSkeletonAnimation;
struct SFModel;
struct SSkeleton;

struct Bone;

class FileReader2Motion
{
public:
	static void ReadFile(DataManager *dataManager);
	static bool LoadMesh(std::string address, Model* model);
    static bool LoadAnimation(SFModel* fmodel, Model* model);
    static bool LoadAnimation(ASFAMCParser* acclaimObject, Model* model);

    static bool LoadAnimationFromFAnnimation(std::string address, Model* model);

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
};

#endif //FILE_READER_2_MOTION_H