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
    //! Interfejs daj�cy dost�p do danych wejsciowych. Nie ma gwarancji �e dane s� dost�pne i zainicjalizowane - nale�e to ka�dorazowo sprawdza�.
    class IObjectSource
    {
    public:

        class InputObjectsCollection;
        typedef core::shared_ptr<InputObjectsCollection> InputObjectsCollectionPtr;
        typedef core::shared_ptr<const InputObjectsCollection> InputObjectsCollectionConstPtr;
        typedef core::weak_ptr<InputObjectsCollection> InputObjectsCollectionWeakPtr;
        typedef core::weak_ptr<const InputObjectsCollection> InputObjectsCollectionConstWeakPtr;

        //! Klasa proxy przykrywaj�ca mo�liwo�� przechowywania ObjecWrapper�w przez ObjecWrapperCollection. Umo�liwia jedynie pobieranie niemodyfikowalnych danych z kolekcji.
        //! Dane pobierane s� w rozpakowanej ju� formie do rzeczywistych typ�w (wyci�gni�te z ObjectWrapper�w). S� to smart pointery na const obiekty.
        class InputObjectsCollection
        {
        public:
            //! Return type resolver - poprzez operator konwersji pozwala kompilatorowi rozpozna� typ zwracany i wykona� dla niego konwersj�
            struct get_t
            {
            public:
                //! const ObjectWrapperPtr kt�y b�dziemy ropzakowywa�
                const ObjectWrapperConstPtr & constObjectWrapperPtr;
                //! Czy typ musi by� dok�adnie taki sam jaki przechowuje ObjectWraper czy mo�e to by� jaki� typ ni�ej w hierarchi dziedziczenia
                bool exact;

                //! Konstruktor inicjuj�cy RtR
                get_t(const ObjectWrapperConstPtr & constObjectWrapperPtr, bool exact)
                    : constObjectWrapperPtr(constObjectWrapperPtr), exact(exact) {}

                //! Operator konwersji w formie wzorca robi�cy ca�a magi�
                template<class SmartConstPtr>
                inline operator SmartConstPtr() const
                {
                    SmartConstPtr ret;
                    constObjectWrapperPtr->tryGet(ret, exact);
                    return ret;
                }

				//! Operator konwersji w formie wzorca robi�cy ca�a magi�
				inline operator ObjectWrapperConstPtr() const
				{
					return constObjectWrapperPtr;
				}
            };

        public:

            //! Konstruktor inicjuj�cy proxy kolekcj� object wrapper�w
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

            //! \return Czy kolekcja pusta (lub niezainicjowana - tak te� si� mo�e zda�y� dla wejscia
            bool empty() const
            {
                if(collection == nullptr){
                    return true;
                }

                return collection->empty();
            }

            //! \return Rozmiar kolekcji danych - 0 je�li pusta lub niezainicjowana
            int size() const
            {
                if(empty() == true){
                    return 0;
                }

                return collection->size();
            }


        private:
            //! Kolekcja danych kt�r� przykrywamy tym proxy
            ObjectWrapperCollectionConstPtr collection;
        };

    // interfejs
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectSource() {}

        //! \return Liczba �r�de� danych.
        virtual int getNumSources() const = 0;

        //! Wy�uskanie wska�nika na obiekt domenowy ze �r�d�a przy za�o�eniu jego niezmienno�ci.
        virtual InputObjectsCollection getObjects(int idx) const = 0;

    // pomocnicze metody inline b�d� szablony
    //public:
    //    //! Zastosowanie idiomu "Return Type Resolver" do unikania podw�jnego okre�lania typu
    //    //! elementu kolekcji
    //    //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
    //    struct getObjects_t
    //    {
    //        //! �r�d�o danych.
    //        IObjectSource* source;
    //        //! Indeks �r�d�a.
    //        int idx;
    //        //! \param source �r�d�o danych.
    //        inline getObjects_t(IObjectSource* source, int idx) :
    //        source(source), idx(idx)
    //        {}
    //        //! \return Kolekcja wska�nik�w.
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

    //    //! \param idx Indeks �r�d�a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    inline SmartPtr getObjects(int idx, SmartPtr* /*dummy*/ = nullptr)
    //    {
    //        UTILS_ASSERT(idx < getNumSources());
    //        return __getObjectsPtrResolver<SmartPtr>(idx, boost::is_pointer<SmartPtr>());
    //    }

    //    //! \param idx Indeks �r�d�a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    inline void getObjects(SmartPtr& result, int idx)
    //    {
    //        result = getObjects<SmartPtr>(idx);
    //    }


    //    //! \param idx Indeks �r�d�a.
    //    //! \param result Dane pod zadanym indeksem.
    //    //! \return Czy uda�o si� pobra� zadany typ?
    //    template <class SmartPtr>
    //    bool tryGetObjects(SmartPtr& result, int idx)
    //    {
    //        // TODO: zrobi� wersj�, kt�ra nie b�dzie bazowa�a na wyj�tkach
    //        try {
    //            result = getObjects<SmartPtr>(idx);
    //            return true;
    //        } catch (std::runtime_error& ex) {
    //            LOG_DEBUG("Source " << idx << " error: " << ex.what());
    //            return false;
    //        }
    //    }

    //private:
    //    //! \param idx Indeks �r�d�a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class RawPtr>
    //    RawPtr __getObjectsPtrResolver(int idx, boost::true_type, RawPtr* /*dummy*/ = nullptr)
    //    {
    //        UTILS_STATIC_ASSERT(false, "Do obiekt�w domenowych nale�y u�ywa� inteligentnych wskaznik�w.");
    //        return nullptr;
    //    }

    //    //! \param idx Indeks �r�d�a danych.
    //    //! \return Dane pod zadanym indeksem.
    //    template <class SmartPtr>
    //    SmartPtr __getObjectsPtrResolver(int idx, boost::false_type, SmartPtr* /*dummy*/ = nullptr)
    //    {
    //        typedef typename SmartPtr::element_type Type;
    //        // pobieramy wska�nik na wrapper const albo mutable
    //        auto collection = getObjects(idx, boost::is_const<Type>());
    //        return collection;
    //        //if ( collection && !wrapper->isNull() ) {
    //        //    // z niego pobieramy obiekt w�a�ciwy
    //        //    return wrapper->get();
    //        //} else {
    //        //    throw std::runtime_error("Source not available.");
    //        //}
    //    }
    };

    typedef shared_ptr<IObjectSource> IObjectSourcePtr;
    
} // namespace core


#endif  // HEADER_GUARD_CORE__IOBJECTSOURCE_H__