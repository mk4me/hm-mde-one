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

namespace plugin 
{
	class ISourceManager;
	class IDataSourceManager;
	class IDataProcessorManager;
	class IDataSinkManager;
	class IVisualizerManager;
	class IMemoryDataManager;
	class IFileDataManager;
	class IActionsGroupManager;

    class IService : public IIdentifiable
    {
    public:
        virtual ~IService() {}
 
        //! Inicjalizacja usługi. Następuje już po wczytaniu pluginów i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich usług.
		virtual void init(ISourceManager * sourceManager,
			IDataSourceManager * dataSourceManager,
			IDataProcessorManager *dataProcessorManager,
			IDataSinkManager * dataSinkManager,
			IVisualizerManager * visualizerManager,
			IMemoryDataManager * memoryDataManager,
			IFileDataManager * fileDataManager) = 0;

        //! Póxna inicjalizacja usług, następuje po wczytaniu i inicjalizacji wszystkich usług
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

        //! Usługa nie musi mieć wizualnej reprezentacji.
        //! \return Widget tworzony przez usługę bądź NULL.
        virtual QWidget* getWidget(IActionsGroupManager * actionsManager) = 0;
        
        //! \return Widget kontrolujący zachowanie usługi/usług zależnych.
        virtual QWidget* getControlWidget(IActionsGroupManager * actionsManager)
        {
            return nullptr;
        }

        //! \return Widget dostarczający opcji związanych z usługą/usługami zależnymi.
        virtual QWidget* getSettingsWidget(IActionsGroupManager * actionsManager)
        {
            return nullptr;
        }

        //! \return Nazwa usługi.
        virtual const std::string& getName() const = 0;

    };

    typedef core::shared_ptr<IService> IServicePtr;
    typedef core::shared_ptr<const IService> IServiceConstPtr;
    typedef core::weak_ptr<IService> IServiceWeakPtr;

}

#endif //HEADER_GUARD___ISERVICE_H__
