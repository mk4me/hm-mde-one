#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

class IModel;
class IC3DModel;

class DataManager: public IDataManager
{
public:
    DataManager(std::string address, IModel* model, IC3DModel* c3dModel);
    virtual ~DataManager();

    virtual void GetVideoFile();
    virtual void Clear();
    virtual std::string GetFileName();
    virtual std::string GetVideoFilePath(int i);
    virtual int GetVideoFilePathCount();

    virtual std::string GetSkeletonFilePath(int i);
    virtual int GetSkeletonFilePathCount();

    virtual std::string GetAnimationFilePath(int i);
    virtual int GetAnimationFilePathCount();

    virtual std::string GetMeshFilePathPath(int i);
    virtual int GetMeshFilePathCount();

    virtual std::string GetC3dFilePath(int i);
    virtual int GetC3dFilePathCount();
    

    virtual IModel* GetModel();
    virtual IC3DModel* GetC3DModel();

private:
    std::vector<std::string> m_VideoFilePathList;
    std::vector<std::string> m_SkeletonFilePathList;
    std::vector<std::string> m_AnimationFilePathList;
    std::vector<std::string> m_MeshFilePathList;

    std::vector<std::string> m_c3dFilePathList;

    std::string m_pFileName;

    IModel* m_pModel;
    IC3DModel* m_pC3dModel;
};



#endif // DATA_MANAGER_H
