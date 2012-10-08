/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:38
    filename: IObjectSource.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IOBJECTSOURCE_H__
#define HEADER_GUARD_CORE__IOBJECTSOURCE_H__

#include <stdexcept>
#include <boost/type_traits.hpp>
#include <utils/Debug.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>
#include <core/ILog.h>

namespace core 
{
    //! Interfejs dający dostęp do danych wejsciowych. Nie ma gwarancji że dane są dostępne i zainicjalizowane - należe to każdorazowo sprawdzać.
    class IObjectSource
    {
    public:

        class InputObjectsCollection;
        typedef core::shared_ptr<InputObjectsCollection> InputObjectsCollectionPtr;
        typedef core::shared_ptr<const InputObjectsCollection> InputObjectsCollectionConstPtr;
        typedef core::weak_ptr<InputObjectsCollection> InputObjectsCollectionWeakPtr;
        typedef core::weak_ptr<const InputObjectsCollection> InputObjectsCollectionConstWeakPtr;

        //! Klasa proxy przykrywająca możliwość przechowywania ObjecWrapperów przez ObjecWrapperCollection. Umożliwia jedynie pobieranie niemodyfikowalnych danych z kolekcji.
        //! Dane pobierane są w rozpakowanej już formie do rzeczywistych typów (wyciągnięte z ObjectWrapperów). Są to smart pointery na const obiekty.
        class InputObjectsCollection
        {
        public:
            //! Return type resolver - poprzez operator konwersji pozwala kompilatorowi rozpoznać typ zwracany i wykonać dla niego konwersję
            struct get_t
            {
            public:
                //! const ObjectWrapperPtr któy będziemy ropzakowywać
                const ObjectWrapperConstPtr & constObjectWrapperPtr;
                //! Czy typ musi być dokładnie taki sam jaki przechowuje ObjectWraper czy może to być jakiś typ niżej w hierarchi dziedziczenia
                bool exact;

                //! Konstruktor inicjujący RtR
                get_t(const ObjectWrapperConstPtr & constObjectWrapperPtr, bool exact)
                    : constObjectWrapperPtr(constObjectWrapperPtr), exact(exact) {}

                //! Operator konwersji w formie wzorca robiący cała magię
                template<class SmartConstPtr>
                inline operator SmartConstPtr() const
                {
                    SmartConstPtr ret;
                    constObjectWrapperPtr->tryGet(ret, exact);
                    return ret;
                }

				//! Operator konwersji w formie wzorca robiący cała magię
				inline operator ObjectWrapperConstPtr() const
				{
					return constObjectWrapperPtr;
				}
            };

        public:

            //! Konstruktor inicjujący proxy kolekcją object wrapperów
            InputObjectsCollection(const ObjectWrapperCollectionConstPtr & collection = ObjectWrapperCollectionConstPtr())
                : collection(collection)
            {

            }

            //! Destruktor
            ~InputObjectsCollection()
            {

            }

            //! Zwraca dane w zadanej formie - RtR
            //! \param idx Indeks danych
            //! \return Return Type Resolver
            const get_t getObject(int idx) const
            {
                UTILS_ASSERT((collection != nullptr), "Bledna kolekcja w proxy dla wejscia");

                if(collection == nullptr){
                    throw std::runtime_error("Call beyond collection range or to nullptr");
                }

                const get_t ret(collection->getObject(idx), false);
                return ret;
            }

            //! \return Czy kolekcja pusta (lub niezainicjowana - tak też się może zdażyć dla wejscia
            bool empty() const
            {
                if(collection == nullptr){
                    return true;
                }

                return collection->empty();
            }

            //! \return Rozmiar kolekcji danych - 0 jeśli pusta lub niezainicjowana
            int size() const
            {
                if(empty() == true){
                    return 0;
                }

                return collection->size();
            }


        private:
            //! Kolekcja danych którą przykrywamy tym proxy
            ObjectWrapperCollectionConstPtr collection;
        };

    // interfejs
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectSource() {}

        //! \return Liczba źródeł danych.
        virtual int getNumSources() const = 0;

        //! Wyłuskanie wskaźnika na obiekt domenowy ze źródła przy założeniu jego niezmienności.
        virtual InputObjectsCollection getObjects(int idx) const = 0;

    // pomocnicze metody inline bądź szablony
    //public:
    //    //! Zastosowanie idiomu "Return Type Resolver" do unikania podwójnego określania typu
    //    //! elementu kolekcji
    //    //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
    //    struct getObjects_t
    //    {
    //        //! Źródło danych.
    //        IObjectSource* source;
    //        //! Indeks źródła.
    //        int idx;
    //        //! \param source Źródło danych.
    //        inline getObjects_t(IObjectSource* source, int idx) :
    //        source(source), idx(idx)
    //        {}
    //        //! \return Kolekcja wskaźników.
    //        template <class SmartPtr>
    //        inline operator SmartPtr()
    //        {
    //            return source->getObjects<SmartPtr>(idx);
    //        }
    //    };

    //    //! \return Dane pod zadanym indeksem.
    //    inline getObjects_t getObjects(int index)
    //    {
    //        return getObjects_t(this, index);
    //    }

    //    //! \param idx Indeks źródła danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    inline SmartPtr getObjects(int idx, SmartPtr* /*dummy*/ = nullptr)
    //    {
    //        UTILS_ASSERT(idx < getNumSources());
    //        return __getObjectsPtrResolver<SmartPtr>(idx, boost::is_pointer<SmartPtr>());
    //    }

    //    //! \param idx Indeks źródła danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    inline void getObjects(SmartPtr& result, int idx)
    //    {
    //        result = getObjects<SmartPtr>(idx);
    //    }


    //    //! \param idx Indeks źródła.
    //    //! \param result Dane pod zadanym indeksem.
    //    //! \return Czy udało się pobrać zadany typ?
    //    template <class SmartPtr>
    //    bool tryGetObjects(SmartPtr& result, int idx)
    //    {
    //        // TODO: zrobić wersję, która nie będzie bazowała na wyjątkach
    //        try {
    //            result = getObjects<SmartPtr>(idx);
    //            return true;
    //        } catch (std::runtime_error& ex) {
    //            LOG_DEBUG("Source " << idx << " error: " << ex.what());
    //            return false;
    //        }
    //    }

    //private:
    //    //! \param idx Indeks źródła danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class RawPtr>
    //    RawPtr __getObjectsPtrResolver(int idx, boost::true_type, RawPtr* /*dummy*/ = nullptr)
    //    {
    //        UTILS_STATIC_ASSERT(false, "Do obiektów domenowych należy używać inteligentnych wskazników.");
    //        return nullptr;
    //    }

    //    //! \param idx Indeks źródła danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    SmartPtr __getObjectsPtrResolver(int idx, boost::false_type, SmartPtr* /*dummy*/ = nullptr)
    //    {
    //        typedef typename SmartPtr::element_type Type;
    //        // pobieramy wskaźnik na wrapper const albo mutable
    //        auto collection = getObjects(idx, boost::is_const<Type>());
    //        return collection;
    //        //if ( collection && !wrapper->isNull() ) {
    //        //    // z niego pobieramy obiekt właściwy
    //        //    return wrapper->get();
    //        //} else {
    //        //    throw std::runtime_error("Source not available.");
    //        //}
    //    }
    };

    typedef shared_ptr<IObjectSource> IObjectSourcePtr;
    
} // namespace core


#endif  // HEADER_GUARD_CORE__IOBJECTSOURCE_H__
