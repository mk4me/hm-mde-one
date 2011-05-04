/********************************************************************
	created:  2011/02/11
	created:  11:2:2011   15:32
	filename: ObjectWrapper.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__OBJECTWRAPPER_H__
#define __HEADER_GUARD_CORE__OBJECTWRAPPER_H__

#include <list>
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
    //! Deklaracja typu. Trzeba go specjalizowa� za pomoc� makr. Ta wersja b�dzie
    //! rzuca� statyczn� asercj�.
    template <class T>
    class ObjectWrapperT : public ObjectWrapper
    {
    public:
        typedef T* Ptr;
        Ptr get()
        {
            UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego naglowka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
            return nullptr;
        }
        void set(Ptr)
        {
            UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego naglowka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
        }
    };

    template <class T>
    struct ObjectWrapperTraits
    {
        static const bool isDefinitionVisible = false;
    };

    //! Baza dla typu wrapuj�cego jaki� obiekt. Poza trzymaniem metadanych klasy pochodne
    //! trzymaj� referencje do obiekt�w.
    class ObjectWrapper
    {
    public:
        //! Lista typ�w.
        typedef std::list<TypeInfo> Types;
    protected:
        //! Nazwa instancji.
        std::string name;
        //! Nazwa typu.
        std::string className;
        //! Hash identyfikuj�cy typ.
        std::size_t classID;
        //! Opis �r�d�a.
        std::string source;
        //! Czy obiekt uleg� zmianie?
        bool changed;
        //! Return type resolver
        struct get_t
        {
            ObjectWrapper* wrapper;
            const ObjectWrapper* constWrapper;
            bool exact;

            template <class Ptr>
            operator Ptr()
            {
                Ptr result;
                if (wrapper->tryGet(result, exact)) {
                    return result;
                } else {
                    throw std::bad_cast("Invalid cast");
                }
            }
            template <class Ptr>
            operator Ptr() const
            {
                Ptr result;
                if (constWrapper->tryGet(result, exact)) {
                    return result;
                } else {
                    throw std::bad_cast("Invalid cast");
                }
            }
        };

    public:
        //! \param object
        //! \param name
        //! \return Wrapper obiektu.
        template <class T>
        static shared_ptr<ObjectWrapper> create(T* dummy = nullptr)
        {
            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
            typedef ObjectWrapperT<T> Wrapper;
            Wrapper * wrapper = new Wrapper();
            return shared_ptr<ObjectWrapper>(wrapper);
        }

        //! \return Nazwa dla zadanego typu.
        template <class T>
        static const char* getClassName(T* dummy = nullptr)
        {
            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
            return ObjectWrapperT<T>::className();
        }

        //! \param type Typ do por�wnania.
        //! \return Czy to te same typy?
        inline bool isTypeEqual(const TypeInfo& type)
        {
            return getTypeInfo() == type;
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

        //! Pr�ba pobrania obiektu z wrappera.
        //! \param object Rezultat.
        //! \param exact Czy ma by� tylko i wy��cznie ten typ czy te� mo�e by� rzutowanie w d�?
        //! \return Sukces/pora�ka.
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

        //! Pr�ba pobrania obiektu z wrappera.
        //! \param object Rezultat.
        //! \param exact Czy ma by� tylko i wy��cznie ten typ czy te� mo�e by� rzutowanie w d�?
        //! \return Sukces/pora�ka.
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

        //! Pobiera obiekt z wrappera. W razie b��du rzuca bad_castem.
        //! \param exact Czy ma by� tylko i wy��cznie ten typ czy te� mo�e by� rzutowanie w d�?
        //! \return Return Type Resolver
        get_t get(bool exact = false)
        {
            get_t result = { this, nullptr, exact };
            return result;
        }

        //! \param dummy Pozostawi� pusty.
        //! \return Wrappowany obiekt. Gdy wrapper jest innego typu ni� parametr szablonu rzucany jest wyj�tek.
        template <class T>
        typename ObjectWrapperT<T>::Ptr get(T* dummy = nullptr)
        {   
            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
            typedef ObjectWrapperT<T> Wrapper;
            return static_cast<Wrapper::Ptr>(get());
        }

        //! Pobiera obiekt z wrappera. W razie b��du rzuca bad_castem.
        //! \param exact Czy ma by� tylko i wy��cznie ten typ czy te� mo�e by� rzutowanie w d�?
        //! \return Return Type Resolver
        const get_t get(bool exact = false) const
        {
            const get_t result = { nullptr, this, exact };
            return result;
        }

        //! Ustawia obiekt wrappera. W razie b��du rzuca bad_castem.
        //! \param object Obiekt.
        template <class Ptr>
        void set(Ptr& object)
        {
            if ( !trySet(object) ) {
                throw std::bad_cast("Invalid cast");
            }
        }

        //! Ustawia obiekt wrappera.
        //! \param object Obiekt.
        //! \return Sukces/pora�ka.
        template <class Ptr>
        bool trySet(Ptr& object)
        {
            TypeInfo type = typeid(Ptr);
            if ( getPtrTypeInfo().first == type ) {
                return setSmartPtr(&object, type);
            } else {
                return false;
            }
        }

        //! \return ID typu.
        inline std::size_t getClassID() const
        { 
            return classID;
        }
        //! \return Nazwa typu.
        inline const std::string& getClassName() const
        {
            return className;
        }
        //! \return Nazwa obiektu.
        inline const std::string& getName() const
        {
            return name;
        }
        //! \param name Nazwa obiektu.
        inline void setName(const std::string& name)
        {
            this->name = name;
        }
        //! \return
        inline const std::string& getSource() const
        { 
            return source;
        }
        //! \param source
        void setSource(const std::string& source) 
        { 
            this->source = source; 
        }


        //!
        inline TypeInfo getType() const
        {
            return getTypeInfo();
        }

        //! \return
        bool isChanged() const
        { 
            return changed;
        }
        //! \param changed
        void setChanged(bool changed) 
        { 
            this->changed = changed; 
        }


        //! \param type 
        //! \return Czy obiekt wspiera okre�lony typ?
        virtual bool isSupported(const TypeInfo& type) const = 0;
        //! \param type Typ inteligentnego wska�nika.
        //! \return true je�eli do z obiektu mo�na wy�uska� dany wska�nik.
        virtual bool isPtrSupported(const TypeInfo& type) const = 0;

        //! \return Informacje o typie.
        virtual TypeInfo getTypeInfo() const = 0;

        //! \return Informacje o typie odpowiednio normalnego i sta�ego wska�nika.
        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const = 0;

        //! \param supported Lista wspieranych rozszerze�.
        virtual void getSupportedTypes(Types& supported) const = 0;

        //! \return Czy wrappowany obiekt jest wyzerowany?
        virtual bool isNull() const = 0;

        //! \return Klon bie��cego obiektu. Wewn�trzny wska�nik r�wnie� jest kopiowany.
        virtual shared_ptr<ObjectWrapper> clone() const = 0;

    private:

        //! \return Czy uda�o si� ustawi� m�dry wska�nik?
        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const = 0;

        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type) = 0;
    };

    typedef shared_ptr<ObjectWrapper> ObjectWrapperPtr;
    typedef shared_ptr<const ObjectWrapper> ObjectWrapperConstPtr;
    typedef weak_ptr<ObjectWrapper> ObjectWrapperWeakPtr;

    //! Pomocniczy typ bazowy, zarz�dzaj�cy obiektem za pomoc� parametru
    //! PtrPolicy. Tego typu nigdy nie u�ywa si� wprost.
    template <class T, class PtrPolicy, class ClonePolicy>
    class __ObjectWrapperT : public ObjectWrapper, protected PtrPolicy, protected ClonePolicy
    {

    public:
        //!
        typedef PtrPolicy PtrPolicy;
        //!
        typedef PtrPolicy Base;
        //! Wrappowany obiekt.
        typedef typename Base::Ptr<T>::Type Ptr;
        //! Sta�y wrappowany obiekt
        typedef typename Base::Ptr<const T>::Type ConstPtr;

    protected:
        //! Wrappowany obiekt.
        Ptr wrapped;

    protected:
        //! \param wrapped
        __ObjectWrapperT(const char* className)
            : ObjectWrapper(className, typeid(T))
        {
            PtrPolicy::initPtr(wrapped);
        }

    public:
        //!
        virtual ~__ObjectWrapperT()
        {
            PtrPolicy::setPtr<T>(wrapped, nullptr);
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
            std::auto_ptr<__ObjectWrapperT> cloned(new __ObjectWrapperT(*this));
            T* newPtr = ClonePolicy::clone<T>(&*wrapped);
            Ptr temp;
            PtrPolicy::setPtr(temp, newPtr);
            cloned->wrapped = temp;
            return ObjectWrapperPtr(cloned.release());
        }

        //! \param type 
        //! \return Czy obiekt wspiera okre�lony typ?
        virtual bool isSupported(const TypeInfo& type) const
        {
            return type == typeid(T);
        }
        //! \param supported
        virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
        {
            supported.push_back(typeid(T));
        }
        //! \return Czy wewn�trzny wska�nik jest wyzerowany?
        virtual bool isNull() const
        {
            return wrapped ? false : true;
        }

        virtual bool isPtrSupported(const TypeInfo& type) const
        {
            return type == typeid(Ptr) || type == typeid(ConstPtr);
        }

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

    template <class T, class B>
    class __ObjectWrapperTInherited : public ObjectWrapperT<B>
    {
    public:
        //! Wrappowany obiekt.
        typedef ObjectWrapperT<B> Base;
        //! U�ycie PtrPolicy typu bazowego.
        typedef typename Base::PtrPolicy PtrPolicy;
        //! U�ywany wska�nik.
        typedef typename PtrPolicy::Ptr<T>::Type Ptr;
        //! 
        typedef typename PtrPolicy::Ptr<const T>::Type ConstPtr;
    public:
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

        //! \param type 
        //! \return Czy obiekt wspira okre�lony typ?
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

        virtual bool isPtrSupported(const TypeInfo& type) const
        {
            return type == typeid(Ptr) || type == typeid(ConstPtr) || Base::isPtrSupported(type);
        }

    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#define __CORE_WRAPPER_DEFINE_META(type)                                                          \
template <> struct ObjectWrapperTraits<type>                                                      \
{                                                                                                 \
    static const bool isDefinitionVisible = true;                                                 \
};     

// Makro tworz�ce specjalizacj� ObjectWrapperT. Musi wyst�powa� w globalnym
// namespace. Drugim parametrem mo�e by� dowolny typ maj�cy cechy ptrPolicy,
// wbudowane: PtrPolicyRaw, PtrPolicyBoost, PtrPolicyOSG
#define CORE_DEFINE_WRAPPER(type, ptrPolicy, clonePolicy)                                         \
namespace core {                                                                                  \
    template <>                                                                                   \
class ObjectWrapperT<type> : public __ObjectWrapperT<type,  ptrPolicy, clonePolicy>               \
{                                                                                                 \
public:                                                                                           \
    static const char* className() { return #type; }                                              \
public:                                                                                           \
    ObjectWrapperT(const char* className = #type) :                                               \
    __ObjectWrapperT<type,ptrPolicy, clonePolicy >(className)                                     \
      {}                                                                                          \
};                                                                                                \
__CORE_WRAPPER_DEFINE_META(type)                                                                  \
}


#define CORE_DEFINE_WRAPPER_INHERITANCE(type, baseType)                                            \
namespace core {                                                                                   \
    template <>                                                                                    \
class ObjectWrapperT<type> : public __ObjectWrapperTInherited<type,  baseType>                     \
{                                                                                                  \
public:                                                                                            \
    static const char* className() { return #type; }                                               \
public:                                                                                            \
    ObjectWrapperT(const char* className = #type) :                                                \
    __ObjectWrapperTInherited<type,baseType>(className)                                            \
      {}                                                                                           \
};                                                                                                 \
__CORE_WRAPPER_DEFINE_META(type)                                                                   \
}


#endif  // __HEADER_GUARD_CORE__OBJECTWRAPPER_H__