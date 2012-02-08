/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   8:58
    filename: ISource.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs �r�d�a danych mog�cy wstrzykiwa� dane poprzez obiekty domenowe do aplikacji. Ma dost�p do MemoryDataManager i FileDataManager
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
    //! Interfejs �r�d�a danych. Powinien to by� jedeny obiekt inicjowany i maj�cy dostep do MemoryDM i FIleDM
    class ISource : public IIdentifiable
    {
    public:
        virtual ~ISource() {}

        //! Inicjalizacja �r�d�a. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich �r�de�.
        virtual void init(IMemoryDataManager * memoryDM, IFileDataManager * fileDM) = 0;

        //! P�na inicjalizacja �r�d�a, nast�puje po wczytaniu i inicjalizacji wszystkich innych �r�de�
        virtual void lateInit()
        {

        }

        //! �r�d�o nie musi mie� wizualnej reprezentacji.
        //! \return Widget tworzony przez �r�d�o b�d� NULL.
        virtual QWidget* getWidget(IActionsGroupManager * actionsGroupManager) = 0;

        //! �r�d�o nie musi mie� widgeta konfiguruj�cego.
        //! \return Widget tworzony przez �r�d�o b�d� NULL.
        virtual QWidget* getConfigurationWidget(IActionsGroupManager * actionsGroupManager)
        {
            return nullptr;
        }

        //! \return Nazwa �r�d�a.
        virtual std::string getName() const = 0;

    };

    typedef shared_ptr<ISource> ISourcePtr;
    typedef shared_ptr<const ISource> ISourceConstPtr;
    typedef weak_ptr<ISource> ISourceWeakPtr;

}

#endif //HEADER_GUARD_CORE__ISOURCE_H__