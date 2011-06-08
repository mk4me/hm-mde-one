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

    //! Deklaracja wyprzedzaj¹ca
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
        // NOWY INTERFEJS US£UGI
 
        //! Inicjalizacja us³ugi. Nastêpuje ju¿ po wczytaniu pluginów i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich us³ug. Mo¿na wiêc pobieraæ wskaŸniki.
        //! \param serviceManager Manager us³ug.
        //! \param root Korzeñ wspólnej sceny 3D.
	    //! \param dataManager Manager zasobów.
        virtual void init(IServiceManager* serviceManager, core::IDataManager* dataManager)
        {

        }

        // TODO: zostaje tylko do momentu wprowadzenia nowego timeline'a
        UTILS_DEPRECATED(virtual void loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
        {

        })

        //! Aktualizacja logiki us³ugi. Ten sam w¹tek co UI.
        //! \param serviceManager 
        // TODO: zostaje tylko do momentu wprowadzenia nowego timeline'a
        UTILS_DEPRECATED(virtual void update(double time, double timeDelta) 
        { 

        })

        //! Drugi przebieg aktualizacji logiki; us³ugi jako takie nie maj¹ wp³ywu na kolejnoœæ, z jak¹
        //! wywo³ywana jest metoda Update. Gdy jakaœ us³uga bazuje na pozosta³ych nale¿y wówczas
        //! u¿yæ metody LateUpdate, która jest wywo³ywana po cyklu Update wszystkich us³ug.
        //! Ten sam w¹tek co UI.
        //! \param serviceManager
        // TODO: zostaje tylko do momentu wprowadzenia nowego timeline'a
        UTILS_DEPRECATED(virtual void lateUpdate(double time, double timeDelta) 
        { 

        })

        //! Us³uga nie musi mieæ wizualnej reprezentacji.
        //! \return Widget tworzony przez us³ugê b¹dŸ NULL.
        virtual QWidget* getWidget(std::vector<QObject*>& actions) = 0;
        
        //! \return Widget kontroluj¹cy zachowanie us³ugi/us³ug zale¿nych.
        virtual QWidget* getControlWidget(std::vector<QObject*>& actions)
        {
            return nullptr;
        }

        //! \return Widget dostarczaj¹cy opcji zwi¹zanych z us³ug¹/us³ugami zale¿nymi.
        virtual QWidget* getSettingsWidget(std::vector<QObject*>& actions)
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
