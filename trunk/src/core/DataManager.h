#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <core/IDataManager.h>

class DataManager: public IDataManager
{
public:
    DataManager(std::string address);
    virtual ~DataManager();

    virtual void GetVideoFile();
    virtual void Clear();
    virtual std::string GetVideoFilePath(int i);
    virtual int GetFilePathCount();

private:
    std::vector<std::string> m_VideoFilePathList;
};



#endif // DATA_MANAGER_H
