#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <core/IParser.h>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/type_traits.hpp>
#include <utils/Utils.h>

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

    //! Wyszukiwanie zasob�w na lokalnym dysku.
    virtual void findResources() = 0;
    //! Wyszukiwanie pr�b pomiarowych na lokalnym dysku.
    virtual void findLocalTrials() = 0;
    //! �adowanie zasob�w znalezionych na dysku.
 	virtual void loadResources() = 0;
    //! �adowanie pr�by pomiarowej z lokalnego dysku.
    //! \param trial pr�ba pomiarowa do za�adowania.
    virtual void loadTrial(const LocalTrial& trial) = 0;
    //! Zwraca i-t� pr�b� pomiarow� z listy kolalnych pr�b pomiarowych.
    //! \param i indeks.
    //! \return i-ta pr�ba pomiarowa.
	virtual const LocalTrial& getLocalTrial(int i) const = 0;
    //! Czy�ci zasoby i pr�by pomiarowe.
	virtual void clear() = 0;
    //! Zwraca i-t� �cie�k� do sk�rki UI aplikacji.
    //! \param i indeks.
    //! \return �cie�ka do sr�rki aplikacji.
    virtual const std::string& getApplicationSkinsFilePath(int i) = 0;
    //! \return ilo�� sk�rek aplikacji.
    virtual int getApplicationSkinsFilePathCount() = 0;
    //! \return czy za�adowano pr�b� pomiarow�.
	virtual bool isLoadLocalTrialData() const = 0;
    //! \param load ustaw flag� �adowania lokalnej pr�by pomiarowej.
	virtual void setLoadLocalTrialData(bool load) = 0;
    //! \return �cie�ka do katalogu z zasobami
	virtual const Path& getResourcesPath() const = 0;
    //! \return �cie�ka do katalogu z pr�bami pomiarowymi
	virtual const Path& getTrialsPath() const = 0;

    //! \return Liczba parser�w.
    UTILS_DEPRECATED(virtual int getNumParsers() const = 0);
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Zainicjowany b�d� nie.
	UTILS_DEPRECATED(virtual core::IParserPtr getParser(int idx) = 0);
    //! \return Zainicjowany parser o indeksie idx.
    UTILS_DEPRECATED(virtual core::IParserPtr getInitializedParser(int idx) = 0);
    //! \param filename nazwa pliku parsera. 
    //! \return Odnaleziony parser (zainicjowany b�d� nie) b�d� NULL.
    UTILS_DEPRECATED(virtual core::IParserPtr getParser(const std::string& filename) = 0);
    //! \param filename nazwa pliku parsera. 
    //! \return Odnaleziony parser (zainicjowany b�d� nie) b�d� NULL.
    UTILS_DEPRECATED(virtual core::IParserPtr getInitializedParser(const std::string& filename) = 0);

    //! \param type Typ obiektu kt�ry chcemy wyszuka�.
    //! \param exact Czy wyszukiwa� te� typy pochodne?
    //! \return Lista obiekt�w wype�niaj�cych zadane kryterium.
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects, const std::type_info& type, bool exact = false) = 0;

    //! \return Czy zadane rozszerzenie jest wspierane?
    virtual bool isExtensionSupported(const std::string& extension) const = 0;
    
    //! Liczba pr�b pomiarowych w zasobach
	virtual int getLocalTrialsCount() const = 0;
    //! \param i za�aduj i-t� pr�b� pomiarow� z zasob�w
    virtual void loadLocalTrial(int i) = 0;
    //! \param path Za�aduj pr�b� pomiarow� z podanej �cie�ki.
    virtual void loadLocalTrial(const Path& path) = 0;
};
    
    //! \param manager Data manager.
    //! \param target Wektor wska�nik�w na obiekty. Wska�niki musz� by� konwertowalne z tego
    //!     zdefiniowanego w zasadach wska�nik�w dla wrappera.
    //! \param exact Czy maj� by� wyci�gane obiekty konkretnie tego typu (z pomini�ciem polimorfizmu)?
    template <class SmartPtr>
    inline void queryDataPtr(core::IDataManager* manager, std::vector<SmartPtr>& target, bool exact = false)
    {
        core::__queryDataIsConvertible<typename SmartPtr::element_type, SmartPtr>(manager, target, exact, boost::true_type());
    }

    //! \param manager Data manager.
    //! \param exact Czy maj� by� wyci�gane obiekty konkretnie tego typu (z pomini�ciem polimorfizmu)?
    //! \return Wektor wska�nik�w na obiekty. Wska�niki musz� by� konwertowalne z tego
    //!     zdefiniowanego w zasadach wska�nik�w dla wrappera.
    template <class SmartPtr>
    inline std::vector<SmartPtr> queryDataPtr(core::IDataManager* manager, bool exact = false, SmartPtr* /*dummy*/ = nullptr)
    {
        std::vector<SmartPtr> target;
        queryDataPtr<SmartPtr>(manager, target, exact);
        return target;
    }

    //! Zastosowanie idiomu "Return Type Resolver" do unikania podw�jnego okre�lania typu
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
        //! \return Kolekcja wska�nik�w.
        template <class SmartPtr>
        inline operator std::vector<SmartPtr>()
        {
            std::vector<SmartPtr> result;
            queryDataPtr<SmartPtr>(manager, result, exact);
            return result;
        }
    };

    //! Wersja funkcji queryData oparta o idiom "Return Type Resolver". Nie trzeba
    //! podawa� jawnie typu elementu kolekcji jako parametru szablonu.
    inline queryDataPtr_t queryDataPtr(core::IDataManager* manager, bool exact = false)
    {
        return queryDataPtr_t(manager, exact);
    }


    //! \param manager Data manager.
    //! \param target Wektor wska�nik�w na obiekty. Wska�niki musz� by� konwertowalne z tego
    //!     zdefiniowanego w zasadach wska�nik�w dla wrappera.
    //! \param exact Czy maj� by� wyci�gane obiekty konkretnie tego typu (z pomini�ciem polimorfizmu)?
    template <class T, class Ptr>
    inline void queryData(IDataManager* manager, std::vector<Ptr>& target, bool exact = false)
    {
        __queryDataIsConvertible<T, Ptr>(manager, target, exact, boost::is_convertible<Ptr, typename ObjectWrapperT<T>::Ptr>());
    }

    //! \param manager Data manager.
    //! \param target Wektor wska�nik�w na obiekty. Wska�niki musz� by� konwertowalne z tego
    //!     zdefiniowanego w zasadach wska�nik�w dla wrappera.
    //! \param exact Czy maj� by� wyci�gane obiekty konkretnie tego typu (z pomini�ciem polimorfizmu)?
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
        // wyci�gamy obiekty z wrapper�w
        for ( auto it = objects.begin(), last = objects.end(); it != last; ++it ) {
            target.push_back( (*it)->get<T>() );
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