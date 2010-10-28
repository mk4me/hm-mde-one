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
    // NOWY INTERFEJS US�UGI
 
    //! Inicjalizacja us�ugi. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
    //! (nie zainicjalizowaniu!) wszystkich us�ug. Mo�na wi�c pobiera� wska�niki.
    //! \param serviceManager Manager us�ug.
    //! \param root Korze� wsp�lnej sceny 3D.
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot)
    {
        return AsyncResult_Complete;
    }

    //! �aduje 
    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager)
    {
        return AsyncResult_Complete;
    }

    //! Aktualizacja logiki us�ugi. Ten sam w�tek co UI.
    //! \param serviceManager 
    virtual AsyncResult update(double time, double timeDelta) 
    { 
        return AsyncResult_Complete; 
    }

    //! Drugi przebieg aktualizacji logiki; us�ugi jako takie nie maj� wp�ywu na kolejno��, z jak�
    //! wywo�ywana jest metoda Update. Gdy jaka� us�uga bazuje na pozosta�ych nale�y w�wczas
    //! u�y� metody LateUpdate, kt�ra jest wywo�ywana po cyklu Update wszystkich us�ug.
    //! Ten sam w�tek co UI.
    //! \param serviceManager
    virtual AsyncResult lateUpdate(double time, double timeDelta) 
    { 
        return AsyncResult_Complete; 
    }

    //! Miejsce na bardziej skomplikowane obliczenia. W�tek inny ni� dla UI, niekoniecznie taki
    //! sam dla wszystkich us�ug.
    virtual AsyncResult compute()
    {
        return AsyncResult_Complete;
    }

    //! Us�uga nie musi mie� wizualnej reprezentacji.
    //! \return Widget tworzony przez us�ug� b�d� NULL.
    virtual IWidget* getWidget() = 0;

    //! \return Nazwa us�ugi.
    virtual const std::string& getName() const = 0;

};

typedef CORE_SHARED_PTR(IService) IServicePtr;
typedef CORE_CONST_SHARED_PTR(IService) IServiceConstPtr;

#endif //BASE_SERVICE_H
