#ifndef BASE_SERVICE_H
#define BASE_SERVICE_H

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>

namespace osg 
{
    class Node;
} // namespace osg

namespace osgViewer 
{
    class CompositeViewer;
} // namespace osgViewer

namespace core 
{
    class IDataManager;
} // namespace core

enum AsyncResult
{
    AsyncResult_Failure,
    AsyncResult_Pending,
    AsyncResult_Complete,
};

class IServiceManager;
class IWidget;

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
	//! \param dataManager Manager zasob�w.
    virtual AsyncResult init(IServiceManager* serviceManager, core::IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer)
    {
        return AsyncResult_Complete;
    }

    //! �aduje 
    virtual AsyncResult loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
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


    //! Callback wywo�ywany gdy widget zniknie (np. b�dzie schowany w tabach
    //! albo z jakiego� innego powodu nie b�dzie widoczny). Docelowo powinna to
    //! by� metoda IWidget, ale poniewa� u nas mo�na by ten typ zast�pi� void*,
    //! metoda CZASOWO dodana do IService.
    //! W tej metodzie mo�na kontrolowa� w��czanie/wy��czanie pewnych cz�ci
    //! us�ug, np. rendering OSG.
    //! \param widget
    //! \param visible
    virtual void visibilityChanged(IWidget* widget, bool visible)
    {
    }

    //! Us�uga nie musi mie� wizualnej reprezentacji.
    //! \return Widget tworzony przez us�ug� b�d� NULL.
    virtual IWidget* getWidget() = 0;
    //! \return Widget kontroluj�cy zachowanie us�ugi/us�ug zale�nych.
    virtual IWidget* getControlWidget() { return NULL; }
    //! \return Widget dostarczaj�cy opcji zwi�zanych z us�ug�/us�ugami zale�nymi.
    virtual IWidget* getSettingsWidget() { return NULL; }
    //! \return Nazwa us�ugi.
    virtual const std::string& getName() const = 0;

    //! \return Korze� lokalnej sceny osg.
    virtual osg::Node* debugGetLocalSceneRoot()
    {
        return NULL;
    }

};

typedef core::shared_ptr<IService> IServicePtr;
typedef core::shared_ptr<const IService> IServiceConstPtr;
typedef core::weak_ptr<IService> IServiceWeakPtr;

#endif //BASE_SERVICE_H
