#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <vector>
#include <utils/Utils.h>
#include <utils/ObserverPattern.h>
#include <core/Filesystem.h>
#include <core/IPath.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
#include <boost/function.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    //! Interfejs dostepu do danych i ³adowania danych w aplikacji
	class IDataManager
	{
	public:
        typedef Filesystem::Path Path;
		typedef std::vector<Path> LocalTrial;
		typedef int DataProcessorPtr;

		//! Wirtualny destruktor.
		virtual ~IDataManager() {};

        //! \param files Lista plików dla których zostan¹ utworzone parsery i z których wyci¹gniête dane
        //! bêda dostepne poprzez DataMangera
		virtual void addFiles(const std::vector<Path>& files) = 0;

        //! \param files Lista plików które zostan¹ usuniête z aplikacji a wraz z nimi skojarzone parsery i dane
		virtual void removeFiles(const std::vector<Path>& files) = 0;

		//! \param files lista do za³adowania, inicjalizacja parserów
		virtual void loadFiles(const std::vector<Path>& files) = 0;
		//! \param files lista do za³adowania, inicjalizacja parserów
		//! \param types lista typow, ktore powinny zostac zaladowane
		virtual void loadFiles(const std::vector<Path>& files, const ObjectWrapper::Types& types) = 0;

		//! \return Czy zadane rozszerzenie jest wspierane?
		virtual bool isExtensionSupported(const std::string& extension) const = 0;
        
        // ------------------------- EXPERIMENTAL, DO NO USE -------------------------------
    
        //! \param objects Kolekcja obiektów spe³niaj¹cych zadane kryterium. Poprzez ten parametr zwracane s¹ wartoœci.
		//! \param type Typ obiektu który chcemy wyszukaæ.
		//! \param exact Czy wyszukiwaæ te¿ typy pochodne?
		virtual void getObjects(std::vector<ObjectWrapperPtr>& objects, const TypeInfo& type, bool exact = false) = 0;

        //! \param rawPtr Surowy wskaŸnik do obiektu domenowego
        //! \return ObjectWrapper skojarzony z tym obiektem domenowym, pusty OW jeœli obiekt nie zarejestrowany w DataManager
        virtual ObjectWrapperPtr getWrapper(void * rawPtr) const = 0;
    
        //! \param object ObjectWrapperPtr z nowymi danymi domenowymi wytworzonymi w czasie dzia³ania programu
        virtual void addExternalData(const ObjectWrapperPtr & object) = 0;
        
        //! \param object ObjectWrapperPtr do usuniecia wraz z danymi domenowymi, DataManager juz wiecej tych danych nie zwroci przy zapytaniu pasujacemu ich typowi
        virtual void removeExternalData(const ObjectWrapperPtr & object) = 0;

		virtual void addFileCallback(boost::function<void (const Filesystem::Path&, bool)> function) = 0;
		virtual void removeFileCallback(boost::function<void (const Filesystem::Path&, bool)> function) = 0;
		virtual void addWrappersCallback(boost::function<void (const std::vector<ObjectWrapperPtr>&, bool)> function) = 0;
		virtual void removeWrappersCallback(boost::function<void (const std::vector<ObjectWrapperPtr>&, bool)> function) = 0;
		
		//! Przeparsowanie danego wrappera
		virtual bool tryParseWrapper(ObjectWrapperPtr wrapper) = 0;
	};

////////////////////////////////////////////////////////////////////////////////
//              DO NOT USE CODE BELOWE - MIGHT BE CHANGED
////////////////////////////////////////////////////////////////////////////////


	//! \param manager Data manager.
	//! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
	//!     zdefiniowanego w zasadach wskaŸników dla wrappera.
	//! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
	template <class SmartPtr>
	inline void queryDataPtr(IDataManager* manager, std::vector<SmartPtr>& target, bool exact = false)
	{
		__queryDataIsConvertible<typename SmartPtr::element_type, SmartPtr>(manager, target, exact, boost::true_type());
	}

	//! \param manager Data manager.
	//! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
	//! \return Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
	//!     zdefiniowanego w zasadach wskaŸników dla wrappera.
	template <class SmartPtr>
	inline std::vector<SmartPtr> queryDataPtr(IDataManager* manager, bool exact = false, SmartPtr* /*dummy*/ = nullptr)
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
		IDataManager* manager;
		bool exact;
		//! \param manager
		//! \param exact
		inline queryDataPtr_t(IDataManager* manager, bool exact = false) :
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
	inline queryDataPtr_t queryDataPtr(IDataManager* manager, bool exact = false)
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
		UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
		__queryDataIsConvertible<T, Ptr>(manager, target, exact, boost::is_convertible<Ptr, typename ObjectWrapperT<T>::Ptr>());
	}

	//! \param manager Data manager.
	//! \param target Wektor wskaŸników na obiekty. WskaŸniki musz¹ byæ konwertowalne z tego
	//!     zdefiniowanego w zasadach wskaŸników dla wrappera.
	//! \param exact Czy maj¹ byæ wyci¹gane obiekty konkretnie tego typu (z pominiêciem polimorfizmu)?
	template <class T>
	inline std::vector<typename ObjectWrapperT<T>::Ptr> queryData(IDataManager* manager, bool exact = false, T* /*dummy*/ = nullptr)
	{
		UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
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