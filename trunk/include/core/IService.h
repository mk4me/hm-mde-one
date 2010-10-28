#ifndef BASE_SERVICE_H
#define BASE_SERVICE_H

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/WidgetInterface.h>
#include <core/SmartPtr.h>

enum AsyncResult
{
    AsyncResult_Failure,
    AsyncResult_Pending,
    AsyncResult_Complete,
};

class IDataManager;
class IServiceManager;

class IService : public IIdentifiable
{
public:
	virtual ~IService() {}


    //------------------------------------------------------------------------------
    // NOWY INTERFEJS US£UGI
 
    //! Inicjalizacja us³ugi. Nastêpuje ju¿ po wczytaniu pluginów i skonstruowaniu
    //! (nie zainicjalizowaniu!) wszystkich us³ug. Mo¿na wiêc pobieraæ wskaŸniki.
    //! \param serviceManager Manager us³ug.
    //! \param root Korzeñ wspólnej sceny 3D.
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot)
    {
        return AsyncResult_Complete;
    }

    //! £aduje 
    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager)
    {
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

    //! Us³uga nie musi mieæ wizualnej reprezentacji.
    //! \return Widget tworzony przez us³ugê b¹dŸ NULL.
    virtual IWidget* getWidget() = 0;

    //! \return Nazwa us³ugi.
    virtual const std::string& getName() const = 0;

};

typedef CORE_SHARED_PTR(IService) IServicePtr;
typedef CORE_CONST_SHARED_PTR(IService) IServiceConstPtr;

#endif //BASE_SERVICE_H
