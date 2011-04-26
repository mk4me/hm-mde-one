#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/type_traits.hpp>
#include <utils/Utils.h>
#include <core/ObjectWrapper.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class IDataManager
{
public:
    typedef boost::filesystem::path Path;
    typedef std::vector<Path> LocalTrial;

    //! Wirtualny destruktor.
	virtual ~IDataManager() {};

    virtual void addFiles(const std::vector<Path>& files) = 0;
    virtual void removeFiles(const std::vector<Path>& files) = 0;

    //! Wyszukiwanie zasobów na lokalnym dysku.
    virtual void findResources() = 0;
    //! Wyszukiwanie prób pomiarowych na lokalnym dysku.
    UTILS_DEPRECATED(virtual void findLocalTrials() = 0);
    //! £adowanie zasobów znalezionych na dysku.
 	virtual void loadResources() = 0;
    //! £adowanie próby pomiarowej z lokalnego dysku.
    //! \param trial próba pomiarowa do za³adowania.
    UTILS_DEPRECATED(virtual void loadTrial(const LocalTrial& trial) = 0);
    //! \param files lista do za³adowania, inicjalizacja parserów
    virtual void loadFiles(const std::vector<Path>& files) = 0;
    //! Zwraca i-t¹ próbê pomiarow¹ z listy kolalnych prób pomiarowych.
    //! \param i indeks.
    //! \return i-ta próba pomiarowa.
	UTILS_DEPRECATED(virtual const LocalTrial& getLocalTrial(int i) const = 0);
    //! Czyœci zasoby i próby pomiarowe.
	virtual void clear() = 0;
    //! \return czy za³adowano próbê pomiarow¹.
	UTILS_DEPRECATED(virtual bool isLoadLocalTrialData() const = 0);
    //! \param load ustaw flagê ³adowania lokalnej próby pomiarowej.
	UTILS_DEPRECATED(virtual void setLoadLocalTrialData(bool load) = 0);
    
    
    //! zwraca pelna sciezke do folderu "MyDocuments\EDR"
    virtual const Path& getUserDataPath() const = 0;
    //! zwraca pelna sciezke do folderu "ApplicationData\EDR"
    virtual const Path& getApplicationDataPath() const = 0;
    //! zwraca pelna sciezke do folderu z zasobami aplikacji
    virtual const Path& getResourcesPath() const = 0;


    //! \return œcie¿ka do katalogu z próbami pomiarowymi
	UTILS_DEPRECATED(virtual const Path& getTrialsPath() const = 0);

    //! \param type Typ obiektu który chcemy wyszukaæ.
    //! \param exact Czy wyszukiwaæ te¿ typy pochodne?
    //! \return Lista obiektów wype³niaj¹cych zadane kryterium.
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects, const std::type_info& type, bool exact = false) = 0;

    //! \return Czy zadane rozszerzenie jest wspierane?
    virtual bool isExtensionSupported(const std::string& extension) const = 0;
    
    //! Liczba prób pomiarowych w zasobach
	UTILS_DEPRECATED(virtual int getLocalTrialsCount() const = 0);
    //! \param i za³aduj i-t¹ próbê pomiarow¹ z zasobów
    UTILS_DEPRECATED(virtual void loadLocalTrial(int i) = 0);
    //! \param path Za³aduj próbê pomiarow¹ z podanej œcie¿ki.
    UTILS_DEPRECATED(virtual void loadLocalTrial(const Path& path) = 0);
};
    
    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class SmartPtr>
    inline void queryDataPtr(core::IDataManager* manager, std::vector<SmartPtr>& target, bool exact = false)
    {
        core::__queryDataIsConvertible<typename SmartPtr::element_type, SmartPtr>(manager, target, exact, boost::true_type());
    }

    //! \param manager Data manager.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    //! \return Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    template <class SmartPtr>
    inline std::vector<SmartPtr> queryDataPtr(core::IDataManager* manager, bool exact = false, SmartPtr* /*dummy*/ = nullptr)
    {
        std::vector<SmartPtr> target;
        queryDataPtr<SmartPtr>(manager, target, exact);
        return target;
    }

    //! Zastosowanie idiomu "Return Type Resolver" do unikania podwójnego okreœlania typu
    //! elementu kolekcji
    //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
    struct queryDataPtr_t
    {
        core::IDataManager* manager;
        bool exact;
        //! \param manager
        //! \param exact
        inline queryDataPtr_t(core::IDataManager* manager, bool exact = false) :
        manager(manager), exact(exact)
        {}
        //! \return Kolekcja wskaŸników.
        template <class SmartPtr>
        inline operator std::vector<SmartPtr>()
        {
            std::vector<SmartPtr> result;
            queryDataPtr<SmartPtr>(manager, result, exact);
            return result;
        }
    };

    //! Wersja funkcji queryData oparta o idiom "Return Type Resolver". Nie trzeba
    //! podawaæ jawnie typu elementu kolekcji jako parametru szablonu.
    inline queryDataPtr_t queryDataPtr(core::IDataManager* manager, bool exact = false)
    {
        return queryDataPtr_t(manager, exact);
    }


    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class T, class Ptr>
    inline void queryData(IDataManager* manager, std::vector<Ptr>& target, bool exact = false)
    {
        __queryDataIsConvertible<T, Ptr>(manager, target, exact, boost::is_convertible<Ptr, typename ObjectWrapperT<T>::Ptr>());
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaŸników dla wrappera.
    //! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
    template <class T>
    inline std::vector<typename ObjectWrapperT<T>::Ptr> queryData(IDataManager* manager, bool exact = false, T* /*dummy*/ = nullptr)
    {
        std::vector<typename ObjectWrapperT<T>::Ptr> target;
        queryData<T>(manager, target, exact);
        return target;
    }

    //! Przekierowanie z queryData dla poprawnych danych.
    template <class T, class Ptr>
    void __queryDataIsConvertible(IDataManager* manager, std::vector<Ptr>& target, bool exact, boost::true_type)
    {
        // pobieramy wrappery
        std::vector<ObjectWrapperPtr> objects;
        manager->getObjects(objects, typeid(T), exact);
        // wyci¹gamy obiekty z wrapperów
        for ( auto it = objects.begin(), last = objects.end(); it != last; ++it ) {
            target.push_back( static_cast<Ptr>((*it)->get(exact)) );
        }
    }

    //! Przekierowanie z queryData dla niepoprawnych danych.
    template <class T, class Ptr>
    void __queryDataIsConvertible(IDataManager*, std::vector<Ptr>&, bool, boost::false_type)
    {
        UTILS_STATIC_ASSERT( false, "Niewlasciwy typ elementu wektora lub niezdefiniowno wrap. Sprawdz CORE_DEFINE_WRAPPER dla poszukiwanego typu." );
    }

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif // I_DATA_MANAGER_H