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
#include <corelib/IIdentifiable.h>
#include <corelib/SmartPtr.h>

class QObject;
class QWidget;

namespace core {

	class ISourceManager;
	class IVisualizerManager;
	class IMemoryDataManager;
	class IStreamDataManager;
	class IFileDataManager;

}

namespace plugin 
{
    class IService : public ICoreElement
    {
    public:
 
        //! Inicjalizacja usługi. Następuje już po wczytaniu pluginów i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich usług.
		virtual void init(core::ISourceManager * sourceManager,
			core::IVisualizerManager * visualizerManager,
			core::IMemoryDataManager * memoryDataManager,
			core::IStreamDataManager * streamDataManager,
			core::IFileDataManager * fileDataManager) = 0;

        //! Późna inicjalizacja usług, następuje po wczytaniu i inicjalizacji wszystkich usług
		//! \return Czy usługa poprawnie zainicjalizowana czy nie - może brakuje innych usług zależnych i sama nie ma jak działać
		//! Dla false usługa zostanie wyładowana
        virtual const bool lateInit() = 0;

        //! Metoda powinna w bezpieczny sposób zwalniac zasoby, mając na uwadze że niekoniecznie wszystkie usługi i zasoby pobrane z zewnątrz są jeszcze dostępne.
        //! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, które uruchomił serwis, może tez zwalniac pamieć przydzieloną dynamicznie
        //! Generalnie to taki bezpieczny destruktor uniezależniający dana usługę od pozostałych usług i przygotowujący ja na usunięcie
        virtual void finalize() = 0;

        //! Metoda aktualizująca pochodzi z wątku UI! Powinny tu być realizowane lekkie operacje odświeżania widgetów!!
        //! Jako parametr dostajemy przyrost czasu jaki minał od poprzedniego wywołania
        virtual void update(double deltaTime) = 0;

        //! Usługa nie musi mieć wizualnej reprezentacji.
        //! \return Widget tworzony przez usługę bądź NULL.
        virtual QWidget* getWidget() = 0;
        
        //! \return Widget kontrolujący zachowanie usługi/usług zależnych.
        virtual QWidget* getControlWidget() = 0;

        //! \return Widget dostarczający opcji związanych z usługą/usługami zależnymi.
        virtual QWidget* getSettingsWidget() = 0;
    };

    typedef core::shared_ptr<IService> IServicePtr;
    typedef core::shared_ptr<const IService> IServiceConstPtr;
    typedef core::weak_ptr<IService> IServiceWeakPtr;

}

#endif //HEADER_GUARD___ISERVICE_H__
