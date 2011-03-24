/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:38
    filename: IObjectSource.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IOBJECTSOURCE_H__
#define HEADER_GUARD_CORE__IOBJECTSOURCE_H__

#include <list>
#include <stdexcept>
#include <boost/type_traits.hpp>
#include <utils/Debug.h>
#include <core/ObjectWrapper.h>
#include <core/Log.h>

namespace core 
{
    //! �r�d�o obiekt�w.
    class IObjectSource
    {
    // interfejs
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectSource() {}

        //! \return Liczba �r�de� danych.
        virtual int getNumObjects() const = 0;

        //! Wy�uskanie wska�nika na obiekt domenowy ze �r�d�a przy za�o�eniu jego zmienno�ci.
        virtual ObjectWrapperPtr getObject(int idx, boost::false_type) = 0;

        //! Wy�uskanie wska�nika na obiekt domenowy ze �r�d�a przy za�o�eniu jego niezmienno�ci.
        virtual ObjectWrapperConstPtr getObject(int idx, boost::true_type) = 0;

    // pomocnicze metody inline b�d� szablony
    public:
        //! Zastosowanie idiomu "Return Type Resolver" do unikania podw�jnego okre�lania typu
        //! elementu kolekcji
        //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
        struct getObject_t
        {
            //! �r�d�o danych.
            IObjectSource* source;
            //! Indeks �r�d�a.
            int idx;
            //! \param source �r�d�o danych.
            inline getObject_t(IObjectSource* source, int idx) :
            source(source), idx(idx)
            {}
            //! \return Kolekcja wska�nik�w.
            template <class SmartPtr>
            inline operator SmartPtr()
            {
                return source->getObject<SmartPtr>(idx);
            }
        };

        //! \return Dane pod zadanym indeksem.
        inline getObject_t getObject(int index)
        {
            return getObject_t(this, index);
        }

        //! \param idx Indeks �r�d�a danych.
        //! \return Dane pod zadanym indeksem.
        template <class SmartPtr>
        inline SmartPtr getObject(int idx, SmartPtr* /*dummy*/ = nullptr)
        {
            UTILS_ASSERT(idx < getNumObjects());
            return __getObjectPtrResolver<SmartPtr>(idx, boost::is_pointer<SmartPtr>());
        }

        //! \param idx Indeks �r�d�a danych.
        //! \return Dane pod zadanym indeksem.
        template <class SmartPtr>
        inline void getObject(SmartPtr& result, int idx)
        {
            result = getObject<SmartPtr>(idx);
        }


        //! \param idx Indeks �r�d�a.
        //! \param result Dane pod zadanym indeksem.
        //! \return Czy uda�o si� pobra� zadany typ?
        template <class SmartPtr>
        bool tryGetObject(SmartPtr& result, int idx)
        {
            // TODO: zrobi� wersj�, kt�ra nie b�dzie bazowa�a na wyj�tkach
            try {
                result = getObject<SmartPtr>(idx);
                return true;
            } catch (std::runtime_error& ex) {
                LOG_DEBUG("Source " << idx << " error: " << ex.what());
                return false;
            }
        }

    private:
        //! \param idx Indeks �r�d�a danych.
        //! \return Dane pod zadanym indeksem.
        template <class RawPtr>
        RawPtr __getObjectPtrResolver(int idx, boost::true_type, RawPtr* /*dummy*/ = nullptr)
        {
            UTILS_STATIC_ASSERT(false, "Do obiektow domenowych nalezy uzywac inteligentnych wskaznikow.");
            return nullptr;
        }

        //! \param idx Indeks �r�d�a danych.
        //! \return Dane pod zadanym indeksem.
        template <class SmartPtr>
        SmartPtr __getObjectPtrResolver(int idx, boost::false_type, SmartPtr* /*dummy*/ = nullptr)
        {
            typedef typename SmartPtr::element_type Type;
            // pobieramy wska�nik na wrapper const albo mutable
            auto wrapper = getObject(idx, boost::is_const<Type>());
            if ( wrapper && !wrapper->isNull() ) {
                // z niego pobieramy obiekt w�a�ciwy
                return wrapper->get<Type>();
            } else {
                throw std::runtime_error("Source not available.");
            }
        }
    };

    typedef shared_ptr<IObjectSource> IObjectSourcePtr;

} // namespace core


#endif  // HEADER_GUARD_CORE__IOBJECTSOURCE_H__