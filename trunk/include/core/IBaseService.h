#ifndef BASE_SERVICE_H
#define BASE_SERVICE_H

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/WidgetInterface.h>

// in h file
#define M_DECLARE_CLASS()             \
public:                               \
    static const ClassID CLASS_ID;    

// in cpp file
#define M_DECLARED_CLASS(className, idvalue)  const ClassID className::CLASS_ID = idvalue;                     

enum AsyncResult
{
    AsyncResult_Failure,
    AsyncResult_Pending,
    AsyncResult_Complete,
};


class IModel;
class IDataManager;

class IServiceManager;
class IBaseService : public IIdentifiable
{
public:
    //! Us³uga nie musi mieæ wizualnej reprezentacji, wiêc nie zawsze jest
    //! potrzeba prze³adowywania tej metody.
    //! \return Widget tworzony przez us³ugê.
    virtual IWidget* getWidget()
    {
        return NULL;
    }

    //virtual AsyncResult Initialize() { return AsyncResult_Complete; } 
    //virtual AsyncResult PostInitialize() { return AsyncResult_Complete; } 

    virtual AsyncResult OnTick(double delta) { return AsyncResult_Complete; } 
    //virtual AsyncResult SetTimeUpdate(double currentTime) { return AsyncResult_Complete; } 

    virtual AsyncResult OnAdded(IServiceManager* serviceManager) {return AsyncResult_Complete; } 

    virtual void SetModel(IDataManager* dataManager) = 0;

	virtual ~IBaseService() {}
};

#endif //BASE_SERVICE_H
