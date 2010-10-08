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
    virtual std::string GetVideoFilePath(int i);
    virtual int GetFilePathCount();

    virtual IModel* GetModel();

private:
    std::vector<std::string> m_VideoFilePathList;

    IModel* m_pModel;
};



#endif // DATA_MANAGER_H
