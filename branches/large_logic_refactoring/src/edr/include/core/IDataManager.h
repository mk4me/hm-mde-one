#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <boost/type_traits.hpp>
#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>
#include <core/Filesystem.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

	//! Typ metadanych dla OW
	class Metadata {
	public:

		Metadata(const ObjectWrapperWeakPtr & object, const std::map<std::string, std::string> & descriptor = std::map<std::string, std::string>())
			: descriptor_(descriptor), object_(object), constObject_(object)
		{
			if(object_.lock() == nullptr || object_.expired() == true){
				throw std::runtime_error("Invalid object - empty or expired");
			}
		}

		Metadata(const Metadata & metadata) : object_(metadata.object_), constObject_(metadata.constObject_), descriptor_(metadata.descriptor_) {}

		//! Wirtualny destruktor
		virtual ~Metadata() {}

		//! \param key Klucz dla którego poszukujemy wartości
		//! \param value [out] Wartośc dla poszukiweanego klucza
		//! \return Prawda jeśli udało się znaleźć poszukiwany klucz
		bool value(const std::string & key, std::string & value) const
		{
			bool ret = false;
			auto it = descriptor_.find(key);
			if(it != descriptor_.end()){
				ret = true;
				value = it->second;
			}

			return ret;
		}
		//! \param i Indeks wartości z zakresu [0-size()-1]
		//! \return Wartośc dla zadanego indeksu
		const std::string & value(int i) const
		{
			UTILS_ASSERT(i > -1 && i < size(), "Błędny indeks");
			auto it = descriptor_.begin();
			std::advance(it, i);
			return it->second;
		}
		//! \param i Indeks klucza o który pytamy
		//! \return Wartość klucza dla zadanego indeksu
		const std::string & key(int i) const
		{
			UTILS_ASSERT(i > -1 && i < size(), "Błędny indeks");
			auto it = descriptor_.begin();
			std::advance(it, i);
			return it->first;
		}
		//! \param key Klucz który sprawdzamy czy opisuje obiekt
		//! \return Prawda jeśli klucz istnieje
		bool hasKey(const std::string & key) const
		{
			return descriptor_.find(key) != descriptor_.end();
		}
		//! \return Ilość par klucz->wartośc opisujących obiekt
		int size() const
		{
			return descriptor_.size();
		}
		//! \return Czy opis jest pusty
		int empty() const
		{
			return descriptor_.empty();
		}

		void setValue(const std::string & key, const std::string & value)
		{
			descriptor_[key] = value;
		}

		void removeValue(const std::string & key)
		{
			descriptor_.erase(key);
		}

		//! \return Obiekt opisywany - słaby smart_ptr -> usuwamy opisy w momencie usunięcia obiektu
		const ObjectWrapperWeakPtr & object()
		{
			return object_;
		}

		//! \return Obiekt opisywany - słaby smart_ptr -> usuwamy opisy w momencie usunięcia obiektu
		const ObjectWrapperConstWeakPtr & object() const
		{
			return constObject_;
		}

	private:
		//! Opis obiektu
		std::map<std::string, std::string> descriptor_;
		//! Opisywany obiekt
		ObjectWrapperWeakPtr object_;
		//! Opisywany obiekt
		ObjectWrapperConstWeakPtr constObject_;
	};

	typedef shared_ptr<Metadata> MetadataPtr;
	typedef shared_ptr<const Metadata> MetadataConstPtr;
	typedef weak_ptr<Metadata> MetadataWeakPtr;
	typedef weak_ptr<const Metadata> MetadataWeakConstPtr;

    //! Interfejs służąca do inicjalizacji/deinicjalizacji obiektów domenowych
    class IDataInitializer
    {
    public:
        virtual ~IDataInitializer() {};

        //! \param object Obiekt do inicjalizacji
        virtual void initialize(ObjectWrapperPtr & object) = 0;

        //! \param object Obiekt do deinicjalizacji
        void deinitialize(ObjectWrapperPtr & object)
        {
            //wywołujemy metode klienta do deinicjalizacji
            //nie musi bezpośrednio resetować wskaźnika - zrobimy to tutaj sami jeśli trzeba
            doDeinitialize(object);
            if(object->isNull() == false){
                object->reset();
            }
        }

    protected:
        //! Metoda implementowana w kodzie klienckim, jeśli tam wskaźnik nie zostanie wyzerowany zrobimy to my
        //! \param object Obiekt do deinicjalizacji
        virtual void doDeinitialize(ObjectWrapperPtr & object) {}

    };

    typedef shared_ptr<IDataInitializer> DataInitializerPtr;

    //! Podstawowe operacje związane z danymi - pobieranie danych, informacje o hierarchi typów danych, informacje o wspieranych typach danych
    class IDataManagerReader
    {
    public:
        virtual ~IDataManagerReader() {}

        //virtual ObjectWrapperPtr createWrapper(const TypeInfo & typeInfo) const = 0;

		//! \param objects [out] Obiekty pasujące do zapytania
		//! \param type Typ obniektów o które pytamy
		//! \param exact Czy typ musi być zgodny czy moga to być tez typy pochodne
		//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
        virtual void getObjects(std::vector<ObjectWrapperConstPtr> & objects, const TypeInfo & type, bool exact = true, bool initialize = true) = 0;
		//! \param objects [out] Obiekty zarządzane przez DM
		//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
        virtual void getObjects(ObjectWrapperCollection& objects, bool initialize = true) = 0;

		virtual bool isManaged(const ObjectWrapperConstPtr & object) const = 0;

		template<class SmartPtr>
		static bool isManaged(const IDataManagerReader * manager, const SmartPtr & data)
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename boost::remove_const<SmartPtr::element_type>::type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");

			return manager->objectIsManaged(data.get());
		}

		//! \param object Obiekt dla którego pobieram metadane
		//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
		virtual void getMetadataForObject(const ObjectWrapperConstPtr & object, std::vector<ObjectWrapperConstPtr> & metadataCollection) const = 0;

		template<class SmartPtr>
		static void getMetadataForObject(const IDataManagerReader * manager, const SmartPtr & data, std::vector<ObjectWrapperConstPtr> & metadataCollection)
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename boost::remove_const<SmartPtr::element_type>::type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer się nie zgadza");

			manager->getMetadataForObject(manager->getObjectWrapperForRawPtr(data.get()), metadataCollection);
		}

		//! \param typeInfo Typ danych dla których chcemy pobrać wszystkie metadane
		//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
		//! \param exact Czy mamy dołanczać metadane typów pochodnych
		virtual void getMetadataForType(const TypeInfo & typeInfo, std::vector<ObjectWrapperConstPtr> & metadataCollection, bool exact = true) const = 0;

        //! \param rawPtr Surowy wskaźnik do danych
        //! \return ObjectWrapperConstPtr opakowujący ten wskaźnik lub pusty ObjectWrapperPtr jeśli nie ma takich danych w DataManager
        //virtual ObjectWrapperConstPtr getWrapper(void * rawPtr) const = 0;

        //! \return Zarejestrowane w aplikacji typy danych
        virtual const TypeInfoSet & getSupportedTypes() const = 0;

        //! \return Hierarchia typów danych - jakie operacje moge realizować, po czym dziedzicze
        virtual const TypeInfoSet & getTypeBaseTypes(const TypeInfo & type) const = 0;

        //! \return Hierarchia typów danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i może być downcastowany na mnie
        virtual const TypeInfoSet & getTypeDerrivedTypes(const TypeInfo & type) const = 0;

		//! \param ptr Surowy wskaźnik, dla którego sprawdzamy czy jest już zażadzany przez DM
		//! \return prawda jeśli wskaźnik jest zarządzany prze DM i opakowano go w OW
		virtual bool objectIsManaged(const void * ptr) const = 0;

		//! \param ptr Surowy wskaźnik, dla którego sprawdzamy czy jest już zażadzany przez DM
		//! \return OW opakowujący dany wskaźnik
		virtual const ObjectWrapperPtr & getObjectWrapperForRawPtr(const void * ptr) const = 0;
    };

    //! Zbiór typów
    typedef std::set<TypeInfo> Types;

    //! Interfejs najniższego poziomu DataManager - pozwala zarządzać pojedynczymi ObjectWrapperami: dodawać je do puli danych, usuwać, inicjalizować, deinicjalizować.
    //!
    class IMemoryDataManager : public utils::Observable<IMemoryDataManager>, virtual public IDataManagerReader
    {

    public:
		//! Konstruktor domyślny
		IMemoryDataManager() : blockNotify_(false), changed(false)
		{

		}

		template<class SmartPtr>
		static ObjectWrapperPtr addData(IMemoryDataManager * manager, const SmartPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr())
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer się nie zgadza");

			if(manager->objectIsManaged(data.get()) == true){
				throw std::runtime_error("Object already managed by DataManager");
			}

			core::ObjectWrapperPtr objectWrapper(manager->createObjectWrapper(typeid(typename SmartPtr::element_type)));
			objectWrapper->set(data);

			manager->nonNotifyAddData(objectWrapper, initializer);
			manager->tryNotify();
			return objectWrapper;
		}

		template<class SmartPtr>
		static void removeData(IMemoryDataManager * manager, const SmartPtr & data)
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer się nie zgadza");
			manager->removeData(manager->getObjectWrapperForRawPtr(data.get()));
		}

        virtual ~IMemoryDataManager() {};

        //! \param Obiekt który chcemy inicjalizować
        virtual void initializeData(ObjectWrapperPtr & data) = 0;

        //! \param Obiekt dla którego chcemy wykonać deinicjalizacje
        //! \return Prawda jeśli obiekt posiada inicjalizator
        virtual bool isInitializable(const ObjectWrapperPtr & data) const = 0;

        //! \param Obiekt który chcemy deinicjalizować - dalej jest w DataManager ale nie zawiera danych - trzeba potem inicjalizować
        virtual void deinitializeData(ObjectWrapperPtr & data) = 0;

		//! Ta metoda notyfikuje o zmianie stanu DM!!
        //! \param Obiekt który zostanie usunięty jeśli zarządza nim DataManager
        void removeData(const ObjectWrapperPtr & data)
		{
			nonNotifyRemoveData(data);
			tryNotify();
		}

		//! \param block Czy blokować notyfikacje przy zarządzaniu danymi DM
		void blockNotify(bool block)
		{
			blockNotify_ = block;

			if(blockNotify_ == false && changed){
				changed = false;
				notify();
			}
		}
		//! \return Czy są blokowane aktualnie notyfikacje o zmianie stanu DM
		bool isNotifyBlocked() const
		{
			return blockNotify_;
		}

    private:
		//! Wewnętrzna próba realizacji notyfikacji jeśli nastąpiła zmiana i są włączone notyfikacje
		void tryNotify()
		{
			if(blockNotify_ == false){
				notify();
			}else{
				changed = true;
			}
		}

		//! Schowane bo DM wprowadza własną implementację OW z auto i nicjalizacją zasobów przy pierwszym pobraniu!! Nie upubliczniamy tego w żaden sposób!!
		//! Używamy tylko metod statycznych. Ta metoda nie notyfikuje o zmianie!!
        //! \param Obiekt który zostanie utrwalony w DataManager i będzie dostępny przy zapytaniach, nie może być niezainicjowany - isNull musi być false!!
        virtual void nonNotifyAddData(const ObjectWrapperPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr()) = 0;

		//! Metoda faktycznie usuwająca dane z DM ale nie notyfikująca o jego zmianie
		virtual void nonNotifyRemoveData(const ObjectWrapperPtr & data) = 0;

        //! \param type Typ dla którego tworzymy specyficzny dla DM ObjectWrapper
        //! \return OW dla zadanego typu
        virtual ObjectWrapperPtr createObjectWrapper(const TypeInfo & type) const = 0;

	private:
		//! Czy blokujemy notyfikacje
		bool blockNotify_;
		//! Czy nastąpiła zmiana w DM
		bool changed;

    };

    //! Interfejs dostępu do danych i ładowania danych w aplikacji
	class IFileDataManager : public utils::Observable<IFileDataManager>, virtual public IDataManagerReader
	{
	public:
        //! Zbiór rozszerzeń
        typedef std::set<std::string> Extensions;

        //! Opis rozszerzeń
        struct ExtensionDescription
        {
            //! Zarejestrowane opisy
            std::vector<std::string> descriptions;
            //! Zarejestrowane typy
            Types types;
        };
        //! Konstruktor domyślny
		IFileDataManager() : blockNotify_(false), changed(false)
		{

		}

		//! Wirtualny destruktor.
		virtual ~IFileDataManager() {};

        //! \param files Zbiór plików ktrymi aktualnie zarządza ten DataManager
        virtual void getManagedFiles(Files & files) const = 0;

        //! \param file Plik kótry weryfikujemy czy jest zarządzany przez DM
        //! \return Prawda jeśli plik jest zarządzany przez ten DM
        virtual bool isFileManaged(const Filesystem::Path & file) const = 0;

        //! \param files Lista plików dla których zostaną utworzone parsery i z których wyciągnięte dane
        //! będa dostępne poprzez DataMangera LENIWA INICJALIZACJA
		void addFile(const Filesystem::Path & file)
		{
		    std::vector<ObjectWrapperPtr> temp;
			nonNotifyAddFile(file, temp);
			tryNotify();
		}

        //! \param files Lista plików dla których zostaną utworzone parsery i z których wyciągnięte dane
		//! \param objects [out] Agregat obiektów wyciągniętych z danego pliku przez parsery
        //! będa dostępne poprzez DataMangera LENIWA INICJALIZACJA
		void addFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects)
		{
			nonNotifyAddFile(file, objects);
			tryNotify();
		}

        //! \param files Lista plików które zostaną usunięte z aplikacji a wraz z nimi skojarzone parsery i dane
		void removeFile(const Filesystem::Path & file)
		{
			nonNotifyRemoveFile(file);
			tryNotify();
		}

        //! \param path Ściezka pliku który chemy załadować (parsować) WYMUSZAMY PARSOWANIE I INICJALIZACJE
        virtual void initializeFile(const Filesystem::Path & file) = 0;

        //! \param path Ściezka pliku który chemy załadować (parsować) ZWALNIAMY ZASOBY, ALE ZEZWALAMY PONOWNIE NA LENIWA INICJALIZACJE
        virtual void deinitializeFile(const Filesystem::Path & file) = 0;

        //! \param files Zbior plików dla których chcemy pobrać listę obiektów
        //! \return Mapa obiektów względem plików z których pochodza
        virtual void getObjectsForFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects) const = 0;

        //! \return Zbior obsługiwanych rozszerzen plików wraz z ich opisem
        virtual const Extensions & getSupportedFilesExtensions() const = 0;

        //! \return true jeśli rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
        virtual bool isExtensionSupported(const std::string & extension) const = 0;

        //! \param extension Rozszerzenie dla którego pytamy o opis
        //! \return Opis rozszerzenia
        virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const = 0;

		void blockNotify(bool block)
		{
			blockNotify_ = block;

			if(blockNotify_ == false && changed){
				changed = false;
				notify();
			}
		}

		bool isNotifyBlocked() const
		{
			return blockNotify_;
		}

	protected:

		void tryNotify()
		{
			if(blockNotify_ == false){
				notify();
			}else{
				changed = true;
			}
		}

	private:

        // rev - prywatna i czysto wirtualna?
		//! Ta metoda nie notyfikuje o zmianie stanu DM!!
		//! \param files Lista plików dla których zostaną utworzone parsery i z których wyciągnięte dane
		//! \param objects [out] Agregat obiektów wyciągniętych z danego pliku przez parsery
		//! będa dostępne poprzez DataMangera LENIWA INICJALIZACJA
		virtual void nonNotifyAddFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects) = 0;

		//! Ta metoda nie notyfikuje o zmianie stanu DM!!
		//! \param files Lista plików które zostaną usunięte z aplikacji a wraz z nimi skojarzone parsery i dane
		virtual void nonNotifyRemoveFile(const Filesystem::Path & file) = 0;

	private:

		bool blockNotify_;
		bool changed;
	};

	//! Klasa pomocnicza do lokalnego blokowania notyfikacji w DM
	template<class T>
	class NotifyBlocker
	{
	public:
		//! Konstruktor
		//! \param t Obiekt którego notyfikacje chcemy lokalnie wyłączyć, ich stan zostanie przywrócony w momencie niszczenia tego obiektu
		NotifyBlocker(T & t) : t(&t), prevBlockingState(t.isNotifyBlocked())
		{
			t.blockNotify(true);
		}
		//! Destruktor - przywraca poprzedni stan notyfikcaji
		~NotifyBlocker()
		{
			t->blockNotify(false);
			t->blockNotify(prevBlockingState);
		}

	private:
		//! Obsługiwany obiekt
		T * t;
		//! Poprzedni stan blokowania notyfikacji
		bool prevBlockingState;

	};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

CORE_DEFINE_WRAPPER(core::Metadata, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

#endif // I_DATA_MANAGER_H
