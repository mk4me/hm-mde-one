/********************************************************************
	created:  2014/08/06	18:30:28
	filename: ObjectWrapperT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/

#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__
	#error This is implementation specific private file. This header should newer be included explicitely. Instead You should include ObjectWrapper.h
#endif

#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPERT_H__
#define __HEADER_GUARD_UTILS__OBJECTWRAPPERT_H__

namespace utils
{
	//! Pomocniczy typ bazowy, zarz�dzaj�cy obiektem za pomoc� parametru
	//! PtrPolicy. Tego typu nigdy nie u�ywa si� wprost.
	template <typename T>
	class __ObjectWrapperT : public ObjectWrapper
	{
	public:
		//! Typ wrapowanych danych
		typedef T Type;
		//! Pe�ny typ aktualnego wrappera
		typedef __ObjectWrapperT<Type> ImplType;

	private:
		//! Wrappowany obiekt.
		typename ObjectWrapperTraits<Type>::Ptr wrapped_;	

	protected:

		//! Konstruktor
		//! \param data Dane		
		__ObjectWrapperT(typename ObjectWrapperTraits<Type>::Ptr data)
			: wrapped_(data)
		{
		}
		
		//! Domyslny konstruktor
		__ObjectWrapperT()
		{
			ObjectWrapperTraits<Type>::PtrPolicy::initPtr(wrapped_);
		}

	private:

		//! \param dest ObjectWrapper do kt�rego chcemy kopiowa� dane
		//! \return Wska�nik na ObjectWrapper mojego typu lub rzuca wyj�tkiem
		static ImplType * __getMe(ObjectWrapper & dest)
		{
			auto cloned = dynamic_cast<ImplType*>(&dest);

			if (cloned == nullptr){
				throw std::runtime_error("Clone destination wrapper type mismatch");
			}

			return cloned;
		}

		//! \param cloned Wska�nik na ObjectWrapper mojego typu
		//! \param co Typ klonowania
		void __cloneImpl(ImplType * cloned, const ObjectWrapper::CloneOp co) const
		{
			if (co == ObjectWrapper::ShallowClone){
				cloned->wrapped_ = wrapped_;
			}
			else{
				typename ObjectWrapperTraits<Type>::Ptr newPtr(ObjectWrapperTraits<Type>::ClonePolicy::clone(&*wrapped_));
				cloned->wrapped_ = newPtr;
			}
		}

		virtual void __setData(const void * object)
		{
			ObjectWrapperTraits<Type>::PtrPolicy::setPtr(wrapped_, *(static_cast<const typename ObjectWrapperTraits<Type>::Ptr *>(object)));
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
			typename ObjectWrapperTraits<Type>::ConstPtr w;
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
		//! Destruktor wirtualny
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

		virtual const bool isSupported(const TypeInfo& type) const
		{
			return ObjectWrapperTraits<Type>::typeSupported(type);
		}

		virtual const ObjectWrapperPtr create() const {
			return ObjectWrapper::create<T>();
		}	
	};

	//! Deklaracja typu. Trzeba go specjalizowa� za pomoc� makr. Ta wersja b�dzie
	//! rzuca� statyczn� asercj�.
	template <typename T>
	class ObjectWrapperT : public ObjectWrapper
	{
		static_assert(sizeof(T) == 0, "Nalezy uzywac makr DEFINE_WRAPPER lub DEFINE_WRAPPER_INHERITANCE dla definiowania nowych wrapper�w");	
	};
}

#define __DEFINE_WRAPPER_CLASS(typeT)\
template <> class ObjectWrapperT<typeT> : public __ObjectWrapperT<typeT>{\
private:\
	ObjectWrapperT() {} \
	ObjectWrapperT(ObjectWrapperTraits<typeT>::Ptr data) : __ObjectWrapperT<typeT>(data) {}\
public:\
	virtual ~ObjectWrapperT() {}\
	static const std::string className(){\
		return #typeT;\
	}\
	static ObjectWrapperPtr create(ObjectWrapperTraits<typeT>::Ptr data = ObjectWrapperTraits<typeT>::Ptr()){\
		return ObjectWrapperPtr(data ? new ObjectWrapperT : new ObjectWrapperT(data));\
	}\
	static ObjectWrapperPtr wrapp(ObjectWrapperTraits<typeT>::Ptr data){\
		return ObjectWrapperPtr(new ObjectWrapperT(data)); \
	}\
};

#endif	// __HEADER_GUARD_UTILS__OBJECTWRAPPERT_H__
