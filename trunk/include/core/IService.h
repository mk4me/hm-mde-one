#ifndef BASE_SERVICE_H
#define BASE_SERVICE_H

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/WidgetInterface.h>

// in h file
// #define M_DECLARE_CLASS()             \
// public:                               \
//     static const ClassID CLASS_ID;    

// in cpp file
/*#define M_DECLARED_CLASS(className, idvalue)  const ClassID className::CLASS_ID = idvalue;       */              

enum AsyncResult
{
    AsyncResult_Failure,
    AsyncResult_Pending,
    AsyncResult_Complete,
};


class IModel;
class IDataManager;

class IServiceManager;
class IService : public IIdentifiable
{
public:
    //! Us³uga nie musi mieæ wizualnej reprezentacji, wiêc nie zawsze jest
    //! potrzeba prze³adowywania tej metody.
    //! \return Widget tworzony przez us³ugê.
    virtual IWidget* getWidget()
    {
        return NULL;
    }


    virtual AsyncResult OnAdded(IServiceManager* serviceManager) {return AsyncResult_Complete; } 

    virtual void SetModel(IDataManager* dataManager)
    {        
    }

	virtual ~IService() {}


    //------------------------------------------------------------------------------
    // NOWY INTERFEJS US£UGI
 
    //! Inicjalizacja us³ugi. Nastêpuje ju¿ po wczytaniu pluginów i skonstruowaniu
    //! (nie zainicjalizowaniu!) wszystkich us³ug. Mo¿na wiêc pobieraæ wskaŸniki.
    virtual AsyncResult init(IServiceManager* serviceManager)
    {
        return AsyncResult_Complete;
    }

    //! £aduje 
    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager)
    {
        SetModel(dataManager);
        return AsyncResult_Complete;
    }

    //! Aktualizacja logiki us³ugi. Ten sam w¹tek co UI.
    //! \param serviceManager 
    virtual AsyncResult update(double time, double timeDelta) 
    { 
        return AsyncResult_Complete; 
    }

    //! Drugi przebieg aktualizacji logiki; us³ugi jako takie nie maj¹ wp³ywu na kolejnoœæ, z jak¹
    //! wywo³ywana jest metoda Update. Gdy jakaœ us³uga bazuje na pozosta³ych nale¿y wówczas
    //! u¿yæ metody LateUpdate, która jest wywo³ywana po cyklu Update wszystkich us³ug.
    //! Ten sam w¹tek co UI.
    //! \param serviceManager
    virtual AsyncResult lateUpdate(double time, double timeDelta) 
    { 
        return AsyncResult_Complete; 
    }

    //! Miejsce na bardziej skomplikowane obliczenia. W¹tek inny ni¿ dla UI, niekoniecznie taki
    //! sam dla wszystkich us³ug.
    virtual AsyncResult compute()
    {
        return AsyncResult_Complete;
    }
};

#endif //BASE_SERVICE_H
