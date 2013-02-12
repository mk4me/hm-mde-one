/********************************************************************
	created:  2011/02/11
	created:  11:2:2011   15:32
	filename: ObjectWrapper.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__
#define __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__

#include <set>
#include <string>
#include <map>
#include <list>
#include <boost/type_traits.hpp>
#include <boost/function.hpp>
#include <utils/TypeInfo.h>
#include <utils/Debug.h>
#include <utils/PtrPolicyBoost.h>
#include <utils/ClonePolicies.h>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////
    class ObjectWrapper;
    typedef boost::shared_ptr<ObjectWrapper> ObjectWrapperPtr;
    typedef boost::shared_ptr<const ObjectWrapper> ObjectWrapperConstPtr;
    typedef boost::weak_ptr<ObjectWrapper> ObjectWrapperWeakPtr;
    typedef boost::weak_ptr<const ObjectWrapper> ObjectWrapperConstWeakPtr;

    typedef std::set<ObjectWrapperPtr> Objects;
    typedef std::set<ObjectWrapperConstPtr> ConstObjects;

	typedef std::list<ObjectWrapperPtr> ObjectsList;
	typedef std::list<ObjectWrapperConstPtr> ConstObjectsList;

	template<class T>
	class ObjectWrapperT;

	template <class T, class PtrPolicyT, class ClonePolicyT>
	class __ObjectWrapperT;

    //! Baza dla typu wrapującego jakiś obiekt. Poza trzymaniem metadanych klasy pochodne
    //! trzymają referencje do obiektów.
    class ObjectWrapper : public std::map<std::string, std::string>
    {
		template<class T>
		friend class ObjectWrapperT;
		
		template <class T, class PtrPolicyT, class ClonePolicyT>
		friend class __ObjectWrapperT;

	public:
		//! Typ inicjalizatora danych - używany do leniwej inicjalizacji
		typedef boost::function<void(ObjectWrapper &)> LazyInitializer;

	private:
		//! Inicjalizator danych
		LazyInitializer initializer_;
		//! Czy obiekt był już inicjalizowany
		bool initialized;

	public:

		//! Lista typów.
		typedef TypeInfoList Types;

		//! Return type resolver
		struct get_t
		{
			ObjectWrapper* wrapper;
			const ObjectWrapper* constWrapper;
			bool exact;

			template <class Ptr>
			operator const Ptr()
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
			operator const Ptr() const
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

		const bool tryGetMeta(const std::string & key, std::string & val) const;

		void copyMeta(const ObjectWrapper & srcWrapper);

		template <class Ptr>
		operator const Ptr()
		{
			Ptr result;
			tryGet(result, false);
			return result;
		}

		template <class Ptr>
		operator const Ptr() const
		{
			Ptr result;
			tryGet(result, false);
			return result;
		}

		//! \param object
		//! \param name
		//! \return Wrapper obiektu.
		template <class T>
		static const ObjectWrapperPtr create(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
			return ObjectWrapperPtr(new ObjectWrapperT<T>());
		}

		//! \return Nazwa dla zadanego typu.
		template <class T>
		static const char* getClassName(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			return ObjectWrapperT<T>::className();
		}

		//! Próba pobrania obiektu z wrappera.
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool tryGet(Ptr& object, bool exact = false)
		{
			return __tryGetRawPtr(object, exact);
		}

		//! Próba pobrania obiektu z wrappera.
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool tryGet(Ptr& object, bool exact = false) const
		{
			return __tryGetRawPtr(object, exact, boost::is_pointer<Ptr>());
		}

		//! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Return Type Resolver
		get_t get(bool exact = false);

		//! \param dummy Pozostawić pusty.
		//! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
		template <class T>
		const typename ObjectWrapperT<T>::Ptr get(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			return static_cast<ObjectWrapperT<T>::Ptr>(get());
		}

		//! \param dummy Pozostawić pusty.
		//! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
		template <class T>
		const typename ObjectWrapperT<T>::ConstPtr get(T* dummy = nullptr) const
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			return static_cast<ObjectWrapperT<T>::ConstPtr>(get());
		}

		//! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Return Type Resolver
		const get_t get(bool exact = false) const;

		//! Ustawia obiekt wrappera. W razie błędu rzuca bad_castem.
		//! \param object Obiekt.
		template <class Ptr>
		void set(const Ptr& object)
		{
			if ( !trySet(object) ) {
				throw std::bad_cast();
			}
		}

		void set(const LazyInitializer & initializer);

		//! Ustawia obiekt wrappera.
		//! \param object Obiekt.
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool trySet(const Ptr& object)
		{
			return __trySetRawPtr(object, boost::is_pointer<Ptr>());
		}

    private:

        //! \param className
        //! \param classId
        ObjectWrapper();

		ObjectWrapper(const ObjectWrapper & wrapper);

		//! Ustawia obiekt wrappera.
		//! \param object Obiekt.
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __trySetRawPtr(const Ptr& object, boost::true_type)
		{
			UTILS_STATIC_ASSERT((!boost::is_const<typename boost::remove_pointer<Ptr>::type>::value), "Nalezy zapisywac dane bez modyfikatora const");
			if(getPtrTypeInfo().first == typeid(Ptr)){
				__setData(&object);
				return true;
			}
			return false;
		}

		template <class Ptr>
		const bool __trySetRawPtr(const Ptr& object, boost::false_type)
		{
			UTILS_STATIC_ASSERT((!boost::is_const<typename Ptr::element_type>::value), "Nalezy zapisywac dane bez modyfikatora const");
			if(getPtrTypeInfo().first == typeid(Ptr)){
				__setData(&object);
				return true;
			}
			return false;
		}

		//! Próba pobrania obiektu z wrappera.
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __tryGetRawPtr(Ptr& object, bool exact)
		{
			TypeInfo ptrInfo(typeid(Ptr));
			if ( ptrInfo == getPtrTypeInfo().first || ptrInfo == getPtrTypeInfo().second ) {
				return __tryUnpackData(&object, ptrInfo);
			} else if(exact == false && isPtrSupported(ptrInfo) == true){
				return __tryUnpackBaseData(&object, ptrInfo);
			} else {
				return false;
			}
		}

		//! Próba pobrania obiektu z wrappera.
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __tryGetRawPtr(Ptr& object, bool exact, boost::true_type) const
		{
			UTILS_STATIC_ASSERT((boost::is_const<typename boost::remove_pointer<Ptr>::type>::value), "Ta metoda mozna pobierac tylko obiekty typu const");		

			TypeInfo ptrInfo(typeid(Ptr));
			if ( ptrInfo == getPtrTypeInfo().second ) {
				return __tryUnpackData(&object, ptrInfo);
			} else if(exact == false && isPointerSupported(ptrInfo) == true){
				return __tryUnpackBaseData(&object, ptrInfo);
			} else {
				return false;
			}
		}

		//! Próba pobrania obiektu z wrappera.
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __tryGetRawPtr(Ptr& object, bool exact, boost::false_type) const
		{
			UTILS_STATIC_ASSERT((boost::is_const<typename Ptr::element_type>::value), "Ta metoda mozna pobierac tylko obiekty typu const");		

			TypeInfo ptrInfo(typeid(Ptr));
			if ( ptrInfo == getPtrTypeInfo().second ) {
				return __tryUnpackData(&object, ptrInfo);
			} else if(exact == false && isPtrSupported(ptrInfo) == true){
				return __tryUnpackBaseData(&object, ptrInfo);
			} else {
				return false;
			}
		}

		const bool __tryUnpackData(void * object, const TypeInfo & ptrType);

		const bool __tryUnpackData(void * object, const TypeInfo & ptrType) const;

		const bool __tryUnpackBaseData(void * object, const TypeInfo & ptrType);

		const bool __tryUnpackBaseData(void * object, const TypeInfo & ptrType) const;

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~ObjectWrapper();

        //! \return Nazwa typu.
        virtual const std::string& getClassName() const = 0;

		//! \return Informacje o typie.
		virtual const TypeInfo & getTypeInfo() const = 0;

		//! \param type 
		//! \return Czy obiekt wspiera określony typ?
		virtual const bool isSupported(const TypeInfo& type) const;

		//! \param supported Lista wspieranych rozszerzeń.
		virtual void getSupportedTypes(Types& supported) const;

		//! \return Czy wrappowany obiekt jest wyzerowany?
		const bool isNull() const;

		const LazyInitializer & initializer() const;

		void initialize() const;

		//! \return Klon bieżącego obiektu. Wewnętrzny wskaźnik również jest kopiowany.
		const ObjectWrapperPtr clone() const;

		virtual const ObjectWrapperPtr create() const = 0;

		virtual const void* getRawPtr() const = 0;

		virtual void* getRawPtr() = 0;

		//! \return Informacje o typie odpowiednio normalnego i stałego wskaźnika.
		virtual const std::pair<TypeInfo, TypeInfo> & getPtrTypeInfo() const = 0;

		virtual const bool isPtrSupported(const TypeInfo & ptrInfo) const = 0;

		void reset();

		void swap(ObjectWrapper & ow);

	private:

		virtual void __clone(const ObjectWrapperPtr & dest) const = 0;

		//! \param ow Object wrapper z ktorym zamieniamy zawartość
		//! Ważne!! To może rzucać wyjątkiem!!
		virtual void __swap(ObjectWrapper & ow) = 0;

		virtual void __reset() = 0;
		//! \return Czy udało się ustawić mądry wskaźnik?
		virtual void __setData(const void * object) = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual const bool __tryGetData(void * object, const TypeInfo & ptrType) = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual const bool __tryGetData(void * object, const TypeInfo & ptrType) const = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType) = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType) const = 0;
    };

    //! Pomocniczy typ bazowy, zarządzający obiektem za pomocą parametru
    //! PtrPolicy. Tego typu nigdy nie używa się wprost.
    template <class T, class PtrPolicyT, class ClonePolicyT>
    class __ObjectWrapperT : public ObjectWrapper, protected PtrPolicyT, protected ClonePolicyT
    {
		friend class ObjectWrapperT<T>;

    public:
        //! Mechanizm inteligetnego wskaźnika obsługujący dane
        typedef PtrPolicyT PtrPolicy;
        //! Bazowy inteligetny wskaznik obsługujacy dane
        typedef PtrPolicyT Base;
        //! Polityka klonowania danych
        typedef ClonePolicyT ClonePolicy;

        //! Wrappowany obiekt.
        typedef typename Base::template Ptr<T>::Type Ptr;
        //! Stały wrappowany obiekt
        typedef typename Base::template Ptr<const T>::Type ConstPtr;

	private:
		typedef __ObjectWrapperT<T, PtrPolicy, ClonePolicy> ImplType;

    private:
        //! Wrappowany obiekt.
        Ptr wrapped_;

	protected:

		const Ptr & wrapped()
		{
			return wrapped_;
		}

		const ConstPtr wrapped() const
		{
			return wrapped_;
		}

    private:
        //! \param wrapped
        __ObjectWrapperT() : ObjectWrapper()
        {
			PtrPolicy::initPtr(wrapped_);
        }

		__ObjectWrapperT(const __ObjectWrapperT & wrapper)
			: ObjectWrapper(wrapper), wrapped_(wrapper.wrapped_)
		{
		
		}

		virtual void __setData(const void * object)
		{
			__setData(object, wrapped_);
		}

		virtual const bool __tryGetData(void * object, const TypeInfo & ptrType)
		{
			return __tryGetData(object, ptrType, wrapped_);
		}

		virtual const bool __tryGetData(void * object, const TypeInfo & ptrType) const
		{
			return __tryGetConstData(object, ptrType, wrapped_);
		}

		virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType)
		{
			//UTILS_STATIC_ASSERT((false), "Nie mozna pobierac typu bazowego dla tego ObjectWrappera - on jest baza")
			return false;
		}

		virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType) const
		{
			//UTILS_STATIC_ASSERT((false), "Nie mozna pobierac typu bazowego dla tego ObjectWrappera - on jest baza")
			return false;
		}

    public:
        //!
        virtual ~__ObjectWrapperT()
        {

        }

		static void __setData(const void * object, Ptr & wrapped)
		{
			PtrPolicy::setPtr(wrapped, *(static_cast<const Ptr *>(object)));
		}

		template<class PtrIn>
		static const bool __tryGetData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{
			if(ptrType == ptrTypeInfo().first){
				*(static_cast<Ptr*>(object)) = wrapped;
			}else if(ptrType == ptrTypeInfo().second){
				*(static_cast<ConstPtr*>(object)) = wrapped;
			}else{
				//tu nas nigdy nie powinno być - powinniśmy rzucać wyjatkiem?
				return false;
			}

			return true;
		}

		template<class PtrIn>
		static const bool __tryGetConstData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{
			if(ptrType == ptrTypeInfo().second){
				*(static_cast<ConstPtr*>(object)) = wrapped;
			}else{
				//tu nas nigdy nie powinno być - powinniśmy rzucać wyjatkiem?
				return false;
			}

			return true;
		}

		template<class PtrIn>
		static const bool __tryGetBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{
			return false;
		}

		template<class PtrIn>
		static const bool __tryGetConstBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped)
		{
			return false;
		}

		virtual const bool isPtrSupported(const TypeInfo & ptrInfo) const
		{
			return ptrTypeSupported(ptrInfo);
		}

		static const std::pair<TypeInfo, TypeInfo> & ptrTypeInfo()
		{
			static const std::pair<TypeInfo, TypeInfo> ptrTypeInfo_ = std::make_pair(TypeInfo(typeid(Ptr)), TypeInfo(typeid(ConstPtr)));
			return ptrTypeInfo_;
		}

		static const bool ptrTypeSupported(const TypeInfo & ptrInfo)
		{			
			return ptrInfo == ptrTypeInfo().first || ptrInfo == ptrTypeInfo().second;
		}

		static const bool typeSupported(const TypeInfo & type)
		{
			return type == typeInfo();
		}

		static void supportedTypes(Types & types)
		{
			types.push_back(typeInfo());
		}

		virtual const std::pair<TypeInfo, TypeInfo> & getPtrTypeInfo() const
		{
			return ptrTypeInfo();
		}

		static const TypeInfo & typeInfo()
		{
			static const TypeInfo typeInfo_ = TypeInfo(typeid(T));
			return typeInfo_;
		}

		//! \return Nazwa typu.
		virtual const std::string& getClassName() const
		{
			return ObjectWrapperT<T>::className();
		}

		//! \return Informacje o typie.
		virtual const TypeInfo & getTypeInfo() const
		{
			return typeInfo();
		}

        virtual void __clone(const ObjectWrapperPtr & dest) const {
			T* newPtr = ClonePolicy::clone(&*wrapped_);
			Ptr temp;
			PtrPolicy::setPtr(temp, newPtr);
			auto cloned = boost::dynamic_pointer_cast<ImplType>(dest);
			cloned->wrapped_ = temp;
        }

        virtual const ObjectWrapperPtr create() const {
            return ObjectWrapper::create<T>();
        }

        virtual const void* getRawPtr() const
        {
            return PtrPolicy::getConstRawPtr(wrapped_);
        }

        virtual void* getRawPtr()
        {
            return PtrPolicy::getRawPtr(wrapped_);
        }

	private:

		virtual void __swap(ObjectWrapper & ow)
		{
			auto owT = dynamic_cast<ImplType*>(&ow);
			if(owT == nullptr){
				throw std::runtime_error("Wrong swap ObjectWrapper");
			}

			PtrPolicy::swapPtr(wrapped_, owT->wrapped_);
		}

        virtual void __reset()
        {
            PtrPolicy::setPtr(wrapped_, static_cast<T*>(nullptr));
        }
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

template <class T>
struct ObjectWrapperTraits
{
	static const bool isDefinitionVisible = false;

	struct PtrPolicy
	{
		//UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
	};

	struct Ptr
	{
		//UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
	};

	struct ConstPtr
	{
		//UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
	};

	struct ClonePolicy
	{
		//UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia DEFINE_WRAPPER.");
	};
};

#define __DEFINE_WRAPPER_META(type, ptrPolicy, clonePolicy)\
template <> struct ObjectWrapperTraits<type>\
{\
    static const bool isDefinitionVisible = true;\
	typedef ptrPolicy PtrPolicy;\
	typedef PtrPolicy::Ptr<type>::Type Ptr;\
	typedef PtrPolicy::Ptr<const type>::Type ConstPtr;\
	typedef clonePolicy ClonePolicy;\
};

#define __DEFINE_WRAPPER_DERIVED_META(typeT, baseTypeT)\
template <> struct ObjectWrapperTraits<typeT>\
{\
	static const bool isDefinitionVisible = ObjectWrapperTraits<baseTypeT>::isDefinitionVisible;\
	typedef ObjectWrapperTraits<baseTypeT>::PtrPolicy PtrPolicy;\
	typedef PtrPolicy::Ptr<typeT>::Type Ptr;\
	typedef PtrPolicy::Ptr<const typeT>::Type ConstPtr;\
	typedef ObjectWrapperTraits<baseTypeT>::ClonePolicy ClonePolicy;\
};

namespace utils {
//! Deklaracja typu. Trzeba go specjalizować za pomocą makr. Ta wersja będzie
//! rzucać statyczną asercją.
template <class T>
class ObjectWrapperT : public ObjectWrapper
{
public:
	//! \return Nazwa typu.
	virtual const std::string& getClassName() const
	{
		const static std::string className = "";
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return className;
	}

	//! \return Informacje o typie.
	virtual const TypeInfo & getTypeInfo() const
	{
		static const TypeInfo typeInfo_ = TypeInfo();
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return typeInfo_;
	}

	//! \return Klon bieżącego obiektu. Wewnętrzny wskaźnik również jest kopiowany.
	virtual const ObjectWrapperPtr clone() const
	{		
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return ObjectWrapperPtr();
	}

	virtual const ObjectWrapperPtr create() const
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return ObjectWrapperPtr();
	}

	virtual const void* getRawPtr() const
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return nullptr;
	}

	virtual void* getRawPtr()
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return nullptr;
	}

	//! \return Informacje o typie odpowiednio normalnego i stałego wskaźnika.
	virtual const std::pair<TypeInfo, TypeInfo> & getPtrTypeInfo() const
	{
		static const std::pair<TypeInfo, TypeInfo> ptrTypeInfo_ = std::pair<TypeInfo, TypeInfo>();
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");		
		return ptrTypeInfo_;
	}

	virtual const bool isPtrSupported(const TypeInfo & ptrInfo) const
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return false;
	}

private:

	virtual void __reset()
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
	}

	virtual const bool __setData(const void * object)
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
	}

	virtual const bool __tryGetData(void * object, const TypeInfo & ptrType)
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return false;
	}

	virtual const bool __tryGetData(void * object, const TypeInfo & ptrType) const
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return false;
	}

	virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType)
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return false;
	}

	virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType) const
	{
		UTILS_STATIC_ASSERT((false), "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
		return false;
	}
};
}

// Makro tworzące specjalizację ObjectWrapperT. Musi występować w globalnym
// namespace. Drugim parametrem może być dowolny typ mający cechy ptrPolicy,
// wbudowane: PtrPolicyRaw, PtrPolicyBoost, PtrPolicyOSG
#define DEFINE_WRAPPER(type, ptrPolicy, clonePolicy)\
namespace utils {\
	__DEFINE_WRAPPER_META(type, ptrPolicy, clonePolicy)\
    template <>\
class ObjectWrapperT<type> : public __ObjectWrapperT<type,  ptrPolicy, clonePolicy>\
{\
	friend class __ObjectWrapperT<type,  ptrPolicy, clonePolicy>;\
	friend class ObjectWrapper;\
public:\
	typedef type Type;\
	typedef type BaseType;\
    typedef ObjectWrapperT<Type> ObjectWrapperType;\
    typedef __ObjectWrapperT<Type,  ptrPolicy, clonePolicy> Base;\
private: \
    ObjectWrapperT() : Base() {}\
public:\
	static const std::string & className(){\
		static const std::string className_ = #type;\
		return className_;\
	}\
};\
}

#define DEFINE_WRAPPER_INHERITANCE(typeT, baseTypeT)\
namespace utils {\
__DEFINE_WRAPPER_DERIVED_META(typeT, baseTypeT)\
	template<>\
class ObjectWrapperT<typeT> : public __ObjectWrapperT<typeT, ObjectWrapperTraits<baseTypeT>::PtrPolicy, ObjectWrapperTraits<baseTypeT>::ClonePolicy>\
{\
	friend class __ObjectWrapperT<typeT, ObjectWrapperTraits<baseTypeT>::PtrPolicy, ObjectWrapperTraits<baseTypeT>::ClonePolicy>;\
	friend class ObjectWrapper;\
	template<class T> friend class ObjectWrapperT;\
public:\
	typedef typeT Type;\
	typedef baseTypeT BaseType;\
	typedef ObjectWrapperT<Type> ObjectWrapperType;\
	typedef __ObjectWrapperT<typeT, ObjectWrapperTraits<baseTypeT>::PtrPolicy, ObjectWrapperTraits<baseTypeT>::ClonePolicy> Base;\
private:\
	ObjectWrapperT() : Base() {}\
	template<class PtrIn>\
	static const bool __tryGetBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped) {\
		if(ObjectWrapperT<baseTypeT>::ptrTypeInfo().first == ptrType || ObjectWrapperT<Type>::ptrTypeInfo().second == ptrType){\
			return ObjectWrapperT<baseTypeT>::__tryGetData(object, ptrType, wrapped);\
		}\
		return ObjectWrapperT<baseTypeT>::__tryGetBaseData(object, ptrType, wrapped);\
	}\
	template<class PtrIn>\
	static const bool __tryGetConstBaseData(void * object, const TypeInfo & ptrType, const PtrIn & wrapped) {\
		if(ObjectWrapperT<baseTypeT>::ptrTypeInfo().second == ptrType){\
			return ObjectWrapperT<baseTypeT>::__tryGetConstBaseData(object, ptrType, wrapped);\
		}\
		return ObjectWrapperT<baseTypeT>::__tryGetConstBaseData(object, ptrType, wrapped);\
	}\
	virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType)\
	{\
		return __tryGetBaseData(object, ptrType, wrapped());\
	}\
	virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType) const\
	{\
		return __tryGetConstBaseData(object, ptrType, wrapped());\
	}\
public:\
	static void supportedTypes(Types & types) {	ObjectWrapperT<baseTypeT>::supportedTypes(types); Base::supportedTypes(types); }\
	static const bool typeSupported(const TypeInfo & type) { return Base::typeSupported(type) || ObjectWrapperT<baseTypeT>::typeSupported(type); }\
	virtual const bool isPtrSupported(const TypeInfo & ptrInfo) const {\
		return Base::isPtrSupported(ptrInfo) || ObjectWrapperT<baseTypeT>::ptrTypeSupported(ptrInfo);\
	}\
	virtual const bool isSupported(const TypeInfo& type) const {\
		return Base::isSupported(type) || ObjectWrapperT<baseTypeT>::typeSupported(type);\
	}\
	virtual void getSupportedTypes(Types& supported) const {\
		supportedTypes(supported);\
	}\
	static const std::string & className(){\
		static const std::string className_ = #typeT;\
		return className_;\
	}\
};\
}

//old impl

//	class IObjectWrapper;
//    typedef boost::shared_ptr<IObjectWrapper> ObjectWrapperPtr;
//    typedef boost::shared_ptr<const IObjectWrapper> ObjectWrapperConstPtr;
//    typedef boost::weak_ptr<IObjectWrapper> ObjectWrapperWeakPtr;
//    typedef boost::weak_ptr<const IObjectWrapper> ObjectWrapperConstWeakPtr;
//
//    typedef std::set<ObjectWrapperPtr> Objects;
//    typedef std::set<ObjectWrapperConstPtr> ConstObjects;
//
//    template <class OWT> class ObjectWrapperT;
//
//    class IObjectWrapper : public std::map<std::string, std::string>
//    {
//    protected:
//        class IObjectWrapperHelper
//        {
//        public:
//            static bool getSmartPtr(const ObjectWrapperPtr & ow, void* ptr, const TypeInfo& type)
//            {
//                return ow->getSmartPtr(ptr, type);
//            }
//
//            static bool setSmartPtr(const ObjectWrapperPtr & ow, const void* ptr, const TypeInfo& type)
//            {
//                return ow->setSmartPtr(ptr, type);
//            }
//        };
//
//	protected:
//
//		IObjectWrapper() : initialized(false) {}
//
//	public:
//		//! Typ inicjalizatora danych - używany do leniwej inicjalizacji
//		typedef boost::function<void(IObjectWrapper &)> LazyInitializer;
//		
//	private:
//		//! Inicjalizator danych
//		LazyInitializer initializer_;
//		//! Czy obiekt był już inicjalizowany
//		mutable bool initialized;
//
//    public:
//        //! Lista typów.
//        typedef TypeInfoList Types;
//
//    public:
//
//		const bool tryGetMeta(const std::string & key, std::string & val) const
//		{
//			auto it = find(key);
//			if(it == end()){
//				return false;
//			}
//			
//			val = it->second;
//			return true;
//		}		
//			
//		void copyMeta(const IObjectWrapper & srcWrapper)
//		{
//			insert(srcWrapper.begin(), srcWrapper.end());
//		}
//
//        //! Return type resolver
//        struct get_t
//        {
//            IObjectWrapper* wrapper;
//            const IObjectWrapper* constWrapper;
//            bool exact;
//
//            template <class Ptr>
//            operator Ptr()
//            {
//                Ptr result;
//                if (wrapper->tryGet(result, exact)) {
//                    return result;
//                } else {
//                    //throw std::bad_cast("Invalid cast");
//                    throw std::bad_cast();
//                }
//            }
//            template <class Ptr>
//            operator Ptr() const
//            {
//                Ptr result;
//                if (constWrapper->tryGet(result, exact)) {
//                    return result;
//                } else {
//                    //throw std::bad_cast("Invalid cast");
//                    throw std::bad_cast();
//                }
//            }
//        };
//
//    public:
//
//        //! \return Nazwa dla zadanego typu.
//        template <class T>
//        static const char* getClassName(T* dummy = nullptr)
//        {
//            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
//            return ObjectWrapperT<T>::className();
//        }
//
//        //! \param type Typ do porównania.
//        //! \return Czy to te same typy?
//        inline bool isTypeEqual(const TypeInfo& type) const
//        {
//            return getTypeInfo() == type;
//        }
//
//    public:
//        //! Pusty polimorficzny destruktor.
//        virtual ~IObjectWrapper() {}
//
//    public:
//
//        //! Próba pobrania obiektu z wrappera.
//        //! \param object Rezultat.
//        //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
//        //! \return Sukces/porażka.
//        template <class Ptr>
//        bool tryGet(Ptr& object, bool exact = false)
//        {
//            TypeInfo type = typeid(Ptr);
//            if ( exact ) {
//                auto types = getPtrTypeInfo();
//                if ( types.first == type || types.second == type ) {
//                    return getSmartPtr(&object, type);
//                } else {
//                    return false;
//                }
//            } else {
//                return getSmartPtr(&object, type);
//            }
//        }
//
//        //! Próba pobrania obiektu z wrappera.
//        //! \param object Rezultat.
//        //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
//        //! \return Sukces/porażka.
//        template <class Ptr>
//        bool tryGet(Ptr& object, bool exact = false) const
//        {
//            TypeInfo type = typeid(Ptr);
//            if ( exact ) {
//                auto types = getPtrTypeInfo();
//                if ( types.second == type ) {
//                    return getSmartPtr(&object, type);
//                } else {
//                    return false;
//                }
//            } else {
//                return getSmartPtr(&object, type);
//            }
//        }
//
//        //! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
//        //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
//        //! \return Return Type Resolver
//        get_t get(bool exact = false)
//        {
//            get_t result = { this, nullptr, exact };
//            return result;
//        }
//
//        //! \param dummy Pozostawić pusty.
//        //! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
//        template <class T>
//        typename ObjectWrapperT<T>::Ptr get(T* dummy = nullptr)
//        {
//            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
//            typedef ObjectWrapperT<T> Wrapper;
//            typedef typename Wrapper::Ptr Ptr;
//            return static_cast<Ptr>(get());
//        }
//
//        //! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
//        //! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
//        //! \return Return Type Resolver
//        const get_t get(bool exact = false) const
//        {
//            const get_t result = { nullptr, this, exact };
//            return result;
//        }
//
//		void set(const LazyInitializer & initializer)
//		{
//			reset();
//			initializer_ = initializer;
//			initialized = false;
//		}
//
//        //! Ustawia obiekt wrappera, jego nazwę i źródło
//        //! \param object Obiekt
//        //! \param name Nazwa obiektu, która ma być wyświetlona
//        //! \param source źródło obiektu - ścieżka do pliku z którego pochodzi
//        template <class Ptr>
//        void set(const Ptr& object, const LazyInitializer & initializer = LazyInitializer())
//        {
//            if ( !trySet(object, initializer)) {
//                throw std::bad_cast();
//            }
//        }
//
//        //! Ustawia obiekt wrappera.
//        //! \param object Obiekt.
//        //! \return Sukces/porażka.
//        template <class Ptr>
//        bool trySet(const Ptr& object, const LazyInitializer & initializer = LazyInitializer())
//        {
//			bool ret = false;
//            TypeInfo type = typeid(Ptr);
//            if ( getPtrTypeInfo().first == type ) {
//                ret = setSmartPtr(&object, type);
//
//				if(ret == true){
//					initializer_ = initializer;
//					initialized = false;
//				}
//            }
//
//			return ret;
//        }
//
//        virtual const void* getRawPtr() const = 0;
//        virtual void* getRawPtr() = 0;
//        virtual void reset() = 0;
//		void initialize() const
//		{
//			if(getRawPtr() == nullptr && initialized == false && initializer_.empty() == false){
//				initializer_(*const_cast<IObjectWrapper*>(this));
//				initialized = true;
//			}
//		}
//
//		const LazyInitializer & initializer() const
//		{
//			return initializer_;
//		}
//
//
//        //! \return ID typu.
//        virtual std::size_t getClassID() const = 0;
//        //! \return Nazwa typu.
//        virtual const std::string& getClassName() const = 0;
//
//        //! \param type
//        //! \return Czy obiekt wspiera określony typ?
//        virtual bool isSupported(const TypeInfo& type) const = 0;
//        //! \param type Typ inteligentnego wskaźnika.
//        //! \return true jeżeli do z obiektu można wyłuskać dany wskaźnik.
//        virtual bool isPtrSupported(const TypeInfo& type) const = 0;
//
//        //! \return Informacje o typie.
//        virtual TypeInfo getTypeInfo() const = 0;
//
//        //! \return Informacje o typie odpowiednio normalnego i stałego wskaźnika.
//        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const = 0;
//
//        //! \param supported Lista wspieranych rozszerzeń.
//        virtual void getSupportedTypes(Types& supported) const = 0;
//
//        //! \return Czy wrappowany obiekt jest wyzerowany?
//        virtual bool isNull() const
//		{
//			if(initializer_.empty() == false && initialized == false){
//				return false;
//			}
//
//			return getRawPtr() == nullptr;
//		}
//
//        //! \return Klon bieżącego obiektu. Wewnętrzny wskaźnik również jest kopiowany.
//        virtual ObjectWrapperPtr clone() const = 0;
//
//        virtual ObjectWrapperPtr create() const = 0;
//
//    protected:
//
//        //! \return Czy udało się ustawić mądry wskaźnik?
//        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const = 0;
//
//        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type) = 0;
//    };
//
//    //! Baza dla typu wrapującego jakiś obiekt. Poza trzymaniem metadanych klasy pochodne
//    //! trzymają referencje do obiektów.
//    class ObjectWrapper : public IObjectWrapper
//    {
//    protected:
//        //! Nazwa typu.
//        std::string className;
//        //! Hash identyfikujący typ.
//        std::size_t classID;
//
//    public:
//        //! \param object
//        //! \param name
//        //! \return Wrapper obiektu.
//        template <class T>
//        static ObjectWrapperPtr create(T* dummy = nullptr)
//        {
//            UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
//            typedef ObjectWrapperT<T> Wrapper;
//            Wrapper * wrapper = new Wrapper();
//            return ObjectWrapperPtr(wrapper);
//        }
//
//    protected:
//        //! \param className
//        //! \param classId
//        ObjectWrapper(const char* className, const TypeInfo& type) :
//             className(className), classID(0)
//             {}
//
//    public:
//        //! Pusty polimorficzny destruktor.
//        virtual ~ObjectWrapper()
//        {
//
//        }
//
//    public:
//
//        //! \return ID typu.
//        virtual std::size_t getClassID() const
//        {
//            return classID;
//        }
//        //! \return Nazwa typu.
//        virtual const std::string& getClassName() const
//        {
//            return className;
//        }
//    };
//
//    //! Deklaracja typu. Trzeba go specjalizować za pomocą makr. Ta wersja będzie
//    //! rzucać statyczną asercją.
//    template <class T>
//    class ObjectWrapperT : public ObjectWrapper
//    {
//    public:
//        typedef T* Ptr;
//        typedef const T* ConstPtr;
//        Ptr get()
//        {
//            // rev co z tymi statycznymi asercjami?
//            //UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
//            return nullptr;
//        }
//        void set(Ptr)
//        {
//            // rev co z tymi statycznymi asercjami?
//            //UTILS_STATIC_ASSERT(false, "Nie zdefiniowano wrappera albo nie zaincludowano odpowiedniego nagłówka. Poszukaj wystapienia CORE_DEFINE_WRAPPER.");
//        }
//    };
//
//    template <class T>
//    struct ObjectWrapperTraits
//    {
//        static const bool isDefinitionVisible = false;
//    };
//
//    //! Pomocniczy typ bazowy, zarządzający obiektem za pomocą parametru
//    //! PtrPolicy. Tego typu nigdy nie używa się wprost.
//    template <class T, class PtrPolicyT, class ClonePolicyT>
//    class __ObjectWrapperT : public ObjectWrapper, protected PtrPolicyT, protected ClonePolicyT
//    {
//    public:
//        //!
//        typedef PtrPolicyT PtrPolicy;
//        //!
//        typedef PtrPolicyT Base;
//        //!
//        typedef ClonePolicyT ClonePolicy;
//
//        //! Wrappowany obiekt.
//        typedef typename Base::template Ptr<T>::Type Ptr;
//        //! Stały wrappowany obiekt
//        typedef typename Base::template Ptr<const T>::Type ConstPtr;
//
//    protected:
//        //! Wrappowany obiekt.
//        Ptr wrapped;
//
//    protected:
//        //! \param wrapped
//        __ObjectWrapperT(const char* className)
//            : ObjectWrapper(className, typeid(T))
//        {
//            PtrPolicyT::initPtr(wrapped);
//        }
//
//    public:
//        //!
//        virtual ~__ObjectWrapperT()
//        {
//            //PtrPolicy::setPtr<T>(wrapped, nullptr);
//            reset();
//        }
//
//    public:
//        //! \return Informacja o typie.
//        virtual TypeInfo getTypeInfo() const
//        {
//            return typeid(T);
//        }
//
//        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const
//        {
//            return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));
//        }
//
//        virtual ObjectWrapperPtr clone() const {
//            std::auto_ptr<ObjectWrapperT<T>> cloned(new ObjectWrapperT<T>(ObjectWrapperT<T>::className()));
//            // rev czy na pewno nie trzeba <T> przy clone?
//			initialize();
//            T* newPtr = ClonePolicy::clone(&*wrapped);
//            Ptr temp;
//            PtrPolicy::setPtr(temp, newPtr);
//            cloned->wrapped = temp;
//            return ObjectWrapperPtr(cloned.release());
//        }
//
//        virtual ObjectWrapperPtr create() const {
//            return ObjectWrapperPtr(new __ObjectWrapperT(this->getClassName().c_str()));
//        }
//
//        virtual const void* getRawPtr() const
//        {
//            return PtrPolicy::getRawPtr(wrapped);
//        }
//
//        virtual void* getRawPtr()
//        {
//            return PtrPolicy::getRawPtr(wrapped);
//        }
//
//        virtual void reset()
//        {
//            if(wrapped != nullptr){
//                // rev usunięte <T>
//                PtrPolicy::setPtr(wrapped, static_cast<T*>(nullptr));
//            }
//        }
//
//        //! \param type
//        //! \return Czy obiekt wspiera określony typ?
//        virtual bool isSupported(const TypeInfo& type) const
//        {
//            return type == typeid(T);
//        }
//        //! \param supported
//        virtual void getSupportedTypes(ObjectWrapper::Types& supported) const
//        {
//            supported.push_back(typeid(T));
//        }
//
//        virtual bool isPtrSupported(const TypeInfo& type) const
//        {
//            return type == typeid(Ptr) || type == typeid(ConstPtr);
//        }
//
//    protected:
//
//        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const
//        {
//            if ( type == typeid(Ptr) ) {
//				initialize();
//                PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), wrapped);
//                return true;
//            } else if ( type == typeid(ConstPtr) ) {
//				initialize();
//                PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(wrapped));
//                return true;
//            }
//            return false;
//        }
//
//        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type)
//        {
//            if ( type == typeid(Ptr) ) {
//                PtrPolicy::setPtr(wrapped, *reinterpret_cast<const Ptr*>(ptr));
//                return true;
//            } else {
//                return false;
//            }
//        }
//    };
//
//	typedef std::set<ObjectWrapperPtr> Objects;
//	typedef std::set<ObjectWrapperConstPtr> ConstObjects;
//	typedef std::list<ObjectWrapperPtr> ObjectsList;
//	typedef std::list<ObjectWrapperConstPtr> ConstObjectsList;
//
//////////////////////////////////////////////////////////////////////////////////
//} // namespace core
//////////////////////////////////////////////////////////////////////////////////
//
//#define __DEFINE_META(type)                                                          \
//template <> struct ObjectWrapperTraits<type>                                                      \
//{                                                                                                 \
//    static const bool isDefinitionVisible = true;                                                 \
//};
//
//// Makro tworzące specjalizację ObjectWrapperT. Musi występować w globalnym
//// namespace. Drugim parametrem może być dowolny typ mający cechy ptrPolicy,
//// wbudowane: PtrPolicyRaw, PtrPolicyBoost, PtrPolicyOSG
//#define DEFINE_WRAPPER(type, ptrPolicy, clonePolicy)                                         \
//namespace utils {                                                                                  \
//    template <>                                                                                   \
//class ObjectWrapperT<type> : public __ObjectWrapperT<type,  ptrPolicy, clonePolicy>               \
//{                                                                                                 \
//    friend class __ObjectWrapperT<type,  ptrPolicy, clonePolicy>;                                    \
//    friend class ObjectWrapper;                                                                         \
//public:                                                                                           \
//    typedef ObjectWrapperT<type> ObjectWrapperType;                                                                         \
//    typedef __ObjectWrapperT<type,  ptrPolicy, clonePolicy> __Base;\
//    typedef __Base::PtrPolicy PtrPolicy;\
//    typedef  __Base::ClonePolicy ClonePolicy;\
//    static const char* className() { return #type; }                                              \
//protected:                                                                                         \
//    ObjectWrapperT(const char* className = #type) :                                               \
//    __ObjectWrapperT<type,ptrPolicy, clonePolicy >(className)                                     \
//      {}                                                                                          \
//};                                                                                                \
//__DEFINE_META(type)                                                                  \
//}
//
//#define DEFINE_WRAPPER_INHERITANCE(typeT, baseTypeT)\
//namespace utils {\
//    template <>\
//class ObjectWrapperT<typeT> : public ObjectWrapperT<baseTypeT>\
//{\
//    friend class ObjectWrapper;\
//public:\
//    typedef typeT type;\
//    typedef baseTypeT baseType;\
//    typedef ObjectWrapperT<type> ObjectWrapperType;\
//    static const char* className() { return #typeT; }\
//    public:\
//        typedef ObjectWrapperT<baseType> Base;\
//        typedef Base::PtrPolicy PtrPolicy;\
//        typedef PtrPolicy::Ptr<type>::Type Ptr;\
//        typedef PtrPolicy::Ptr<const type>::Type ConstPtr;\
//    protected:\
//        ObjectWrapperT(const char* className = #typeT) :\
//        Base(className)\
//        {}\
//    public:\
//        virtual TypeInfo getTypeInfo() const\
//        {\
//            return typeid(type);\
//        }\
//        virtual ObjectWrapperPtr create() const {\
//            return ObjectWrapperPtr(new ObjectWrapperT<type>(ObjectWrapperT<type>::className()));\
//        }\
//        virtual bool isSupported(const TypeInfo& type) const\
//        {\
//            if ( type == typeid(type) ) {\
//                return true;\
//            } else {\
//                return Base::isSupported(type);\
//            }\
//        }\
//        virtual void getSupportedTypes(ObjectWrapper::Types& supported) const\
//        {\
//            Base::getSupportedTypes(supported);\
//            supported.push_back(typeid(type));\
//        }\
//        virtual std::pair<TypeInfo, TypeInfo> getPtrTypeInfo() const\
//        {\
//            return std::pair<TypeInfo, TypeInfo>(typeid(Ptr), typeid(ConstPtr));\
//        }\
//    protected:\
//        virtual bool getSmartPtr(void* ptr, const TypeInfo& type) const\
//        {\
//            if ( type == typeid(Ptr) ) {\
//				initialize(); \
//                PtrPolicy::setPtr(*reinterpret_cast<Ptr*>(ptr), boost::static_pointer_cast<typeT>(this->wrapped));\
//                return true;\
//            } else if ( type == typeid(ConstPtr) ) {\
//				initialize(); \
//                PtrPolicy::setPtr(*reinterpret_cast<ConstPtr*>(ptr), ConstPtr(boost::static_pointer_cast<typeT>(this->wrapped)));\
//                return true;\
//            }\
//            return Base::getSmartPtr(ptr, type);\
//        }\
//        virtual bool setSmartPtr(const void* ptr, const TypeInfo& type)\
//        {\
//            if ( type == typeid(Ptr) ) {\
//                Ptr temp;\
//                PtrPolicy::setPtr(temp, *reinterpret_cast<const Ptr*>(ptr));\
//                this->wrapped = (Ptr)(temp);\
//                return true;\
//            } else {\
//                return Base::setSmartPtr(ptr, type);\
//            }\
//        }\
//    public:\
//        virtual bool isPtrSupported(const TypeInfo& type) const\
//        {\
//            return type == typeid(Ptr) || type == typeid(ConstPtr) || Base::isPtrSupported(type);\
//        }\
//    };\
//}

#endif  // __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__