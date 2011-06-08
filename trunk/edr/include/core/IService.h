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
    class IDataManager;


    class IServiceManager;


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
        virtual void init(IServiceManager* serviceManager, core::IDataManager* dataManager)
        {

        }

        // TODO: zostaje tylko do momentu wprowadzenia nowego timeline'a
        UTILS_DEPRECATED(virtual void loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
        {

        })

        //! Aktualizacja logiki us�ugi. Ten sam w�tek co UI.
        //! \param serviceManager 
        // TODO: zostaje tylko do momentu wprowadzenia nowego timeline'a
        UTILS_DEPRECATED(virtual void update(double time, double timeDelta) 
        { 

        })

        //! Drugi przebieg aktualizacji logiki; us�ugi jako takie nie maj� wp�ywu na kolejno��, z jak�
        //! wywo�ywana jest metoda Update. Gdy jaka� us�uga bazuje na pozosta�ych nale�y w�wczas
        //! u�y� metody LateUpdate, kt�ra jest wywo�ywana po cyklu Update wszystkich us�ug.
        //! Ten sam w�tek co UI.
        //! \param serviceManager
        // TODO: zostaje tylko do momentu wprowadzenia nowego timeline'a
        UTILS_DEPRECATED(virtual void lateUpdate(double time, double timeDelta) 
        { 

        })

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
