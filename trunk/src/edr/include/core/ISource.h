/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   8:58
    filename: ISource.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs �r�d�a danych mog�cy wstrzykiwa� dane poprzez obiekty domenowe do aplikacji. Ma dost�p do MemoryDataManager i FileDataManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCE_H__
#define HEADER_GUARD_CORE__ISOURCE_H__

#include <string>
#include <core/IIdentifiable.h>

class QObject;
class QWidget;

namespace core 
{

	class IMemoryDataManager;
	class IFileDataManager;
	class IServiceManager;
	class IActionsGroupManager;

    //! Interfejs �r�d�a danych. Powinien to by� jedeny obiekt inicjowany i maj�cy dostep do MemoryDM i FIleDM
    class ISource : public IIdentifiable
    {
    public:
        virtual ~ISource() {}

        //! Inicjalizacja �r�d�a. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich �r�de�.
        virtual void init(IMemoryDataManager * memoryDM, IFileDataManager * fileDM, IServiceManager * serviceManager) = 0;

        //! P�na inicjalizacja �r�d�a, nast�puje po wczytaniu i inicjalizacji wszystkich innych �r�de�
        virtual void lateInit()
        {

        }

		//! Metoda powinna w bezpieczny sposob zwalniac zasoby, majac na uwadze �e niekoniecznie wszystkie uslugi i zasoby pobrane z zewnatrz sa jeszcze dostepne.
        //! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, kt�re uruchomi� serwis, moze tez zwalniac pamie� przydzielon� dynamicznie
        //! Generalnie to taki bezpieczny destruktor uniezalezniajacy dana usluge od pozostalych uslug i przygotowujacy ja na usuniecie
        virtual void finalize()
        {

        }

        //! Metoda aktualizuj�ca pochodzi z w�tku UI! Powinny tu by� realizowane lekkie operacje od�wie�ania widget�w!!
        //! Jako parametr dostajemy przyrost czasu jaki mina� od poprzedniego wywo�ania
        virtual void update(double deltaTime)
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