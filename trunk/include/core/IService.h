#ifndef BASE_SERVICE_H
#define BASE_SERVICE_H

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/WidgetInterface.h>
#include <core/SmartPtr.h>
#include <osg/Node>
#include <osgViewer/CompositeViewer>

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
	//! \param dataManager Manager zasobów.
    virtual AsyncResult init(IServiceManager* serviceManager, IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer)
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


    //! Callback wywo³ywany gdy widget zniknie (np. bêdzie schowany w tabach
    //! albo z jakiegoœ innego powodu nie bêdzie widoczny). Docelowo powinna to
    //! byæ metoda IWidget, ale poniewa¿ u nas mo¿na by ten typ zast¹piæ void*,
    //! metoda CZASOWO dodana do IService.
    //! W tej metodzie mo¿na kontrolowaæ w³¹czanie/wy³¹czanie pewnych czêœci
    //! us³ug, np. rendering OSG.
    //! \param widget
    //! \param visible
    virtual void visibilityChanged(IWidget* widget, bool visible)
    {
    }

    //! Us³uga nie musi mieæ wizualnej reprezentacji.
    //! \return Widget tworzony przez us³ugê b¹dŸ NULL.
    virtual IWidget* getWidget() = 0;
    //! \return Widget kontroluj¹cy zachowanie us³ugi/us³ug zale¿nych.
    virtual IWidget* getControlWidget() { return NULL; }
    //! \return Widget dostarczaj¹cy opcji zwi¹zanych z us³ug¹/us³ugami zale¿nymi.
    virtual IWidget* getSettingsWidget() { return NULL; }
    //! \return Nazwa us³ugi.
    virtual const std::string& getName() const = 0;

    //! \return Korzeñ lokalnej sceny osg.
    virtual osg::Node* debugGetLocalSceneRoot()
    {
        return NULL;
    }

};

typedef CORE_SHARED_PTR(IService) IServicePtr;
typedef CORE_CONST_SHARED_PTR(IService) IServiceConstPtr;
typedef CORE_WEAK_PTR(IService) IServiceWeakPtr;

#endif //BASE_SERVICE_H
