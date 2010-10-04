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

    virtual std::string GetVideoFilePath(int i) = 0;
    virtual int GetFilePathCount() = 0;

    virtual IModel* GetModel() = 0;
};

#endif // I_DATA_MANAGER_H