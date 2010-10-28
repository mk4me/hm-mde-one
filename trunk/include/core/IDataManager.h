#ifndef I_DATA_MANAGER_H
#define I_DATA_MANAGER_H

#include <string>

class IModel;

class IDataManager
{
public:
    virtual ~IDataManager() {};

    virtual void GetVideoFile() = 0;
    virtual void Clear() = 0;

    virtual std::string GetFileName() = 0;
    virtual std::string GetVideoFilePath(int i) = 0;
    virtual int GetVideoFilePathCount() = 0;

    virtual std::string GetSkeletonFilePath(int i) = 0;
    virtual int GetSkeletonFilePathCount() = 0;

    virtual std::string GetAnimationFilePath(int i) = 0;
    virtual int GetAnimationFilePathCount() = 0;

    virtual std::string GetMeshFilePathPath(int i) = 0;
    virtual int GetMeshFilePathCount() = 0;

    virtual IModel* GetModel() = 0;
};

#endif // I_DATA_MANAGER_H