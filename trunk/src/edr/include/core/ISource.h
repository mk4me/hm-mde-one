/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   8:58
    filename: ISource.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs ürÛd≥a danych mogπcy wstrzykiwaÊ dane poprzez obiekty domenowe do aplikacji. Ma dostÍp do MemoryDataManager i FileDataManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCE_H__
#define HEADER_GUARD_CORE__ISOURCE_H__

#include <vector>
#include <string>

#include <core/IIdentifiable.h>
#include <core/IDataManager.h>
#include <core/IActionsGroupManager.h>

class QObject;
class QWidget;

namespace core 
{
    //! Interfejs ürÛd≥a danych. Powinien to byÊ jedeny obiekt inicjowany i majπcy dostep do MemoryDM i FIleDM
    class ISource : public IIdentifiable
    {
    public:
        virtual ~ISource() {}

        //! Inicjalizacja ürÛd≥a. NastÍpuje juø po wczytaniu pluginÛw i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich ürÛde≥.
        virtual void init(IMemoryDataManager * memoryDM, IFileDataManager * fileDM) = 0;

        //! PÛüna inicjalizacja ürÛd≥a, nastÍpuje po wczytaniu i inicjalizacji wszystkich innych ürÛde≥
        virtual void lateInit()
        {

        }

        //! èrÛd≥o nie musi mieÊ wizualnej reprezentacji.
        //! \return Widget tworzony przez ürÛd≥o bπdü NULL.
        virtual QWidget* getWidget(IActionsGroupManager * actionsGroupManager) = 0;

        //! èrÛd≥o nie musi mieÊ widgeta konfigurujπcego.
        //! \return Widget tworzony przez ürÛd≥o bπdü NULL.
        virtual QWidget* getConfigurationWidget(IActionsGroupManager * actionsGroupManager)
        {
            return nullptr;
        }

        //! \return Nazwa ürÛd≥a.
        virtual std::string getName() const = 0;

    };

    typedef shared_ptr<ISource> ISourcePtr;
    typedef shared_ptr<const ISource> ISourceConstPtr;
    typedef weak_ptr<ISource> ISourceWeakPtr;

}

#endif //HEADER_GUARD_CORE__ISOURCE_H__