#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <vector>
#include <map>
#include <set>
#include <boost/type_traits.hpp>
#include <utils/Utils.h>
#include <utils/ObserverPattern.h>
#include <core/Filesystem.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    //! Interfejs s³u¿¹ca do inicjalizacji/deinicjalizacji obiektów domenowych
    class IDataInitializer
    {
    public:
        virtual ~IDataInitializer() {};

        //! \param object Obiekt do inicjalizacji
        virtual void initialize(core::ObjectWrapperPtr & object) = 0;
        
        //! \param object Obiekt do deinicjalizacji
        void deinitialize(core::ObjectWrapperPtr & object)
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
        virtual void doDeinitialize(core::ObjectWrapperPtr & object) {}

    };

    typedef core::shared_ptr<IDataInitializer> DataInitializerPtr;

    //! Podstawowe operacje zwiazane z danymi - pobieranie danych, informacje o hierarchi typów danych, informacje o wspieranych typach danych
    class IDataManagerBase
    {
    public:
        virtual ~IDataManagerBase() {}

        //virtual ObjectWrapperPtr createWrapper(const TypeInfo & typeInfo) const = 0;

        virtual void getObjects(std::vector<ObjectWrapperConstPtr> & objects, const TypeInfo & type, bool exact = true) = 0;

        virtual void getObjects(core::ObjectWrapperCollection& objects) = 0;

        //! \param rawPtr Surowy wskaŸnik do danych
        //! \return ObjectWrapperConstPtr opakowuj¹cy ten wskaŸnik lub pusty ObjectWrapperPtr jeœli nie ma takich danych w DataManager
        //virtual ObjectWrapperConstPtr getWrapper(void * rawPtr) const = 0;

        //! \return Zarejestrowane w aplikacji typy danych
        virtual const TypeInfoSet & getSupportedTypes() const = 0;

        //! \return Hierarchia typow danych - jakie operacje moge realizowac, po czym dziedzicze
        virtual const TypeInfoSet & getTypeBaseTypes(const TypeInfo & type) const = 0;

        //! \return Hierarchia typow danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i moze byc downcastowany na mnie
        virtual const TypeInfoSet & getTypeDerrivedTypes(const TypeInfo & type) const = 0;
    };

    //! Zbiór typów
    typedef std::set<TypeInfo> Types;

    //! Interfejs najni¿szego poziomu DataManager - pozwala zarz¹dzaæ pojedynczymi ObjectWrapperami: dodawaæ je do puli danych, usuwaæ, inicjalizowaæ, deinicjalizowaæ.
    //!  
    class IMemoryDataManager : public utils::Observable<IMemoryDataManager>
    {
        ////! ZaprzyjaŸniona metoda pomagaj¹ca dodawaæ dane do DM
        //template<class SmartPtr>
        //friend core::ObjectWrapperPtr addData(IMemoryDataManager * manager, const SmartPtr & data, const DataInitializerPtr & initializer);

        ////! ZaprzyjaŸniona metoda pomagaj¹ca usuwaæ dane z DM
        //template<class SmartPtr>
        //friend void removeData(IMemoryDataManager * manager, const SmartPtr & data);

    public:

		template<class SmartPtr>
		static core::ObjectWrapperPtr addData(IMemoryDataManager * manager, const SmartPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr())
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

		/*template<class SmartPtr>
		static core::ObjectWrapperPtr addFile(IMemoryDataManager * manager, const SmartPtr & data)
		{
			return addFile(manager, data, DataInitializerPtr());
		}*/

		template<class SmartPtr>
		static void removeData(IMemoryDataManager * manager, const SmartPtr & data)
		{
			UTILS_STATIC_ASSERT(ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
			//UTILS_STATIC_ASSERT(boost::is_same<SmartPtr, ObjectWrapperT<typename SmartPtr::element_type>::Ptr>::value, "Pointer sie nie zgadza");
			manager->removeData(getObjectWrapperForRawPtr(data.get()));
		}

        virtual ~IMemoryDataManager() {};

        //! \param objects Zbiór obiektów domenowych zarz¹dzanych przez ten DataManager
        virtual void getManagedData(Objects & objects) const = 0;

        //! \param Obiekt ktory chcemy inicjalizowaæ
        virtual void initializeData(core::ObjectWrapperPtr & data) = 0;

        //! \param Obiekt dla ktorego chcemy wykonac deinicjalizacje
        //! \return Prawda jeœli obiekt posiada inicjalizator
        virtual bool isInitializable(const core::ObjectWrapperPtr & data) const = 0;

        //! \param Obiekt ktory chcemy deinicjalizowaæ - dalej jest w DataManager ale nie zawiera danych - trzeba potem inicjalizowaæ
        virtual void deinitializeData(core::ObjectWrapperPtr & data) = 0;

        //! \param Obiekt ktory zostanie usuniety jesli zarzadza nim DataManager
        virtual void removeData(const core::ObjectWrapperPtr & data) = 0;

    private:

        //! \param Obiekt ktory zostanie utrwalony w DataManager i bêdzie dostepny przy zapytaniach, nie morze byc niezainicjowany - isNull musi byæ false!!
        virtual void addData(const core::ObjectWrapperPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr()) = 0;

        //! \param ptr Surowy wskaŸnik, dla którego sprawdzamy czy jest juz za¿adzany przez DM
        //! \return prawda jeœli wskaŸnik jest zarz¹dzany prze DM i opakowano go w OW
        virtual bool objectIsManaged(void * ptr) const = 0;

        //! \param ptr Surowy wskaŸnik, dla którego sprawdzamy czy jest juz za¿adzany przez DM
        //! \return OW opakowuj¹cy dany wskaŸnik
        virtual const ObjectWrapperPtr & getObjectWrapperForRawPtr(const void * ptr) const = 0;

        //! \param type Typ dla którego tworzymy specyficzny dla DM ObjectWrapper
        //! \return OW dla zadanego typu
        virtual ObjectWrapperPtr createObjectWrapper(const TypeInfo & type) const = 0;
    };

    //! Interfejs dostepu do danych i ³adowania danych w aplikacji
	class IFileDataManager : public utils::Observable<IFileDataManager>
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
        virtual void getManagedFiles(core::Files & files) const = 0;
        
        //! \param file Plik kótry weryfikujemy czy jest zarzadzany przez DM
        //! \return Prawda jesli plik jest zarz¹dzany przez ten DM
        virtual bool isFileManaged(core::Filesystem::Path & file) const = 0;

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

#endif // I_DATA_MANAGER_H