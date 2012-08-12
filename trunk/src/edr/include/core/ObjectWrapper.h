/********************************************************************
	created:  2011/02/11
	created:  11:2:2011   15:32
	filename: ObjectWrapper.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__OBJECTWRAPPER_H__
#define __HEADER_GUARD_CORE__OBJECTWRAPPER_H__

#include <set>
#include <string>
#include <boost/type_traits.hpp>
#include <core/TypeInfo.h>
#include <utils/Debug.h>
#include <core/SmartPtr.h>
#include <utils/PtrPolicyBoost.h>
#include <utils/ClonePolicies.h>


////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
    class IObjectWrapper;
    typedef shared_ptr<IObjectWrapper> ObjectWrapperPtr;
    typedef shared_ptr<const IObjectWrapper> ObjectWrapperConstPtr;
    typedef weak_ptr<IObjectWrapper> ObjectWrapperWeakPtr;
    typedef weak_ptr<const IObjectWrapper> ObjectWrapperConstWeakPtr;

    typedef std::set<ObjectWrapperPtr> Objects;
    typedef std::set<ObjectWrapperConstPtr> ConstObjects;

    template <class OWT> class ObjectWrapperT;

    class IObjectWrapper
    {
    protected:
        class IObjectWrapperHelper
        {
        public:
            static bool getSmartPtr(const ObjectWrapperPtr & ow, void* ptr, const TypeInfo& type)
            {
                return ow->getSmartPtr(ptr, type);
            }

            static bool setSmartPtr(const ObjectWrapperPtr & ow, const void* ptr, const TypeInfo& type)
            {
                return ow->setSmartPtr(ptr, type);
            }
        };

    public:
        //! Lista typów.
        typedef TypeInfoList Types;

    public:
        //! Return type resolver
        struct get_t
        {
            IObjectWrapper* wrapper;
            const IObjectWrapper* constWrapper;
            bool exact;

            template <class Ptr>
            operator Ptr()
            {
                Ptr result;
                if (wrapper->tryGet(result, exact)) {
                    return result;
                } else {
                    //throw std::bad_cast("Invalid cast");
                    throw std::bad_cast();
                }
            }
            template <class Ptr>
            operator Ptr() const
            {
                Ptr result;
                if (constWrapper->tryGet(result, exact)) {
                    return result;
                } else {
                    //throw std::bad_cast("Invalid cast");
                    throw std::bad_cast();
                }
            }
        };

    public:
        //! \param object
        //! \param name
        //! \return Wrapper obiektu.
        /*template <class T>
        static ObjectWrapperPtr create(T* dummy = nullptr)
        {
            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
            typedef ObjectWrapperT<T> Wrapper;
            Wrapper * wrapper = new Wrapper();
            return ObjectWrapperPtr(wrapper);
        }*/

        //! \return Nazwa dla zadanego typu.
        template <class T>
        static const char* getClassName(T* dummy = nullptr)
        {
            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
            return ObjectWrapperT<T>::className();
        }

        //! \param type Typ do porównania.
        //! \return Czy to te same typy?
        inline bool isTypeEqual(const TypeInfo& type) const
        {
            return getTypeInfo() == type;
        }

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IObjectWrapper() {}

    public:

        //! Próba pobrania obiektu z wrappera.
        //! \param object Rezultat.
        //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
        //! \return Sukces/pora¿ka.
        template <class Ptr>
        bool tryGet(Ptr& object, bool exact = false)
        {
            TypeInfo type = typeid(Ptr);
            if ( exact ) {
                auto types = getPtrTypeInfo();
                if ( types.first == type || types.second == type ) {
                    return getSmartPtr(&object, type);
                } else {
                    return false;
                }
            } else {
                return getSmartPtr(&object, type);
            }
        }

        //! Próba pobrania obiektu z wrappera.
        //! \param object Rezultat.
        //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
        //! \return Sukces/pora¿ka.
        template <class Ptr>
        bool tryGet(Ptr& object, bool exact = false) const
        {
            TypeInfo type = typeid(Ptr);
            if ( exact ) {
                auto types = getPtrTypeInfo();
                if ( types.second == type ) {
                    return getSmartPtr(&object, type);
                } else {
                    return false;
                }
            } else {
                return getSmartPtr(&object, type);
            }
        }

        //! Pobiera obiekt z wrappera. W razie b³êdu rzuca bad_castem.
        //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
        //! \return Return Type Resolver
        get_t get(bool exact = false)
        {
            get_t result = { this, nullptr, exact };
            return result;
        }

        //! \param dummy Pozostawiæ pusty.
        //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni¿ parametr szablonu rzucany jest wyj¹tek.
        template <class T>
        typename ObjectWrapperT<T>::Ptr get(T* dummy = nullptr)
        {
            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
            typedef ObjectWrapperT<T> Wrapper;
            typedef typename Wrapper::Ptr Ptr;
            return static_cast<Ptr>(get());
        }

        //! Pobiera obiekt z wrappera. W razie b³êdu rzuca bad_castem.
        //! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
        //! \return Return Type Resolver
        const get_t get(bool exact = false) const
        {
            const get_t result = { nullptr, this, exact };
            return result;
        }

        //! Ustawia obiekt wrappera. W razie b³êdu rzuca bad_castem.
        //! \param object Obiekt.
        template <class Ptr>
        void set(const Ptr& object, const std::string& name, const std::string& source)
        {
            if ( !trySet(object, name, source) ) {
                throw std::bad_cast();
            }
        }

        //! Ustawia obiekt wrappera, jego nazwe i zrodlo
        //! \param object Obiekt
        //! \param name Nazwa obiektu, ktora ma byc wyswietlona
        //! \param source Zrodlo obiektu - sciezka do pliku z ktorego pochodzi
        template <class Ptr>
        void set(const Ptr& object)
        {
            if ( !trySet(object)) {
                throw std::bad_cast();
            }
        }

        //! Ustawia obiekt wrappera.
        //! \param object Obiekt.
        //! \return Sukces/pora¿ka.
        template <class Ptr>
        bool trySet(const Ptr& object)
        {
            TypeInfo type = typeid(Ptr);
            if ( getPtrTypeInfo().first == type ) {
                return setSmartPtr(&object, type);
            } else {
                return false;
            }
        }

        //! Ustawia obiekt wrappera, jego nazwe i zrodlo
        //! \param object Obiekt
        //! \param name Nazwa obiektu, ktora ma byc wyswietlona
        //! \param source Zrodlo obiektu - sciezka do pliku z ktorego pochodzi
        template <class Ptr>
        bool trySet(const Ptr& object, const std::string& name, const std::string& source)
        {
            if (trySet(object)) {
                setName(name);
                setSource(source);
                return true;
            }
            return false;
        }

        virtual const void* getRawPtr() const = 0;
        virtual void* getRawPtr() = 0;
        virtual void reset() = 0;


        //! \return ID typu.
        virtual std::size_t getClassID() const = 0;
        //! \return Nazwa typu.
        virtual const std::string& getClassName() const = 0;
        //! \return Nazwa obiektu.
        virtual const std::string& getName() const = 0;
        //! \param name Nazwa obiektu.
        virtual void setName(const std::string& name) = 0;
        //! \return
        virtual const std::string& getSource() const = 0;
        //! \param source
        virtual void setSource(const std::string& source) = 0;

        //! \return
        virtual bool isChanged() const = 0;
        //! \param changed
        virtual void setChanged(bool changed) = 0;

        //! \param type
        //! \return Czy obiekt wspiera okreœlony typ?
        virtual bool isSupported(const TypeInfo& type) const = 0;
        //! \param type Typ inteligentnego wskaŸnika.
        //! \return true je¿eli do z obiektu mo¿na wy³uskaæ dany wskaŸnik.
        virtual bool isPtrSupported(const TypeInfo& type) const = 0;

        //! \return Informacje o typie.
        virtual TypeInfo getTypeInfo() const = 0;

        //! \return Informacje o typie odpowiednio normalnego i sta³ego wskaŸnika.
        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const = 0;

        //! \param supported Lista wspieranych rozszerzeñ.
        virtual void getSupportedTypes(Types& supported) const = 0;

        //! \return Czy wrappowany obiekt jest wyzerowany?
        virtual bool isNull() const = 0;

        //! \return Klon bie¿¹cego obiektu. Wewnêtrzny wskaŸnik równie¿ jest kopiowany.
        virtual ObjectWrapperPtr clone() const = 0;

        virtual ObjectWrapperPtr create() const = 0;

    protected:

        //! \return Czy uda³o siê ustawiæ m¹dry wskaŸnik?
        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const = 0;

        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type) = 0;
    };

    //! Baza dla typu wrapuj¹cego jakiœ obiekt. Poza trzymaniem metadanych klasy pochodne
    //! trzymaj¹ referencje do obiektów.
    class ObjectWrapper : public IObjectWrapper
    {
    protected:
        //! Nazwa instancji.
        std::string name;
        //! Nazwa typu.
        std::string className;
        //! Hash identyfikuj¹cy typ.
        std::size_t classID;
        //! Opis Ÿród³a.
        std::string source;
        //! Czy obiekt uleg³ zmianie?
        bool changed;

    public:
        //! \param object
        //! \param name
        //! \return Wrapper obiektu.
        template <class T>
        static ObjectWrapperPtr create(T* dummy = nullptr)
        {
            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
            typedef ObjectWrapperT<T> Wrapper;
            Wrapper * wrapper = new Wrapper();
            return ObjectWrapperPtr(wrapper);
        }

    protected:
        //! \param className
        //! \param classId
        ObjectWrapper(const char* className, const TypeInfo& type) :
             className(className), classID(0), changed(false)
             {}

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~ObjectWrapper()
        {

        }

    public:

        //! \return ID typu.
        virtual std::size_t getClassID() const
        {
            return classID;
        }
        //! \return Nazwa typu.
        virtual const std::string& getClassName() const
        {
            return className;
        }
        //! \return Nazwa obiektu.
        virtual const std::string& getName() const
        {
            return name;
        }
        //! \param name Nazwa obiektu.
        virtual void setName(const std::string& name)
        {
            this->name = name;
        }
        //! \return
        virtual const std::string& getSource() const
        {
            return source;
        }
        //! \param source
        virtual void setSource(const std::string& source)
        {
            this->source = source;
        }

        //! \return
        virtual bool isChanged() const
        {
            return changed;
        }
        //! \param changed
        virtual void setChanged(bool changed)
        {
            this->changed = changed;
        }
    };



    //! Deklaracja typu. Trzeba go specjalizowaæ za pomoc¹ makr. Ta wersja bêdzie
    //! rzucaæ statyczn¹ asercj¹.
    template <class T>
    class ObjectWrapperT : public ObjectWrapper
    {
    public:
        typedef T* Ptr;
        typedef const T* ConstPtr;
        Ptr get()
        {
            // rev co z tymi statycznymi asercjami?
            //UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego naglowka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
            return nullptr;
        }
        void set(Ptr)
        {
            // rev co z tymi statycznymi asercjami?
            //UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego naglowka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
        }
    };

    template <class T>
    struct ObjectWrapperTraits
    {
        static const bool isDefinitionVisible = false;
    };

    //! Pomocniczy typ bazowy, zarz¹dzaj¹cy obiektem za pomoc¹ parametru
    //! PtrPolicy. Tego typu nigdy nie u¿ywa siê wprost.
    template <class T, class PtrPolicyT, class ClonePolicyT>
    class __ObjectWrapperT : public ObjectWrapper, protected PtrPolicyT, protected ClonePolicyT
    {
    public:
        //!
        typedef PtrPolicyT PtrPolicy;
        //!
        typedef PtrPolicyT Base;
        //!
        typedef ClonePolicyT ClonePolicy;

        //! Wrappowany obiekt.
        typedef typename Base::template Ptr<T>::Type Ptr;
        //! Sta³y wrappowany obiekt
        typedef typename Base::template Ptr<const T>::Type ConstPtr;

    protected:
        //! Wrappowany obiekt.
        Ptr wrapped;

    protected:
        //! \param wrapped
        __ObjectWrapperT(const char* className)
            : ObjectWrapper(className, typeid(T))
        {
            PtrPolicyT::initPtr(wrapped);
        }

    public:
        //!
        virtual ~__ObjectWrapperT()
        {
            //PtrPolicy::setPtr<T>(wrapped, nullptr);
            reset();
        }

    public:
        //! \return Informacja o typie.
        virtual TypeInfo getTypeInfo() const
        {
            return typeid(T);
        }
        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const
        {
            return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));
        }

        virtual ObjectWrapperPtr clone() const {
            std::auto_ptr<ObjectWrapperT<T>> cloned(new ObjectWrapperT<T>(ObjectWrapperT<T>::className()));
            // rev czy na pewno nie trzeba <T> przy clone?
            T* newPtr = ClonePolicy::clone(&*wrapped);
            Ptr temp;
            PtrPolicy::setPtr(temp, newPtr);
            cloned->wrapped = temp;
            return ObjectWrapperPtr(cloned.release());
        }

        virtual ObjectWrapperPtr create() const {
            return ObjectWrapperPtr(new __ObjectWrapperT(this->getClassName().c_str()));
        }

        virtual const void* getRawPtr() const
        {
            return PtrPolicy::getRawPtr(wrapped);
        }

        virtual void* getRawPtr()
        {
            return PtrPolicy::getRawPtr(wrapped);
        }

        virtual void reset()
        {
            if(wrapped != nullptr){
                // rev usuniete <T>
                PtrPolicy::setPtr(wrapped, static_cast<T*>(nullptr));
            }
        }

        //! \param type
        //! \return Czy obiekt wspiera okreœlony typ?
        virtual bool isSupported(const TypeInfo& type) const
        {
            return type == typeid(T);
        }
        //! \param supported
        virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
        {
            supported.push_back(typeid(T));
        }
        //! \return Czy wewnêtrzny wskaŸnik jest wyzerowany?
        virtual bool isNull() const
        {
            return wrapped ? false : true;
        }

        virtual bool isPtrSupported(const TypeInfo& type) const
        {
            return type == typeid(Ptr) || type == typeid(ConstPtr);
        }

    protected:

        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const
        {
            if ( type == typeid(Ptr) ) {
                PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), wrapped);
                return true;
            } else if ( type == typeid(ConstPtr) ) {
                PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(wrapped));
                return true;
            }
            return false;
        }

        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type)
        {
            if ( type == typeid(Ptr) ) {
                PtrPolicy::setPtr(wrapped, *reinterpret_cast<const Ptr*>(ptr));
                return true;
            } else {
                return false;
            }
        }
    };

