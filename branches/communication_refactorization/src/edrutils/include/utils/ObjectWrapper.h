/********************************************************************
	created:  2014/04/29	14:32:28
	filename: ObjectWrapper.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__
#define __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__

#include <set>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <boost/type_traits.hpp>
#include <utils/ObjectWrapperTraits.h>
#include <utils/PtrPolicyBoost.h>
#include <utils/ClonePolicies.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
	////////////////////////////////////////////////////////////////////////////////
	class ObjectWrapper;
	typedef utils::shared_ptr<ObjectWrapper> ObjectWrapperPtr;
	typedef utils::shared_ptr<const ObjectWrapper> ObjectWrapperConstPtr;
	typedef utils::weak_ptr<ObjectWrapper> ObjectWrapperWeakPtr;
	typedef utils::weak_ptr<const ObjectWrapper> ObjectWrapperConstWeakPtr;

	typedef std::set<ObjectWrapperPtr> Objects;
	typedef std::set<ObjectWrapperConstPtr> ConstObjects;

	typedef std::list<ObjectWrapperPtr> ObjectsList;
	typedef std::list<ObjectWrapperConstPtr> ConstObjectsList;

	typedef std::vector<ObjectWrapperPtr> ObjectsVector;
	typedef std::vector<ObjectWrapperConstPtr> ConstObjectsVector;

	//! Forward declaration
	template<typename T>
	class ObjectWrapperT;

	//! Baza dla typu wrapującego jakiś obiekt. Poza trzymaniem metadanych klasy pochodne
	//! trzymają referencje do obiektów.
	class ObjectWrapper
	{
	public:

		//! Para opisująca typ
		typedef std::pair<TypeInfo, TypeInfo> TypeInfoPair;

		//! Typ klonowania
		enum CloneOp {
			DeepClone,		//! Pełne, głębokie
			ShallowClone	//! Płytkie
		};

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
				}
				else {
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
				}
				else {
					//throw std::bad_cast("Invalid cast");
					throw std::bad_cast();
				}
			}
		};

	public:

		//! \tparam Ptr Typ wskaznika o który pytamy
		template <class Ptr>
		operator const Ptr()
		{
			Ptr result;
			tryGet(result, false);
			return result;
		}

		//! \tparam Ptr Typ wskaznika o który pytamy
		template <class Ptr>
		operator const Ptr() const
		{
			Ptr result;
			tryGet(result, false);
			return result;
		}

		//! \tparam T Typ obiektu dla ktorego chcemy utworzych OW
		//! \param dummy Parametr nie powinien być nigdy uzywany
		//! \return Wrapper obiektu.
		template <class T>
		static const ObjectWrapperPtr create(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
			return ObjectWrapperPtr(new ObjectWrapperT<T>());
		}

		//! \tparam T Typ obiektu ktorego nazwe OW chcemy pobrac
		//! \return Nazwa dla zadanego typu.
		template <class T>
		static const std::string getClassName(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			return ObjectWrapperT<T>::className();
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool tryGet(Ptr& object, bool exact = false)
		{
			return __tryGetRawPtr(object, exact);
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
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
		//! \tparam T Typ obiektu ktory chemy pobrac z OW
		//! \param dummy Pozostawić pusty.
		//! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
		template <typename T>
		const typename ObjectWrapperT<T>::Ptr get(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			return static_cast<typename ObjectWrapperT<T>::Ptr>(get());
		}

		//! \tparam T Typ obiektu ktory chemy pobrac z OW
		//! \param dummy Pozostawić pusty.
		//! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
		template <typename T>
		const typename ObjectWrapperT<T>::ConstPtr get(T* dummy = nullptr) const
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			typename ObjectWrapperT<T>::ConstPtr ret = get();
			return ret;
		}

		//! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Return Type Resolver
		const get_t get(bool exact = false) const;
		//! Ustawia obiekt wrappera. W razie błędu rzuca bad_castem.
		//! \tparam Ptr Typ wskaznika ktory chcemy przypisac do OW
		//! \param object Obiekt.
		template <class Ptr>
		void set(const Ptr& object)
		{
			if (!trySet(object)) {
				throw std::bad_cast();
			}
		}

		//! Ustawia obiekt wrappera.
		//! \tparam Ptr Typ wskaznika ktory chcemy przypisac do OW
		//! \param object Obiekt.
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool trySet(const Ptr& object)
		{
			return __trySetRawPtr(object, boost::is_pointer<Ptr>());
		}

		//! Pusty polimorficzny destruktor.
		virtual ~ObjectWrapper();
		//! \return Ilość referencji do naszych danych
		virtual const int getReferenceCount() const = 0;
		//! \return Nazwa typu.
		virtual const std::string getClassName() const = 0;
		//! \return Informacje o typie.
		virtual const TypeInfo getTypeInfo() const = 0;
		//! \param type
		//! \return Czy obiekt wspiera określony typ?
		virtual const bool isSupported(const TypeInfo& type) const;
		//! \param supported Lista wspieranych rozszerzeń.
		virtual void getSupportedTypes(Types& supported) const = 0;
		//! \param co Typ klonowania
		//! \return Klon bieżącego obiektu. Wewnętrzny wskaźnik również jest kopiowany.
		const ObjectWrapperPtr clone(const CloneOp co = DeepClone) const;
		//! \param dest [out] Obiekt docelowy
		//! \param co Typ klonowania
		virtual void clone(ObjectWrapper & dest, const CloneOp co = DeepClone) const;
		//! \param dest [out] Obiekt docelowy
		//! \param co Typ klonowania
		//! \return Czy udało się sklonować obiekt
		const bool tryClone(ObjectWrapper & dest, const CloneOp co = DeepClone) const;
		//! \return Pusty OW dla danego typu
		virtual const ObjectWrapperPtr create() const = 0;
		//! \return Surowa wartość wskaźnika danych przechowywanych w OW
		const void* getRawPtr() const;
		//! \return Surowa wartość wskaźnika danych przechowywanych w OW
		void* getRawPtr();
		//! \return Informacje o typie odpowiednio normalnego i stałego wskaźnika.
		virtual const TypeInfoPair getPtrTypeInfo() const = 0;
		//! \param ptrInfo Informacja o wskaźniku
		//! \return Prawda jeśli wskaźnik można pobrać z danego OW
		virtual const bool isPtrSupported(const TypeInfo & ptrInfo) const = 0;
		//! Zeruje obiekt
		virtual void reset();
		//! Zamienia zawartosc OW jesli to możliwe
		//! \param ow Obiekt z którym zamienimy przechowywane wartości
		virtual void swap(ObjectWrapper & ow);
		//! \param obj Obiekt z którym się porównujemy
		//! \return Czy obiekty są takie same - trzymają te same dane
		virtual const bool isEqual(const ObjectWrapper & obj) const;

	protected:

		//! \param className
		//! \param classId
		ObjectWrapper();
		//! Konstruktor kopiujący
		//! \param wrapper Obiekt ktory kopiujemy
		ObjectWrapper(const ObjectWrapper & wrapper);

	private:
		//! Ustawia obiekt wrappera.
		//! \tparam Ptr Typ wskaznika ktory chcemy przypisac do OW
		//! \param object Obiekt.
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __trySetRawPtr(const Ptr& object, boost::true_type)
		{
			UTILS_STATIC_ASSERT((!boost::is_const<typename boost::remove_pointer<Ptr>::type>::value), "Nalezy zapisywac dane bez modyfikatora const");
			if (getPtrTypeInfo().first == typeid(Ptr)){
				__setData(&object);
				return true;
			}
			return false;
		}

		//! Ustawia obiekt wrappera.
		//! \tparam Ptr Typ wskaznika ktory chcemy przypisac do OW
		//! \param object Obiekt.
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __trySetRawPtr(const Ptr& object, boost::false_type)
		{
			UTILS_STATIC_ASSERT((!boost::is_const<typename Ptr::element_type>::value), "Nalezy zapisywac dane bez modyfikatora const");
			if (getPtrTypeInfo().first == typeid(Ptr)){
				__setData(&object);
				return true;
			}
			return false;
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Typ wskaznika ktory chcemy pobrać z OW
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __tryGetRawPtr(Ptr& object, bool exact)
		{
			TypeInfo ptrInfo(typeid(Ptr));
			auto p = getPtrTypeInfo();
			if (ptrInfo == p.first || ptrInfo == p.second) {
				return __tryUnpackData(&object, ptrInfo);
			}
			else if (exact == false && isPtrSupported(ptrInfo) == true){
				return __tryUnpackBaseData(&object, ptrInfo);
			}
			else {
				return false;
			}
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Typ wskaznika ktory chcemy pobrać z OW
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __tryGetRawPtr(Ptr& object, bool exact, boost::true_type) const
		{
			UTILS_STATIC_ASSERT((boost::is_const<typename boost::remove_pointer<Ptr>::type>::value), "Ta metoda mozna pobierac tylko obiekty typu const");

			TypeInfo ptrInfo(typeid(Ptr));
			if (ptrInfo == getPtrTypeInfo().second) {
				return __tryUnpackData(&object, ptrInfo);
			}
			else if (exact == false && isPtrSupported(ptrInfo) == true){
				return __tryUnpackBaseData(&object, ptrInfo);
			}
			else {
				return false;
			}
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Typ wskaznika ktory chcemy pobrać z OW
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		const bool __tryGetRawPtr(Ptr& object, bool exact, boost::false_type) const
		{
			UTILS_STATIC_ASSERT((boost::is_const<typename Ptr::element_type>::value), "Ta metoda mozna pobierac tylko obiekty typu const");

			TypeInfo ptrInfo(typeid(Ptr));
			if (ptrInfo == getPtrTypeInfo().second) {
				return __tryUnpackData(&object, ptrInfo);
			}
			else if (exact == false && isPtrSupported(ptrInfo) == true){
				return __tryUnpackBaseData(&object, ptrInfo);
			}
			else {
				return false;
			}
		}

		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual const bool __tryUnpackData(void * object, const TypeInfo & ptrType);
		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual const bool __tryUnpackData(void * object, const TypeInfo & ptrType) const;
		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual const bool __tryUnpackBaseData(void * object, const TypeInfo & ptrType);
		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual const bool __tryUnpackBaseData(void * object, const TypeInfo & ptrType) const;
		//! \return Surowy wskaźnik do przechowywanego obiektu
		virtual const void* __getRawPtr() const = 0;
		//! \return Surowy wskaźnik do przechowywanego obiektu
		virtual void* __getRawPtr() = 0;
		//! \param dest Obiekt docelowy do którego kopiujemy dane
		virtual void __clone(ObjectWrapper & dest, const CloneOp co) const = 0;
		//! Ważne!! Swap może rzucać wyjątkiem!!
		//! \param ow Object wrapper z ktorym zamieniamy zawartość
		virtual void __swap(ObjectWrapper & ow) = 0;
		//! Zeruje dane OW
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
		//! \param obj Obiekt z którym się porównujemy
		//! \return Czy obiekty są takie same - trzymają te same dane
		virtual const bool __isEqual(const ObjectWrapper & obj) const = 0;
	};

	//! Pomocniczy typ bazowy, zarządzający obiektem za pomocą parametru
	//! PtrPolicy. Tego typu nigdy nie używa się wprost.
	template <typename T>
	class __ObjectWrapperT : public ObjectWrapper
	{
	public:
		//! Typ wrapowanych danych
		typedef T Type;
		//! Pełny typ aktualnego wrappera
		typedef __ObjectWrapperT<Type> ImplType;

	private:
		//! Wrappowany obiekt.
		typename ObjectWrapperTraits<Type>::Ptr wrapped_;

	protected:

		__ObjectWrapperT() : ObjectWrapper()
		{
			ObjectWrapperTraits<Type>::PtrPolicy::initPtr(wrapped_);
		}

		__ObjectWrapperT(const __ObjectWrapperT & wrapper)
			: ObjectWrapper(wrapper), wrapped_(wrapper.wrapped_)
		{
		}

	private:

		//! \param dest ObjectWrapper do którego chcemy kopiować dane
		//! \return Wskaźnik na ObjectWrapper mojego typu lub rzuca wyjątkiem
		static ImplType * __getMe(ObjectWrapper & dest)
		{
			auto cloned = dynamic_cast<ImplType*>(&dest);

			if (cloned == nullptr){
				throw std::runtime_error("Clone destination wrapper type mismatch");
			}

			return cloned;
		}

		//! \param cloned Wskaźnik na ObjectWrapper mojego typu
		//! \param co Typ klonowania
		void __cloneImpl(ImplType * cloned, const ObjectWrapper::CloneOp co) const
		{
			if (co == ObjectWrapper::ShallowClone){
				cloned->wrapped_ = wrapped_;
			}
			else{
				ObjectWrapperTraits<Type>::Ptr newPtr(ObjectWrapperTraits<Type>::ClonePolicy::clone(&*wrapped_));
				cloned->wrapped_ = newPtr;
			}
		}

		virtual void __setData(const void * object)
		{
			ObjectWrapperTraits<Type>::PtrPolicy::setPtr(wrapped_, *(static_cast<const ObjectWrapperTraits<Type>::Ptr *>(object)));
		}

		virtual const bool __tryGetData(void * object, const TypeInfo & ptrType)
		{
			return ObjectWrapperTraits<Type>::tryGetData(object, ptrType, wrapped_);
		}

		virtual const bool __tryGetData(void * object, const TypeInfo & ptrType) const
		{
			return ObjectWrapperTraits<Type>::tryGetConstData(object, ptrType, wrapped_);
		}

		virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType)
		{
			return ObjectWrapperTraits<Type>::tryGetBaseData(object, ptrType, wrapped_);
		}

		virtual const bool __tryGetBaseData(void * object, const TypeInfo & ptrType) const
		{
			return ObjectWrapperTraits<Type>::tryGetConstBaseData(object, ptrType, wrapped_);
		}

		virtual const bool __isEqual(const ObjectWrapper & obj) const
		{
			ObjectWrapperTraits<Type>::ConstPtr w;
			bool res = obj.tryGet(w);

			return (res == true) && (wrapped_ == w);
		}

		virtual void __clone(ObjectWrapper & dest, const ObjectWrapper::CloneOp co) const {
			auto me = __getMe(dest);
			__cloneImpl(me, co);
		}

		virtual const void* __getRawPtr() const
		{
			return ObjectWrapperTraits<Type>::PtrPolicy::getConstRawPtr(wrapped_);
		}

		virtual void* __getRawPtr()
		{
			return ObjectWrapperTraits<Type>::PtrPolicy::getRawPtr(wrapped_);
		}

		virtual void __swap(ObjectWrapper & ow)
		{
			auto me = __getMe(ow);
			ObjectWrapperTraits<Type>::PtrPolicy::swapPtr(wrapped_, me->wrapped_);
		}

		virtual void __reset()
		{
			ObjectWrapperTraits<Type>::PtrPolicy::setPtr(wrapped_, static_cast<T*>(nullptr));
		}

	public:
		//!
		virtual ~__ObjectWrapperT()
		{
		}

		virtual const int getReferenceCount() const
		{
			return ObjectWrapperTraits<Type>::PtrPolicy::referenceCount(wrapped_);
		}

		virtual const bool isPtrSupported(const TypeInfo & ptrInfo) const
		{
			return ObjectWrapperTraits<Type>::ptrTypeSupported(ptrInfo);
		}

		virtual const ObjectWrapper::TypeInfoPair getPtrTypeInfo() const
		{
			return ObjectWrapperTraits<Type>::ptrTypeInfo();
		}

		//! \return Nazwa typu.
		virtual const std::string getClassName() const
		{
			return ObjectWrapperT<Type>::className();
		}

		//! \return Informacje o typie.
		virtual const TypeInfo getTypeInfo() const
		{
			return ObjectWrapperTraits<Type>::typeInfo();
		}

		virtual void getSupportedTypes(Types& supported) const
		{
			ObjectWrapperTraits<Type>::supportedTypes(supported);
		}

		virtual const ObjectWrapperPtr create() const {
			return ObjectWrapper::create<T>();
		}
	};

	//! Deklaracja typu. Trzeba go specjalizować za pomocą makr. Ta wersja będzie
	//! rzucać statyczną asercją.
	template <typename T>
	class ObjectWrapperT : public ObjectWrapper
	{
		UTILS_STATIC_ASSERT(sizeof(T) == 0, "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapperów");
	};

	////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Operatory porównania dla OW
////////////////////////////////////////////////////////////////////////////////

//! Operator poównania realizowany w oparciu o metodę isEqual
bool operator==(const utils::ObjectWrapper & a, const utils::ObjectWrapper & b);

//! Operator różności realizowany w oparciu o operator porównania
bool operator!=(const utils::ObjectWrapper & a, const utils::ObjectWrapper & b);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Makra dla definiowaia wrapperów
////////////////////////////////////////////////////////////////////////////////

#define __DEFINE_WRAPPER_CLASS(typeT)\
	template <> class ObjectWrapperT<typeT> : public __ObjectWrapperT<typeT>\
{\
	friend class ImplType;\
	friend class ObjectWrapper;\
private:\
	ObjectWrapperT() : ImplType() {}\
public:\
	virtual ~ObjectWrapperT() {}\
	static const std::string className(){\
	return #typeT;\
}\
};

// Makro tworzące specjalizację ObjectWrapperT. Musi występować w globalnym
// namespace. Drugim parametrem może być dowolny typ mający cechy ptrPolicy,
// wbudowane: PtrPolicyRaw, PtrPolicyBoost, PtrPolicyOSG
#define DEFINE_WRAPPER(typeT, ptrPolicyT, clonePolicyT)\
namespace utils {\
	__DEFINE_WRAPPER_META(typeT, ptrPolicyT, clonePolicyT)\
	__DEFINE_WRAPPER_CLASS(typeT)\
}

// Tutaj PtrPolicy i ClonePolicy są dziedziczone po typie bazowym,
// dla któego wcześniej musiał być zdefiniowany wrapper
#define DEFINE_WRAPPER_INHERITANCE(typeT, baseTypeT)\
namespace utils {\
	__DEFINE_WRAPPER_DERIVED_META(typeT, baseTypeT)\
	__DEFINE_WRAPPER_CLASS(typeT)\
}

#endif  // __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__