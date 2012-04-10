#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <boost/type_traits.hpp>
#include <utils/Utils.h>
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

		//! \param key Klucz dla ktorego poszukujemy wartoœci
		//! \param value [out] Wartoœc dla poszukiweanego klucza
		//! \return Prawda jeœli uda³o siê znaleŸæ poszukiwany klucz
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
		//! \param i Indeks wartoœci z zakresu [0-size()-1]
		//! \return Wartoœc dla zadanego indeksu
		const std::string & value(int i) const
		{
			UTILS_ASSERT(i > -1 && i < size(), "B³êdny indeks");
			auto it = descriptor_.begin();
			std::advance(it, i);
			return it->second;
		}
		//! \param i Indeks klucza o który pytamy
		//! \return Wartoœæ klucza dla zadanego indeksu
		const std::string & key(int i) const
		{
			UTILS_ASSERT(i > -1 && i < size(), "B³êdny indeks");
			auto it = descriptor_.begin();
			std::advance(it, i);
			return it->first;
		}
		//! \param key Klucz który sprawdzamy czy opisuje obiekt
		//! \return Prawda jeœli klucz istnieje
		bool hasKey(const std::string & key) const
		{
			return descriptor_.find(key) != descriptor_.end();
		}
		//! \return Iloœæ par klucz->wartoœc opisuj¹cych obiekt
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

		//! \return Obiekt opisywany - s³aby smart_ptr -> usuwamy opisy w momencie usuniêcia obiektu
		const ObjectWrapperWeakPtr & object()
		{
			return object_;
		}

		//! \return Obiekt opisywany - s³aby smart_ptr -> usuwamy opisy w momencie usuniêcia obiektu
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

    //! Interfejs s³u¿¹ca do inicjalizacji/deinicjalizacji obiektów domenowych
    class IDataInitializer
    {
    public:
        virtual ~IDataInitializer() {};

        //! \param object Obiekt do inicjalizacji
        virtual void initialize(ObjectWrapperPtr & object) = 0;
        
        //! \param object Obiekt do deinicjalizacji
        void deinitialize(ObjectWrapperPtr & object)
        {
            //wywolujemy metode klienta do deinicjalizacji
            //nie musi bezposrednio resetowaæ wskaŸnika - zrobimy to tutaj sami jeœli trzeba
            doDeinitialize(object);
            if(object->isNull() == false){
                object->reset();
            }
        }

    protected:
        //! Metoda implementowana w kodzie klienckim, jesli tam wskaŸnik nie zostanie wyzerowany zrobimy to my
        //! \param object Obiekt do deinicjalizacji
        virtual void doDeinitialize(ObjectWrapperPtr & object) {}

    };

    typedef shared_ptr<IDataInitializer> DataInitializerPtr;

    //! Podstawowe operacje zwiazane z danymi - pobieranie danych, informacje o hierarchi typów danych, informacje o wspieranych typach danych
    class IDataManagerReader
    {
    public:
        virtual ~IDataManagerReader() {}

        //virtual ObjectWrapperPtr createWrapper(const TypeInfo & typeInfo) const = 0;

		//! \param objects [out] Obiekty pasuj¹ce do zapytania
		//! \param type Typ obniektów o które pytamy
		//! \param exact Czy typ musi byæ zgodny czy moga to byæ tez typy pochodne
        virtual void getObjects(std::vector<ObjectWrapperConstPtr> & objects, const TypeInfo & type, bool exact = true) = 0;
		//! \param objects [out] Obiekty zarz¹dzane przez DM
        virtual void getObjects(ObjectWrapperCollection& objects) = 0;

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
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer sie nie zgadza");

			manager->getMetadataForObject(manager->getObjectWrapperForRawPtr(data.get()), metadataCollection);
		}

		//! \param typeInfo Typ danych dla których chcemy pobrac wszystkie metadane
		//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
		//! \param exact Czy mamy do³anczaæ metadane typów pochodnych
		virtual void getMetadataForType(const TypeInfo & typeInfo, std::vector<ObjectWrapperConstPtr> & metadataCollection, bool exact = true) const = 0;

        //! \param rawPtr Surowy wskaŸnik do danych
        //! \return ObjectWrapperConstPtr opakowuj¹cy ten wskaŸnik lub pusty ObjectWrapperPtr jeœli nie ma takich danych w DataManager
        //virtual ObjectWrapperConstPtr getWrapper(void * rawPtr) const = 0;

        //! \return Zarejestrowane w aplikacji typy danych
        virtual const TypeInfoSet & getSupportedTypes() const = 0;

        //! \return Hierarchia typow danych - jakie operacje moge realizowac, po czym dziedzicze
        virtual const TypeInfoSet & getTypeBaseTypes(const TypeInfo & type) const = 0;

        //! \return Hierarchia typow danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i moze byc downcastowany na mnie
        virtual const TypeInfoSet & getTypeDerrivedTypes(const TypeInfo & type) const = 0;

	protected:

		//! \param ptr Surowy wskaŸnik, dla którego sprawdzamy czy jest juz za¿adzany przez DM
		//! \return prawda jeœli wskaŸnik jest zarz¹dzany prze DM i opakowano go w OW
		virtual bool objectIsManaged(const void * ptr) const = 0;

		//! \param ptr Surowy wskaŸnik, dla którego sprawdzamy czy jest juz za¿adzany przez DM
		//! \return OW opakowuj¹cy dany wskaŸnik
		virtual const ObjectWrapperPtr & getObjectWrapperForRawPtr(const void * ptr) const = 0;
    };

    //! Zbiór typów
    typedef std::set<TypeInfo> Types;

    //! Interfejs najni¿szego poziomu DataManager - pozwala zarz¹dzaæ pojedynczymi ObjectWrapperami: dodawaæ je do puli danych, usuwaæ, inicjalizowaæ, deinicjalizowaæ.
    //!  
    class IMemoryDataManager : public utils::Observable<IMemoryDataManager>, virtual public IDataManagerReader
    {
        ////! ZaprzyjaŸniona metoda pomagaj¹ca dodawaæ dane do DM
        //template<class SmartPtr>
        //friend core::ObjectWrapperPtr addData(IMemoryDataManager * manager, const SmartPtr & data, const DataInitializerPtr & initializer);

        ////! ZaprzyjaŸniona metoda pomagaj¹ca usuwaæ dane z DM
        //template<class SmartPtr>
        //friend void removeData(IMemoryDataManager * manager, const SmartPtr & data);

    public:

		template<class SmartPtr>
		static ObjectWrapperPtr addData(IMemoryDataManager * manager, const SmartPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr())
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer sie nie zgadza");

			if(manager->objectIsManaged(data.get()) == true){
				throw std::runtime_error("Object already managed by DataManager");
			}

			core::ObjectWrapperPtr objectWrapper(manager->createObjectWrapper(typeid(typename SmartPtr::element_type)));
			objectWrapper->set(data);

			manager->addData(objectWrapper, initializer);

			return objectWrapper;
		}

		template<class SmartPtr>
		static void removeData(IMemoryDataManager * manager, const SmartPtr & data)
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer sie nie zgadza");
			manager->removeData(manager->getObjectWrapperForRawPtr(data.get()));
		}

        virtual ~IMemoryDataManager() {};

        //! \param Obiekt ktory chcemy inicjalizowaæ
        virtual void initializeData(ObjectWrapperPtr & data) = 0;

        //! \param Obiekt dla ktorego chcemy wykonac deinicjalizacje
        //! \return Prawda jeœli obiekt posiada inicjalizator
        virtual bool isInitializable(const ObjectWrapperPtr & data) const = 0;

        //! \param Obiekt ktory chcemy deinicjalizowaæ - dalej jest w DataManager ale nie zawiera danych - trzeba potem inicjalizowaæ
        virtual void deinitializeData(ObjectWrapperPtr & data) = 0;

        //! \param Obiekt ktory zostanie usuniety jesli zarzadza nim DataManager
        virtual void removeData(const ObjectWrapperPtr & data) = 0;

    private:

        //! \param Obiekt ktory zostanie utrwalony w DataManager i bêdzie dostepny przy zapytaniach, nie morze byc niezainicjowany - isNull musi byæ false!!
        virtual void addData(const ObjectWrapperPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr()) = 0;

        //! \param type Typ dla którego tworzymy specyficzny dla DM ObjectWrapper
        //! \return OW dla zadanego typu
        virtual ObjectWrapperPtr createObjectWrapper(const TypeInfo & type) const = 0;
    };

    //! Interfejs dostepu do danych i ³adowania danych w aplikacji
	class IFileDataManager : public utils::Observable<IFileDataManager>, virtual public IDataManagerReader
	{
	public:
        //! Zbiór rozszerzeñ
        typedef std::set<std::string> Extensions;

        //! Opis rozszerzeñ
        struct ExtensionDescription
        {
            //! Zarejestrowane opisy
            std::vector<std::string> descriptions;
            //! Zarejestrowane typy
            Types types;
        };
        

		//! Wirtualny destruktor.
		virtual ~IFileDataManager() {};

        //! \param files Zbiór plików ktrymi aktualnie zarz¹dza ten DataManager
        virtual void getManagedFiles(Files & files) const = 0;
        
        //! \param file Plik kótry weryfikujemy czy jest zarzadzany przez DM
        //! \return Prawda jesli plik jest zarz¹dzany przez ten DM
        virtual bool isFileManaged(Filesystem::Path & file) const = 0;

        //! \param files Lista plików dla których zostan¹ utworzone parsery i z których wyci¹gniête dane
		//! \param objects [out] Agregat obiektów wyci¹gniêtych z danego pliku przez parsery
        //! bêda dostepne poprzez DataMangera LENIWA INICJALIZACJA
		virtual void addFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects = std::vector<ObjectWrapperPtr>()) = 0;

        //! \param files Lista plików które zostan¹ usuniête z aplikacji a wraz z nimi skojarzone parsery i dane
		virtual void removeFile(const Filesystem::Path & file) = 0;

        //! \param path Œciezka pliku który chemy za³adowaæ (parsowaæ) WYMUSZAMY PARSOWANIE I INICJALIZACJE
        virtual void initializeFile(const Filesystem::Path & file) = 0;

        //! \param path Œciezka pliku który chemy za³adowaæ (parsowaæ) ZWALNIAMY ZASOBY, ALE ZEZWALAMY PONOWNIE NA LENIWA INICJALIZACJE
        virtual void deinitializeFile(const Filesystem::Path & file) = 0;

        //! \param files Zbior plikow dla ktorych chcemy pobrac liste obiektow
        //! \return Mapa obiektow wzgledem plikow z ktorych pochodza
        virtual void getObjectsForFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects) const = 0;

        //! \return Zbior obslugiwanych rozszerzen plikow wraz z ich opisem
        virtual const Extensions & getSupportedFilesExtensions() const = 0;

        //! \return true jeœli rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
        virtual bool isExtensionSupported(const std::string & extension) const = 0;

        //! \param extension Rozszerzenie dla którego pytamy o opis
        //! \return Opis rozszerzenia
        virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const = 0;
	};    

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

CORE_DEFINE_WRAPPER(core::Metadata, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

#endif // I_DATA_MANAGER_H