// rev - klasa przeniesiona do makrodefinicji
// wyglada na to, ze nie da sie odwolac do pola specjalizowanej klasy, po ktorej sie dziedziczy
/*
    template <class T, class B>
    class __ObjectWrapperTInherited : public ObjectWrapperT<B>
    {
    public:
        //! Wrappowany obiekt.
        typedef ObjectWrapperT<B> Base;
        //! U¿ycie PtrPolicy typu bazowego.
        typedef typename Base::PtrPolicy PtrPolicy;
        //! U¿ywany wskaŸnik.
        typedef typename PtrPolicy::template Ptr<T>::Type Ptr;
        //!
        typedef typename PtrPolicy::template Ptr<const T>::Type ConstPtr;

    protected:
        //! \param wrapped
        __ObjectWrapperTInherited(const char* className)
            : Base(className)
        {}
    public:
        //! \return Informacja o typie.
        virtual TypeInfo getTypeInfo() const
        {
            return typeid(T);
        }

        //virtual ObjectWrapperPtr clone() const {
        //    std::auto_ptr<__ObjectWrapperTInherited> cloned(new __ObjectWrapperTInherited(*this));
        //    T* newPtr = clone<T>(&*wrapped);
        //    Ptr temp;
        //    setPtr(temp, newPtr);
        //    cloned->wrapped = temp;
        //    return ObjectWrapperPtr(cloned.release());
        //}

        virtual ObjectWrapperPtr create() const {
            return ObjectWrapperPtr(new __ObjectWrapperTInherited(ObjectWrapperT<T>::className()));
        }

        //! \param type
        //! \return Czy obiekt wspira okreœlony typ?
        virtual bool isSupported(const TypeInfo& type) const
        {
            if ( type == typeid(T) ) {
                return true;
            } else {
                return Base::isSupported(type);
            }
        }
        //! \param supported
        virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
        {
            Base::getSupportedTypes(supported);
            supported.push_back(typeid(T));
        }

        //! \see ObjectWrapper::getPtrTypeInfo
        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const
        {
            return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));
        }

    protected:

        //! \see ObjectWrapper::getSmartPtr
        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const
        {
            if ( type == typeid(Ptr) ) {
                PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), static_pointer_cast<T>(this->wrapped));
                return true;
            } else if ( type == typeid(ConstPtr) ) {
                PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(static_pointer_cast<T>(this->wrapped)));
                return true;
            }
            return Base::getSmartPtr(ptr, type);
        }

        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type)
        {
            if ( type == typeid(Ptr) ) {
                Ptr temp;
                PtrPolicy::setPtr(temp, *reinterpret_cast<const Ptr*>(ptr));
                this->wrapped = static_pointer_cast<T>(temp);
                return true;
            } else {
                return Base::setSmartPtr(ptr, type);
            }
        }

    public:

        virtual bool isPtrSupported(const TypeInfo& type) const
        {
            return type == typeid(Ptr) || type == typeid(ConstPtr) || Base::isPtrSupported(type);
        }

    };*/

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#define __CORE_WRAPPER_DEFINE_META(type)                                                          \
template <> struct ObjectWrapperTraits<type>                                                      \
{                                                                                                 \
    static const bool isDefinitionVisible = true;                                                 \
};

