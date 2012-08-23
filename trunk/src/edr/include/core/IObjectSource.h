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
    //! Interfejs dajπcy dostÍp do danych wejsciowych. Nie ma gwarancji øe dane sπ dostÍpne i zainicjalizowane - naleøe to kaødorazowo sprawdzaÊ.
    class IObjectSource
    {
    public:

        class InputObjectsCollection;
        typedef core::shared_ptr<InputObjectsCollection> InputObjectsCollectionPtr;
        typedef core::shared_ptr<const InputObjectsCollection> InputObjectsCollectionConstPtr;
        typedef core::weak_ptr<InputObjectsCollection> InputObjectsCollectionWeakPtr;
        typedef core::weak_ptr<const InputObjectsCollection> InputObjectsCollectionConstWeakPtr;

        //! Klasa proxy przykrywajπca moøliwoúÊ przechowywania ObjecWrapperÛw przez ObjecWrapperCollection. Umoøliwia jedynie pobieranie niemodyfikowalnych danych z kolekcji.
        //! Dane pobierane sπ w rozpakowanej juø formie do rzeczywistych typÛw (wyciπgniÍte z ObjectWrapperÛw). Sπ to smart pointery na const obiekty.
        class InputObjectsCollection
        {
        public:
            //! Return type resolver - poprzez operator konwersji pozwala kompilatorowi rozpoznaÊ typ zwracany i wykonaÊ dla niego konwersjÍ
            struct get_t
            {
            public:
                //! const ObjectWrapperPtr ktÛy bÍdziemy ropzakowywaÊ
                const ObjectWrapperConstPtr & constObjectWrapperPtr;
                //! Czy typ musi byÊ dok≥adnie taki sam jaki przechowuje ObjectWraper czy moøe to byÊ jakiú typ niøej w hierarchi dziedziczenia
                bool exact;

                //! Konstruktor inicjujπcy RtR
                get_t(const ObjectWrapperConstPtr & constObjectWrapperPtr, bool exact)
                    : constObjectWrapperPtr(constObjectWrapperPtr), exact(exact) {}

                //! Operator konwersji w formie wzorca robiπcy ca≥a magiÍ
                template<class SmartConstPtr>
                inline operator SmartConstPtr() const
                {
                    SmartConstPtr ret;
                    constObjectWrapperPtr->tryGet(ret, exact);
                    return ret;
                }

				//! Operator konwersji w formie wzorca robiπcy ca≥a magiÍ
				inline operator ObjectWrapperConstPtr() const
				{
					return constObjectWrapperPtr;
				}
            };

        public:

            //! Konstruktor inicjujπcy proxy kolekcjπ object wrapperÛw
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

            //! \return Czy kolekcja pusta (lub niezainicjowana - tak teø siÍ moøe zdaøyÊ dla wejscia
            bool empty() const
            {
                if(collection == nullptr){
                    return true;
                }

                return collection->empty();
            }

            //! \return Rozmiar kolekcji danych - 0 jeúli pusta lub niezainicjowana
            int size() const
            {
                if(empty() == true){
                    return 0;
                }

                return collection->size();
            }


        private:
            //! Kolekcja danych ktÛrπ przykrywamy tym proxy
            ObjectWrapperCollectionConstPtr collection;
        };

    // interfejs
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectSource() {}

        //! \return Liczba ürÛde≥ danych.
        virtual int getNumSources() const = 0;

        //! Wy≥uskanie wskaünika na obiekt domenowy ze ürÛd≥a przy za≥oøeniu jego niezmiennoúci.
        virtual InputObjectsCollection getObjects(int idx) const = 0;

    // pomocnicze metody inline bπdü szablony
    //public:
    //    //! Zastosowanie idiomu "Return Type Resolver" do unikania podwÛjnego okreúlania typu
    //    //! elementu kolekcji
    //    //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
    //    struct getObjects_t
    //    {
    //        //! èrÛd≥o danych.
    //        IObjectSource* source;
    //        //! Indeks ürÛd≥a.
    //        int idx;
    //        //! \param source èrÛd≥o danych.
    //        inline getObjects_t(IObjectSource* source, int idx) :
    //        source(source), idx(idx)
    //        {}
    //        //! \return Kolekcja wskaünikÛw.
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

    //    //! \param idx Indeks ürÛd≥a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    inline SmartPtr getObjects(int idx, SmartPtr* /*dummy*/ = nullptr)
    //    {
    //        UTILS_ASSERT(idx < getNumSources());
    //        return __getObjectsPtrResolver<SmartPtr>(idx, boost::is_pointer<SmartPtr>());
    //    }

    //    //! \param idx Indeks ürÛd≥a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    inline void getObjects(SmartPtr& result, int idx)
    //    {
    //        result = getObjects<SmartPtr>(idx);
    //    }


    //    //! \param idx Indeks ürÛd≥a.
    //    //! \param result Dane pod zadanym indeksem.
    //    //! \return Czy uda≥o siÍ pobraÊ zadany typ?
    //    template <class SmartPtr>
    //    bool tryGetObjects(SmartPtr& result, int idx)
    //    {
    //        // TODO: zrobiÊ wersjÍ, ktÛra nie bÍdzie bazowa≥a na wyjπtkach
    //        try {
    //            result = getObjects<SmartPtr>(idx);
    //            return true;
    //        } catch (std::runtime_error& ex) {
    //            LOG_DEBUG("Source " << idx << " error: " << ex.what());
    //            return false;
    //        }
    //    }

    //private:
    //    //! \param idx Indeks ürÛd≥a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class RawPtr>
    //    RawPtr __getObjectsPtrResolver(int idx, boost::true_type, RawPtr* /*dummy*/ = nullptr)
    //    {
    //        UTILS_STATIC_ASSERT(false, "Do obiektÛw domenowych naleøy uøywaÊ inteligentnych wskaznikÛw.");
    //        return nullptr;
    //    }

    //    //! \param idx Indeks ürÛd≥a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    SmartPtr __getObjectsPtrResolver(int idx, boost::false_type, SmartPtr* /*dummy*/ = nullptr)
    //    {
    //        typedef typename SmartPtr::element_type Type;
    //        // pobieramy wskaünik na wrapper const albo mutable
    //        auto collection = getObjects(idx, boost::is_const<Type>());
    //        return collection;
    //        //if ( collection && !wrapper->isNull() ) {
    //        //    // z niego pobieramy obiekt w≥aúciwy
    //        //    return wrapper->get();
    //        //} else {
    //        //    throw std::runtime_error("Source not available.");
    //        //}
    //    }
    };

    typedef shared_ptr<IObjectSource> IObjectSourcePtr;
    
} // namespace core


#endif  // HEADER_GUARD_CORE__IOBJECTSOURCE_H__