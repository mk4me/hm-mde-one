/********************************************************************
    created:  2011/04/27
    created:  27:4:2011   14:58
    filename: ObjectWrapperFactory.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__OBJECTWRAPPERFACTORY_H__
#define HEADER_GUARD_CORE__OBJECTWRAPPERFACTORY_H__

#include <core/ObjectWrapper.h>

namespace core 
{
    //! Abstrakcyjna fabryka obiektów.
    class IObjectWrapperFactory
    {
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectWrapperFactory() {}
        //! 
        virtual ObjectWrapper* createWrapper() = 0;
        //!
        virtual core::TypeInfo getType() = 0;
    };

    typedef shared_ptr<IObjectWrapperFactory> IObjectWrapperFactoryPtr;

    //! Domyœlna implementacja interfejsu. ¯eby dzia³aæ musi widzieæ
    //! definicjê CORE_DEFINE_WRAPPER lub pochodnych.
    template <class T>
    class ObjectWrapperFactory : public IObjectWrapperFactory
    {
    public:
        virtual ObjectWrapper* createWrapper()
        {
            UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
            return new ObjectWrapperT<T>();
        }
        virtual core::TypeInfo getType()
        {
            return typeid(T);
        }        
    };

} // namespace core


#endif  // HEADER_GUARD_CORE__OBJECTWRAPPERFACTORY_H__