namespace core {
	template<class T>
	class ObjectWrapperFactory;
}

// Makro tworz¹ce specjalizacjê ObjectWrapperT. Musi wystêpowaæ w globalnym
// namespace. Drugim parametrem mo¿e byæ dowolny typ maj¹cy cechy ptrPolicy,
// wbudowane: PtrPolicyRaw, PtrPolicyBoost, PtrPolicyOSG
#define CORE_DEFINE_WRAPPER(type, ptrPolicy, clonePolicy)                                         \
namespace core {                                                                                  \
    template <>                                                                                   \
class ObjectWrapperT<type> : public __ObjectWrapperT<type,  ptrPolicy, clonePolicy>               \
{                                                                                                 \
    friend class __ObjectWrapperT<type,  ptrPolicy, clonePolicy>;                                    \
    friend class ObjectWrapperFactory<type>;                                                             \
    friend class ObjectWrapper;                                                                         \
public:                                                                                           \
    typedef ObjectWrapperT<type> ObjectWrapperType;                                                                         \
    typedef __ObjectWrapperT<type,  ptrPolicy, clonePolicy> __Base;\
    typedef __Base::PtrPolicy PtrPolicy;\
    typedef  __Base::ClonePolicy ClonePolicy;\
    static const char* className() { return #type; }                                              \
protected:                                                                                         \
    ObjectWrapperT(const char* className = #type) :                                               \
    __ObjectWrapperT<type,ptrPolicy, clonePolicy >(className)                                     \
      {}                                                                                          \
};                                                                                                \
__CORE_WRAPPER_DEFINE_META(type)                                                                  \
}

// rev - makro zastapione nowym pelnieszym (ale nie uzywajacym posredniej klasy)
/*

#define CORE_DEFINE_WRAPPER_INHERITANCE(type, baseType)                                            \
namespace core {                                                                                   \
    template <>                                                                                    \
class ObjectWrapperT<type> : public __ObjectWrapperTInherited<type,  baseType>                     \
{                                                                                                   \
    friend class __ObjectWrapperTInherited<type,  baseType>;                                         \
    friend class ObjectWrapperFactory<type>;                                                             \
    friend class ObjectWrapper;                                                                         \
public:                                                                                            \
    typedef ObjectWrapperT<type> ObjectWrapperType;                                                                         \
    static const char* className() { return #type; }                                               \
protected:                                                                                            \
    ObjectWrapperT(const char* className = #type) :                                                \
    __ObjectWrapperTInherited<type,baseType>(className)                                            \
      {}                                                                                           \
};                                                                                                 \
__CORE_WRAPPER_DEFINE_META(type)                                                                   \
}
*/

#define CORE_DEFINE_WRAPPER_INHERITANCE(typeT, baseTypeT)\
namespace core {\
    template <>\
class ObjectWrapperT<typeT> : public ObjectWrapperT<baseTypeT>\
{\
    friend class ObjectWrapperFactory<typeT>;\
    friend class ObjectWrapper;\
public:\
    typedef typeT type;\
    typedef baseTypeT baseType;\
    typedef ObjectWrapperT<type> ObjectWrapperType;\
    static const char* className() { return #typeT; }\
    public:\
        typedef ObjectWrapperT<baseType> Base;\
        typedef Base::PtrPolicy PtrPolicy;\
        typedef PtrPolicy::Ptr<type>::Type Ptr;\
        typedef PtrPolicy::Ptr<const type>::Type ConstPtr;\
    protected:\
        ObjectWrapperT(const char* className = #typeT) :\
        Base(className)\
        {}\
    public:\
        virtual TypeInfo getTypeInfo() const\
        {\
            return typeid(type);\
        }\
        virtual ObjectWrapperPtr create() const {\
            return ObjectWrapperPtr(new ObjectWrapperT<type>(ObjectWrapperT<type>::className()));\
        }\
        virtual bool isSupported(const TypeInfo& type) const\
        {\
            if ( type == typeid(type) ) {\
                return true;\
            } else {\
                return Base::isSupported(type);\
            }\
        }\
        virtual void getSupportedTypes(ObjectWrapper::Types& supported) const\
        {\
            Base::getSupportedTypes(supported);\
            supported.push_back(typeid(type));\
        }\
        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const\
        {\
            return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));\
        }\
    protected:\
        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const\
        {\
            if ( type == typeid(Ptr) ) {\
                PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), static_pointer_cast<typeT>(this->wrapped));\
                return true;\
            } else if ( type == typeid(ConstPtr) ) {\
                PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(static_pointer_cast<typeT>(this->wrapped)));\
                return true;\
            }\
            return Base::getSmartPtr(ptr, type);\
        }\
        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type)\
        {\
            if ( type == typeid(Ptr) ) {\
                Ptr temp;\
                PtrPolicy::setPtr(temp, *reinterpret_cast<const Ptr*>(ptr));\
                this->wrapped = (Ptr)(temp);\
                return true;\
            } else {\
                return Base::setSmartPtr(ptr, type);\
            }\
        }\
    public:\
        virtual bool isPtrSupported(const TypeInfo& type) const\
        {\
            return type == typeid(Ptr) || type == typeid(ConstPtr) || Base::isPtrSupported(type);\
        }\
    };\
}

#endif  // __HEADER_GUARD_CORE__OBJECTWRAPPER_H__
