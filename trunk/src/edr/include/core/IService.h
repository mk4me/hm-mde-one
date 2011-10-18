/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   11:39
    filename: IService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISERVICE_H__
#define HEADER_GUARD___ISERVICE_H__

#include <vector>
#include <utils/Debug.h>
#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>

class QObject;
class QWidget;

namespace osg{

    //! Deklaracja wyprzedzaj�ca
    class Node;

}

namespace core 
{
    class IService : public IIdentifiable
    {
    public:
	    virtual ~IService() {}
 
        //! Inicjalizacja us�ugi. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich us�ug.
        virtual void init()
        {

        }

        //! P�xna inicjalizacja us�ug, nast�puje po wczytaniu i inicjalizacji wszystkich us�ug
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

        //! Us�uga nie musi mie� wizualnej reprezentacji.
        //! \return Widget tworzony przez us�ug� b�d� NULL.
        virtual QWidget* getWidget(std::vector<QObject*>& actions) = 0;
        
        //! \return Widget kontroluj�cy zachowanie us�ugi/us�ug zale�nych.
        virtual QWidget* getControlWidget(std::vector<QObject*>& actions)
        {
            return nullptr;
        }

        //! \return Widget dostarczaj�cy opcji zwi�zanych z us�ug�/us�ugami zale�nymi.
        virtual QWidget* getSettingsWidget(std::vector<QObject*>& actions)
        {
            return nullptr;
        }

        //! \return Nazwa us�ugi.
        virtual const std::string& getName() const = 0;

    };

    typedef shared_ptr<IService> IServicePtr;
    typedef shared_ptr<const IService> IServiceConstPtr;
    typedef weak_ptr<IService> IServiceWeakPtr;

}

#endif //HEADER_GUARD___ISERVICE_H__
