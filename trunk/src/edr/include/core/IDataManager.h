#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <vector>
#include <map>
#include <set>
#include <utils/Utils.h>
#include <utils/ObserverPattern.h>
#include <core/IParser.h>
#include <core/Filesystem.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
#include <boost/function.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    //Interfejs Klasy s³u¿¹ca do inicjalizacji/deinicjalizacji obiektów domenowych
    class IDataInitializer
    {
    public:
        virtual ~IDataInitializer() {};

        //! \param object Obiekt do inicjalizacji
        virtual void initialize(core::ObjectWrapperPtr & object) = 0;
        
        //! \param object Obiekt do deinicjalizacji
        void deinitialize(core::ObjectWrapperPtr & object)
        {
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

    //! Zbiór typów
    typedef std::set<TypeInfo> Types;
    //! Zbiór obiektów domenowych
    typedef core::Objects Objects;

    class IMemoryDataManager : public utils::Observable<IMemoryDataManager>
    {
    public:

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

        //! \param Obiekt ktory zostanie utrwalony w DataManager i bêdzie dostepny przy zapytaniach, nie morze byc niezainicjowany - isNull musi byæ false!!
        virtual void addData(const core::ObjectWrapperPtr & data, const DataInitializerPtr & initializer = DataInitializerPtr()) = 0;

        //! \param Obiekt ktory zostanie usuniety jesli zarzadza nim DataManager
        virtual void removeData(const core::ObjectWrapperPtr & data) = 0;
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
        virtual void getManagedData(core::Files & files) const = 0;

        //! \param files Lista plików dla których zostan¹ utworzone parsery i z których wyci¹gniête dane
        //! bêda dostepne poprzez DataMangera LENIWA INICJALIZACJA
		virtual void addData(const Filesystem::Path & file) = 0;

        //! \param files Lista plików które zostan¹ usuniête z aplikacji a wraz z nimi skojarzone parsery i dane
		virtual void removeData(const Filesystem::Path & file) = 0;

        //! \param path Œciezka pliku który chemy za³adowaæ (parsowaæ) WYMUSZAMY PARSOWANIE I INICJALIZACJE
        virtual void initializeData(const Filesystem::Path & file) = 0;

        //! \param path Œciezka pliku który chemy za³adowaæ (parsowaæ) ZWALNIAMY ZASOBY, ALE ZEZWALAMY PONOWNIE NA LENIWA INICJALIZACJE
        virtual void deinitializeData(const Filesystem::Path & file) = 0;

        //! \param files Zbior plikow dla ktorych chcemy pobrac liste obiektow
        //! \return Mapa obiektow wzgledem plikow z ktorych pochodza
        virtual void getObjectsForData(const Filesystem::Path & file, Objects & objects) const = 0;

        //! \return Zbior obslugiwanych rozszerzen plikow wraz z ich opisem
        virtual const Extensions & getSupportedFilesExtensions() const = 0;

        //! \param extension Rozszerzenie dla którego pytamy o opis
        //! \return Opis rozszerzenia
        virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const = 0;
	};    

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif // I_DATA_MANAGER_H