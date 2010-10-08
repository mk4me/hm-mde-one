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
    //! Us�uga nie musi mie� wizualnej reprezentacji, wi�c nie zawsze jest
    //! potrzeba prze�adowywania tej metody.
    //! \return Widget tworzony przez us�ug�.
    virtual IWidget* getWidget()
    {
        return NULL;
    }

    //virtual AsyncResult Initialize() { return AsyncResult_Complete; } 
    //virtual AsyncResult PostInitialize() { return AsyncResult_Complete; } 

    virtual AsyncResult OnTick(double delta) { return AsyncResult_Complete; } 
    //virtual AsyncResult SetTimeUpdate(double currentTime) { return AsyncResult_Complete; } 

    virtual AsyncResult OnAdded(IServiceManager* serviceManager) {return AsyncResult_Complete; } 

    virtual AsyncResult OnServicesAdded(IServiceManager* serviceManager) { return AsyncResult_Complete; }

    virtual void SetModel(IDataManager* dataManager) = 0;

	virtual ~IBaseService() {}


    //------------------------------------------------------------------------------
    // NOWY INTERFEJS US�UGI
 
    //! Aktualizacja logiki us�ugi. Ten sam w�tek co UI.
    //! \param serviceManager 
    virtual AsyncResult update(IServiceManager* serviceManager) 
    { 
        return AsyncResult_Complete; 
    }

    //! Drugi przebieg aktualizacji logiki; us�ugi jako takie nie maj� wp�ywu na kolejno��, z jak�
    //! wywo�ywana jest metoda Update. Gdy jaka� us�uga bazuje na pozosta�ych nale�y w�wczas
    //! u�y� metody LateUpdate, kt�ra jest wywo�ywana po cyklu Update wszystkich us�ug.
    //! Ten sam w�tek co UI.
    //! \param serviceManager
    virtual AsyncResult lateUpdate(IServiceManager* serviceManager) 
    { 
        return AsyncResult_Complete; 
    }

    //! Miejsce na bardziej skomplikowane obliczenia. W�tek inny ni� dla UI, niekoniecznie taki
    //! sam dla wszystkich us�ug.
    virtual AsyncResult compute(IServiceManager* serviceManager)
    {
        return AsyncResult_Complete;
    }
};

#endif //BASE_SERVICE_H
