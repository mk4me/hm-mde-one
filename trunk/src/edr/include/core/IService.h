/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   11:39
    filename: IService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISERVICE_H__
#define HEADER_GUARD___ISERVICE_H__

#include <utils/Debug.h>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>

class QObject;
class QWidget;

namespace core 
{
	class IManagersAccessor;
	class IActionsGroupManager;

    class IService : public IIdentifiable
    {
    public:
	    virtual ~IService() {}
 
        //! Inicjalizacja us³ugi. Nastêpuje ju¿ po wczytaniu pluginów i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich us³ug.
        virtual void init(IManagersAccessor * managersAccessor) = 0;

        //! Póxna inicjalizacja us³ug, nastêpuje po wczytaniu i inicjalizacji wszystkich us³ug
        virtual void lateInit()
        {

        }

        //! Metoda powinna w bezpieczny sposob zwalniac zasoby, majac na uwadze ¿e niekoniecznie wszystkie uslugi i zasoby pobrane z zewnatrz sa jeszcze dostepne.
        //! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, które uruchomi³ serwis, moze tez zwalniac pamieæ przydzielon¹ dynamicznie
        //! Generalnie to taki bezpieczny destruktor uniezalezniajacy dana usluge od pozostalych uslug i przygotowujacy ja na usuniecie
        virtual void finalize()
        {

        }

        //! Metoda aktualizuj¹ca pochodzi z w¹tku UI! Powinny tu byæ realizowane lekkie operacje odœwie¿ania widgetów!!
        //! Jako parametr dostajemy przyrost czasu jaki mina³ od poprzedniego wywo³ania
        virtual void update(double deltaTime)
        {
       
        }

        //! Us³uga nie musi mieæ wizualnej reprezentacji.
        //! \return Widget tworzony przez us³ugê b¹dŸ NULL.
        virtual QWidget* getWidget(IActionsGroupManager * actionsManager) = 0;
        
        //! \return Widget kontroluj¹cy zachowanie us³ugi/us³ug zale¿nych.
        virtual QWidget* getControlWidget(IActionsGroupManager * actionsManager)
        {
            return nullptr;
        }

        //! \return Widget dostarczaj¹cy opcji zwi¹zanych z us³ug¹/us³ugami zale¿nymi.
        virtual QWidget* getSettingsWidget(IActionsGroupManager * actionsManager)
        {
            return nullptr;
        }

        //! \return Nazwa us³ugi.
        virtual const std::string& getName() const = 0;

    };

    typedef shared_ptr<IService> IServicePtr;
    typedef shared_ptr<const IService> IServiceConstPtr;
    typedef weak_ptr<IService> IServiceWeakPtr;

}

#endif //HEADER_GUARD___ISERVICE_H__
