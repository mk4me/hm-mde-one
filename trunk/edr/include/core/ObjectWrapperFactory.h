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
#include <core/ObjectWrapperCollection.h>

namespace core 
{
    //! Abstrakcyjna fabryka obiekt�w.
    class IObjectWrapperFactory
    {
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectWrapperFactory() {}
        //! 
        virtual ObjectWrapper* createWrapper() = 0;
        //!
        virtual ObjectWrapperCollection* createWrapperCollection() = 0;
        //!
        virtual const core::TypeInfo & getType() const = 0;
    };

    typedef shared_ptr<IObjectWrapperFactory> IObjectWrapperFactoryPtr;

    //! Domy�lna implementacja interfejsu. �eby dzia�a� musi widzie�
    //! definicj� CORE_DEFINE_WRAPPER lub pochodnych.
    template <class T>
    class ObjectWrapperFactory : public IObjectWrapperFactory
    {
    public:
        ObjectWrapperFactory() : typeInfo(typeid(T)) {}

        virtual ObjectWrapper* createWrapper()
        {
            UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
            return new ObjectWrapperT<T>();
        }

        virtual ObjectWrapperCollection* createWrapperCollection()
        {
            UTILS_STATIC_ASSERT(ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
            return new ObjectWrapperCollectionT<T>();
        }

        virtual const core::TypeInfo & getType() const
        {
            return typeInfo;
        }  

    private:
        core::TypeInfo typeInfo;
    };

} // namespace core


#endif  // HEADER_GUARD_CORE__OBJECTWRAPPERFACTORY_H__