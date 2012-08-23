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

		//! \param key Klucz dla kt�rego poszukujemy warto�ci
		//! \param value [out] Warto�c dla poszukiweanego klucza
		//! \return Prawda je�li uda�o si� znale�� poszukiwany klucz
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
		//! \param i Indeks warto�ci z zakresu [0-size()-1]
		//! \return Warto�c dla zadanego indeksu
		const std::string & value(int i) const
		{
			UTILS_ASSERT(i > -1 && i < size(), "B��dny indeks");
			auto it = descriptor_.begin();
			std::advance(it, i);
			return it->second;
		}
		//! \param i Indeks klucza o kt�ry pytamy
		//! \return Warto�� klucza dla zadanego indeksu
		const std::string & key(int i) const
		{
			UTILS_ASSERT(i > -1 && i < size(), "B��dny indeks");
			auto it = descriptor_.begin();
			std::advance(it, i);
			return it->first;
		}
		//! \param key Klucz kt�ry sprawdzamy czy opisuje obiekt
		//! \return Prawda je�li klucz istnieje
		bool hasKey(const std::string & key) const
		{
			return descriptor_.find(key) != descriptor_.end();
		}
		//! \return Ilo�� par klucz->warto�c opisuj�cych obiekt
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

		//! \return Obiekt opisywany - s�aby smart_ptr -> usuwamy opisy w momencie usuni�cia obiektu
		const ObjectWrapperWeakPtr & object()
		{
			return object_;
		}

		//! \return Obiekt opisywany - s�aby smart_ptr -> usuwamy opisy w momencie usuni�cia obiektu
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

    //! Interfejs s�u��ca do inicjalizacji/deinicjalizacji obiekt�w domenowych
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
            //nie musi bezposrednio resetowa� wska�nika - zrobimy to tutaj sami je�li trzeba
            doDeinitialize(object);
            if(object->isNull() == false){
                object->reset();
            }
        }

    protected:
        //! Metoda implementowana w kodzie klienckim, je�li tam wska�nik nie zostanie wyzerowany zrobimy to my
        //! \param object Obiekt do deinicjalizacji
        virtual void doDeinitialize(ObjectWrapperPtr & object) {}

    };

    typedef shared_ptr<IDataInitializer> DataInitializerPtr;

    //! Podstawowe operacje zwi�zane z danymi - pobieranie danych, informacje o hierarchi typ�w danych, informacje o wspieranych typach danych
    class IDataManagerReader
    {
    public:
        virtual ~IDataManagerReader() {}

        //virtual ObjectWrapperPtr createWrapper(const TypeInfo & typeInfo) const = 0;

		//! \param objects [out] Obiekty pasuj�ce do zapytania
		//! \param type Typ obniekt�w o kt�re pytamy
		//! \param exact Czy typ musi by� zgodny czy moga to by� tez typy pochodne
		//! \param initialzie Czy automatycznie zainicjalizowa� wszystkie obiekty? UWAGA - te, kt�rych si� nie uda�o zainicjalizowa� nie zostan� zwr�cone - DM usunie je jako popsute dane
        virtual void getObjects(std::vector<ObjectWrapperConstPtr> & objects, const TypeInfo & type, bool exact = true, bool initialize = true) = 0;
		//! \param objects [out] Obiekty zarz�dzane przez DM
		//! \param initialzie Czy automatycznie zainicjalizowa� wszystkie obiekty? UWAGA - te, kt�rych si� nie uda�o zainicjalizowa� nie zostan� zwr�cone - DM usunie je jako popsute dane
        virtual void getObjects(ObjectWrapperCollection& objects, bool initialize = true) = 0;

		virtual bool isManaged(const ObjectWrapperConstPtr & object) const = 0;

		template<class SmartPtr>
		static bool isManaged(const IDataManagerReader * manager, const SmartPtr & data)
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename boost::remove_const<SmartPtr::element_type>::type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");

			return manager->objectIsManaged(data.get());
		}

		//! \param object Obiekt dla kt�rego pobieram metadane
		//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
		virtual void getMetadataForObject(const ObjectWrapperConstPtr & object, std::vector<ObjectWrapperConstPtr> & metadataCollection) const = 0;

		template<class SmartPtr>
		static void getMetadataForObject(const IDataManagerReader * manager, const SmartPtr & data, std::vector<ObjectWrapperConstPtr> & metadataCollection)
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename boost::remove_const<SmartPtr::element_type>::type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer si� nie zgadza");

			manager->getMetadataForObject(manager->getObjectWrapperForRawPtr(data.get()), metadataCollection);
		}

		//! \param typeInfo Typ danych dla kt�rych chcemy pobra� wszystkie metadane
		//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
		//! \param exact Czy mamy do�ancza� metadane typ�w pochodnych
		virtual void getMetadataForType(const TypeInfo & typeInfo, std::vector<ObjectWrapperConstPtr> & metadataCollection, bool exact = true) const = 0;

        //! \param rawPtr Surowy wska�nik do danych
        //! \return ObjectWrapperConstPtr opakowuj�cy ten wska�nik lub pusty ObjectWrapperPtr je�li nie ma takich danych w DataManager
        //virtual ObjectWrapperConstPtr getWrapper(void * rawPtr) const = 0;

        //! \return Zarejestrowane w aplikacji typy danych
        virtual const TypeInfoSet & getSupportedTypes() const = 0;

        //! \return Hierarchia typ�w danych - jakie operacje moge realizowa�, po czym dziedzicze
        virtual const TypeInfoSet & getTypeBaseTypes(const TypeInfo & type) const = 0;

        //! \return Hierarchia typ�w danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i mo�e by� downcastowany na mnie
        virtual const TypeInfoSet & getTypeDerrivedTypes(const TypeInfo & type) const = 0;

		//! \param ptr Surowy wska�nik, dla kt�rego sprawdzamy czy jest ju� za�adzany przez DM
		//! \return prawda je�li wska�nik jest zarz�dzany prze DM i opakowano go w OW
		virtual bool objectIsManaged(const void * ptr) const = 0;

		//! \param ptr Surowy wska�nik, dla kt�rego sprawdzamy czy jest ju� za�adzany przez DM
		//! \return OW opakowuj�cy dany wska�nik
		virtual const ObjectWrapperPtr & getObjectWrapperForRawPtr(const void * ptr) const = 0;
    };

    //! Zbi�r typ�w
    typedef std::set<TypeInfo> Types;

    //! Interfejs najni�szego poziomu DataManager - pozwala zarz�dza� pojedynczymi ObjectWrapperami: dodawa� je do puli danych, usuwa�, inicjalizowa�, deinicjalizowa�.
    //!
    class IMemoryDataManager : public utils::Observable<IMemoryDataManager>, virtual public IDataManagerReader
    {

    public:
		//! Konstruktor domy�lny
		IMemoryDataManager() : blockNotify_(false), changed(false)
		{

		}

		template<class SmartPtr>
		static ObjectWrapperPtr addData(IMemoryDataManager * manager, const SmartPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr())
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer si� nie zgadza");

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
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer si� nie zgadza");
			manager->removeData(manager->getObjectWrapperForRawPtr(data.get()));
		}

        virtual ~IMemoryDataManager() {};

        //! \param Obiekt kt�ry chcemy inicjalizowa�
        virtual void initializeData(ObjectWrapperPtr & data) = 0;

        //! \param Obiekt dla kt�rego chcemy wykona� deinicjalizacje
        //! \return Prawda je�li obiekt posiada inicjalizator
        virtual bool isInitializable(const ObjectWrapperPtr & data) const = 0;

        //! \param Obiekt kt�ry chcemy deinicjalizowa� - dalej jest w DataManager ale nie zawiera danych - trzeba potem inicjalizowa�
        virtual void deinitializeData(ObjectWrapperPtr & data) = 0;

		//! Ta metoda notyfikuje o zmianie stanu DM!!
        //! \param Obiekt kt�ry zostanie usuniety je�li zarz�dza nim DataManager
        void removeData(const ObjectWrapperPtr & data)
		{
			nonNotifyRemoveData(data);
			tryNotify();
		}

		//! \param block Czy blokowa� notyfikacje przy zarz�dzaniu danymi DM
		void blockNotify(bool block)
		{
			blockNotify_ = block;

			if(blockNotify_ == false && changed){
				changed = false;
				notify();
			}
		}
		//! \return Czy s� blokowane aktualnie notyfikacje o zmianie stanu DM
		bool isNotifyBlocked() const
		{
			return blockNotify_;
		}

    private:
		//! Wewn�trzna pr�ba realizacji notyfikacji je�li nast�pi�a zmiana i s� w��czone notyfikacje
		void tryNotify()
		{
			if(blockNotify_ == false){
				notify();
			}else{
				changed = true;
			}
		}

		//! Schowane bo DM wprowadza w�asn� implementacj� OW z auto i nicjalizacj� zasob�w przy pierwszym pobraniu!! Nie upubliczniamy tego w �aden spos�b!!
		//! U�ywamy tylko metod statycznych. Ta metoda nie notyfikuje o zmianie!!
        //! \param Obiekt kt�ry zostanie utrwalony w DataManager i b�dzie dost�pny przy zapytaniach, nie mo�e by� niezainicjowany - isNull musi by� false!!
        virtual void nonNotifyAddData(const ObjectWrapperPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr()) = 0;

		//! Metoda faktycznie usuwaj�ca dane z DM ale nie notyfikuj�ca o jego zmianie
		virtual void nonNotifyRemoveData(const ObjectWrapperPtr & data) = 0;

        //! \param type Typ dla kt�rego tworzymy specyficzny dla DM ObjectWrapper
        //! \return OW dla zadanego typu
        virtual ObjectWrapperPtr createObjectWrapper(const TypeInfo & type) const = 0;

	private:
		//! Czy blokujemy notyfikacje
		bool blockNotify_;
		//! Czy nast�pi�a zmiana w DM
		bool changed;

    };

    //! Interfejs dost�pu do danych i �adowania danych w aplikacji
	class IFileDataManager : public utils::Observable<IFileDataManager>, virtual public IDataManagerReader
	{
	public:
        //! Zbi�r rozszerze�
        typedef std::set<std::string> Extensions;

        //! Opis rozszerze�
        struct ExtensionDescription
        {
            //! Zarejestrowane opisy
            std::vector<std::string> descriptions;
            //! Zarejestrowane typy
            Types types;
        };
        //! Konstruktor domy�lny
		IFileDataManager() : blockNotify_(false), changed(false)
		{

		}

		//! Wirtualny destruktor.
		virtual ~IFileDataManager() {};

        //! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
        virtual void getManagedFiles(Files & files) const = 0;

        //! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
        //! \return Prawda je�li plik jest zarz�dzany przez ten DM
        virtual bool isFileManaged(const Filesystem::Path & file) const = 0;

        //! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
        //! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
		void addFile(const Filesystem::Path & file)
		{
		    std::vector<ObjectWrapperPtr> temp;
			nonNotifyAddFile(file, temp);
			tryNotify();
		}

        //! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
		//! \param objects [out] Agregat obiekt�w wyci�gni�tych z danego pliku przez parsery
        //! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
		void addFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects)
		{
			nonNotifyAddFile(file, objects);
			tryNotify();
		}

        //! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
		void removeFile(const Filesystem::Path & file)
		{
			nonNotifyRemoveFile(file);
			tryNotify();
		}

        //! \param path �ciezka pliku kt�ry chemy za�adowa� (parsowa�) WYMUSZAMY PARSOWANIE I INICJALIZACJE
        virtual void initializeFile(const Filesystem::Path & file) = 0;

        //! \param path �ciezka pliku kt�ry chemy za�adowa� (parsowa�) ZWALNIAMY ZASOBY, ALE ZEZWALAMY PONOWNIE NA LENIWA INICJALIZACJE
        virtual void deinitializeFile(const Filesystem::Path & file) = 0;

        //! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
        //! \return Mapa obiekt�w wzgledem plik�w z kt�rych pochodza
        virtual void getObjectsForFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects) const = 0;

        //! \return Zbior obs�ugiwanych rozszerzen plik�w wraz z ich opisem
        virtual const Extensions & getSupportedFilesExtensions() const = 0;

        //! \return true je�li rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
        virtual bool isExtensionSupported(const std::string & extension) const = 0;

        //! \param extension Rozszerzenie dla kt�rego pytamy o opis
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
		//! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
		//! \param objects [out] Agregat obiekt�w wyci�gni�tych z danego pliku przez parsery
		//! b�da dost�pne poprzez DataMangera LENIWA INICJALIZACJA
		virtual void nonNotifyAddFile(const Filesystem::Path & file, std::vector<ObjectWrapperPtr> & objects) = 0;

		//! Ta metoda nie notyfikuje o zmianie stanu DM!!
		//! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
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
		//! \param t Obiekt kt�rego notyfikacje chcemy lokalnie wy��czy�, ich stan zostanie przywr�cony w momencie niszczenia tego obiektu
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
		//! Obs�ugiwany obiekt
		T * t;
		//! Poprzedni stan blokowania notyfikacji
		bool prevBlockingState;

	};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

CORE_DEFINE_WRAPPER(core::Metadata, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

#endif // I_DATA_MANAGER_H
