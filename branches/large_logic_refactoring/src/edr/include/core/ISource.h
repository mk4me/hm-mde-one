/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   8:58
    filename: ISource.h
    author:   Mateusz Janiak

    purpose:  Interfejs źródła danych mogący wstrzykiwać dane poprzez obiekty domenowe do aplikacji. Ma dostęp do MemoryDataManager i FileDataManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCE_H__
#define HEADER_GUARD_CORE__ISOURCE_H__

#include <string>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>

class QObject;
class QWidget;

namespace plugin
{

	class IMemoryDataManager;
	class IFileDataManager;
	class IServiceManager;
	class IActionsGroupManager;

    //! Interfejs źródła danych. Powinien to być jedeny obiekt inicjowany i mający dostęp do MemoryDM i FIleDM
    class ISource : public IIdentifiable
    {
    public:
        virtual ~ISource() {}

        //! Inicjalizacja źródła. Następuje już po wczytaniu pluginów i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich źródeł.
        virtual void init(IMemoryDataManager * memoryDM, IFileDataManager * fileDM, IServiceManager * serviceManager) = 0;

        //! Późna inicjalizacja źródła, następuje po wczytaniu i inicjalizacji wszystkich innych źródeł
        virtual void lateInit()
        {

        }

		//! Metoda powinna w bezpieczny sposób zwalniac zasoby, mając na uwadze że niekoniecznie wszystkie usługi i zasoby pobrane z zewnątrz są jeszcze dostępne.
        //! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, które uruchomił serwis, może tez zwalniac pamieć przydzieloną dynamicznie
        //! Generalnie to taki bezpieczny destruktor uniezależniający dana usługę od pozostałych usług i przygotowujący ja na usunięcie
        virtual void finalize()
        {

        }

        //! Metoda aktualizująca pochodzi z wątku UI! Powinny tu być realizowane lekkie operacje odświeżania widgetów!!
        //! Jako parametr dostajemy przyrost czasu jaki minał od poprzedniego wywołania
        virtual void update(double deltaTime)
        {

        }

        //! Źródło nie musi mieć wizualnej reprezentacji.
        //! \return Widget tworzony przez źródło bądź NULL.
        virtual QWidget* getWidget(IActionsGroupManager * actionsGroupManager) = 0;

        //! Źródło nie musi mieć widgeta konfigurującego.
        //! \return Widget tworzony przez źródło bądź NULL.
        virtual QWidget* getConfigurationWidget(IActionsGroupManager * actionsGroupManager)
        {
            return nullptr;
		}
    };

    typedef core::shared_ptr<ISource> ISourcePtr;
    typedef core::shared_ptr<const ISource> ISourceConstPtr;
    typedef core::weak_ptr<ISource> ISourceWeakPtr;

}

#endif //HEADER_GUARD_CORE__ISOURCE_H__
