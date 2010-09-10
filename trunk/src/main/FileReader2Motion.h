#ifndef FILE_READER_2_MOTION_H
#define FILE_READER_2_MOTION_H

#include <string>
#include <vector>

class Model;
class SkeletonNode;
struct SSkeletonAnimation;
struct SFModel;

class FileReader2Motion
{
public:
	static void ReadFile(const std::string& file, Model* model);
	static bool LoadMesh(std::wstring* address, Model* model);
    static bool LoadAnimation(SFModel* fmodel, Model* model);

private:
    static bool CreateArrayHoldingBones(std::vector<SkeletonNode*>* bones, Model* model);
    static bool IsSkeletalAnimation(std::wstring* address);
    static bool LoadSkeletalAnimationFromFile(std::wstring* address, SSkeletonAnimation* anim);
    static bool InicializeSkeletalAnimation(std::wstring* name, SSkeletonAnimation* anim, std::vector<SkeletonNode*>* bones);
    static bool IsMeshAnimation(std::wstring* address);
    static bool LoadSkeleton(Model* model);
};

#endif //FILE_READER_2_MOTION_H