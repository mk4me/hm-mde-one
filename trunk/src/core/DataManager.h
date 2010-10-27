#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

class IModel;

class DataManager: public IDataManager
{
public:
    DataManager(std::string address, IModel* model);
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

    virtual IModel* GetModel();

private:
    std::vector<std::string> m_VideoFilePathList;
    std::vector<std::string> m_SkeletonFilePathList;
    std::vector<std::string> m_AnimationFilePathList;
    std::string m_pFileName;

    IModel* m_pModel;
};



#endif // DATA_MANAGER_H
