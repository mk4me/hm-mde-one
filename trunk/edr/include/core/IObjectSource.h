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
#include <core/Log.h>

namespace core 
{
    //! èrÛd≥o obiektÛw.
    class IObjectSource
    {
    // interfejs
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectSource() {}

        //! \return Liczba ürÛde≥ danych.
        virtual int getNumSources() const = 0;

        //! \return true jeøeli obiekt o indeksie idx uleg≥ zmianie lub zosta≥ wyzerowany/ustawiony.
        virtual bool isChanged(int idx) const = 0;

        //! Wy≥uskanie wskaünika na obiekt domenowy ze ürÛd≥a przy za≥oøeniu jego zmiennoúci.
        //virtual ObjectWrapperCollectionPtr getObjects(int idx, boost::false_type) = 0;

        //! Wy≥uskanie wskaünika na obiekt domenowy ze ürÛd≥a przy za≥oøeniu jego niezmiennoúci.
        virtual ConstObjectWrapperCollectionConstPtr getObjects(int idx/*, boost::true_type*/) const = 0;

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
    //        UTILS_STATIC_ASSERT(false, "Do obiektow domenowych nalezy uzywac inteligentnych wskaznikow.");
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