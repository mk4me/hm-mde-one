/********************************************************************
created:  2015/08/04	17:09:29
filename: Wrappers.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__WRAPPERS_H__
#define __HEADER_GUARD_DATACHANNEL__WRAPPERS_H__

#include <datachannellib/Accessors.h>
#include <datachannellib/WrapperHelpers.h>
#include <datachannellib/Traits.h>
#include <utils/Debug.h>

namespace dataaccessor
{
	//struct DataWrapper
	//{
	//! ---------------------- W³asny ekstraktor ca³ych próbek -------------
	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	//! \param data Dane do opakowania
	//! \param se Obiekt wyci¹gaj¹cy próbki z danych
	//! \return Lekki interfejs do danych
	static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
		lightWrap(const Container & data, const SampleExtractor & se = SampleExtractor())
	{
		return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteAccessor<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), se));
	}

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	//! \param data Dane do opakowania
	//! \param se Obiekt wyci¹gaj¹cy próbki z danych
	//! \return Lekki interfejs do danych
	static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
		lightWrap(const Container & data, SampleExtractor && se)
	{
		return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteAccessor<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), std::move(se)));
	}

	//TODO
	//dodaæ pozosta³e wariacje kopiowania i przenoszenia
	//! ---------------------- W³asne ekstraktory wartoœci i argumentów
	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	//! \param data Dane do opakowania
	//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
	//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
	//! \return Lekki interfejs do danych
	static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
		lightWrap(const Container & data, const ValueExtractor & ve = ValueExtractor(),
			const ArgumentExtractor & ae = ArgumentExtractor())
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return lightWrap<Container, CSE>(data, CSE(ve, se));
	}

	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	//! \param data Dane do opakowania
	//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
	//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
	//! \return Lekki interfejs do danych
	static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
		lightWrap(const Container & data, ValueExtractor && ve, ArgumentExtractor && ae)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return lightWrap<Container, CSE>(data, CSE(std::move(ve), std::move(se)));
	}

	//---------------------------------------------------------------------

	//! ---------------------- W³asny ekstraktor ca³ych próbek -------------
	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	//! \param data Dane do opakowania
	//! \param se Obiekt wyci¹gaj¹cy próbki z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
		wrap(const Container & data, const SampleExtractor & se = SampleExtractor())
	{
		return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteData<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), se));
	}

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	//! \param data Dane do opakowania
	//! \param se Obiekt wyci¹gaj¹cy próbki z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
		wrap(const Container & data, SampleExtractor && se)
	{
		return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteData<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), std::move(se)));
	}

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	//! \param data Dane do opakowania
	//! \param se Obiekt wyci¹gaj¹cy próbki z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
		wrap(Container && data, const SampleExtractor & se = SampleExtractor())
	{
		const auto size = ContainersSizeExtractor::size(data);
		return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteData<Container, SampleExtractor>(std::move(data), size, se));
	}

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	//! \param data Dane do opakowania
	//! \param se Obiekt wyci¹gaj¹cy próbki z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
		wrap(Container && data, SampleExtractor && se)
	{
		const auto size = ContainersSizeExtractor::size(data);
		return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteData<Container, SampleExtractor>(std::move(data), size, std::move(se)));
	}

	//! ---------------------- W³asne ekstraktory wartoœci i argumentów
	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	//! \param data Dane do opakowania
	//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
	//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
		wrap(const Container & data, const ValueExtractor & ve = ValueExtractor(),
			const ArgumentExtractor & ae = ArgumentExtractor())
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(data, CSE(ve, ae));
	}

	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	//! \param data Dane do opakowania
	//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
	//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
		wrap(const Container & data, ValueExtractor && ve, ArgumentExtractor && ae)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(data, CSE(std::move(ve), std::move(ae)));
	}

	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	//! \param data Dane do opakowania
	//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
	//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
		wrap(Container && data, const ValueExtractor & ve = ValueExtractor(),
			const ArgumentExtractor & ae = ArgumentExtractor())
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(std::move(data), CSE(ve, se));
	}

	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	//! \param data Dane do opakowania
	//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
	//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
	//! \return Interfejs do danych wraz z danymi
	static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
		wrap(Container && data, ValueExtractor && ve, ArgumentExtractor && ae)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(std::move(data), CSE(std::move(ve), std::move(ae)));
	}

	//TODO
	//! -------------------------------- Niezale¿ne wartoœci i próbki ---------------------



	//! -------------------------------- Rozk³ad wektora na sk³adowe
	struct Vector
	{
		//! Dyskretny
		template<typename AccessorType, typename std::enable_if<is_valid_discrete_accessor_ptr<AccessorType>::value>::type * = 0>
		static inline DiscreteAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), 0)), typename AccessorPointerDesc<AccessorType>::argument_type>
			wrap(const AccessorType & accessor, const std::size_t Idx)
		{
			return DiscreteAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), 0)), typename AccessorPointerDesc<AccessorType>::argument_type>(new SafeDiscreteAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, VectorElementExtractor>(accessor, VectorElementExtractor(idx)));
		}

		template<typename ValueType, typename ArgumentType>
		static inline DiscreteAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<ValueType>(), 0)), ArgumentType>
			lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor, const std::size_t Idx)
		{
			return DiscreteAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<ValueType>(), 0)), ArgumentType>(new DiscreteAccessorAdapter<ValueType, ArgumentType, VectorElementExtractor>(accessor, VectorElementExtractor(idx)));
		}


		//! Ci¹g³y
		template<typename AccessorType, typename std::enable_if<is_valid_function_accessor_ptr<AccessorType>::value>::type * = 0>
		static inline FunctionAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), std::declval<typename AccessorPointerDesc<AccessorType>::argument_type>())), typename AccessorPointerDesc<AccessorType>::argument_type>
			wrap(const AccessorType & accessor, const std::size_t Idx)
		{
			return FunctionAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), std::declval<typename AccessorPointerDesc<AccessorType>::argument_type>())), typename AccessorPointerDesc<AccessorType>::argument_type>(new SafeFunctionAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, VectorElementExtractor>(accessor, VectorElementExtractor(idx)));
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType>
			lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & accessor, const std::size_t Idx)
		{
			return FunctionAccessorPtr<decltype(std::declval<VectorElementExtractor>().extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType>(new FunctionAccessorAdapter<ValueType, ArgumentType, VectorElementExtractor>(accessor, VectorElementExtractor(idx)));
		}

	};

	template<std::size_t Idx>
	struct StaticVector
	{
		//! Dyskretny
		template<typename AccessorType, typename std::enable_if<is_valid_discrete_accessor_ptr<AccessorType>::value>::type * = 0>
		static inline DiscreteAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), 0)), typename AccessorPointerDesc<AccessorType>::argument_type>
			wrap(const AccessorType & accessor)
		{
			return DiscreteAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), 0)), typename AccessorPointerDesc<AccessorType>::argument_type>(new SafeDiscreteAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, StaticVectorElementExtractor<Idx>>(accessor));
		}

		template<typename ValueType, typename ArgumentType>
		static inline DiscreteAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<ValueType>(), 0)), ArgumentType>
			lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return DiscreteAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<ValueType>(), 0)), ArgumentType>(new DiscreteAccessorAdapter<ValueType, ArgumentType, StaticVectorElementExtractor<Idx>>(accessor));
		}

		//! Ci¹g³y

		template<typename AccessorType, typename std::enable_if<is_valid_function_accessor_ptr<AccessorType>::value>::type * = 0>
		static inline FunctionAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), std::declval<typename AccessorPointerDesc<AccessorType>::argument_type>())), typename AccessorPointerDesc<AccessorType>::argument_type>
			wrap(const AccessorType & accessor)
		{
			return FunctionAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), std::declval<typename AccessorPointerDesc<AccessorType>::argument_type>())), typename AccessorPointerDesc<AccessorType>::argument_type>(new SafeFunctionAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, StaticVectorElementExtractor<Idx>>(accessor));
		}		

		template<typename ValueType, typename ArgumentType>
		static inline FunctionAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType>
			lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & accessor)
		{
			return FunctionAccessorPtr<decltype(StaticVectorElementExtractor<Idx>::extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType>(new FunctionAccessorAdapter<ValueType, ArgumentType, StaticVectorElementExtractor<Idx>>(accessor));
		}
	};

	//};



	//! -------------------------------- Funkcje ------------------------------------------

	//struct FunctionWrapper
	//{
	//! \tparam T Typ wartoœci
	//! \tparam U Typ argumentu
	template<class T, class U>
	//! \param func Funkcja jednoargumentowa zwracaj¹ca wartoœci
	//! \return Interfejs dostêpu do danych
	static inline FunctionAccessorPtr<T, U> lightWrap(T(*const func)(U),
		const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new FunctionAccessor<T(U)>(func, even, odd, monotony));
	}

	//! \tparam Func Typ funkcji
	template<typename Func>
	//! \param func Funkcja jednoargumentowa zwracaj¹ca wartoœci
	//! \return Interfejs dostêpu do danych
	static inline typename FuncRet<Func>::type wrap(const Func & func, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FuncRet<Func>::type(new FunctionAccessor<typename FuncRet<Func>::R(typename FuncRet<Func>::Arg)>(func, even, odd, monotony));
	}

	//! \tparam Func Typ funkcji
	template<typename Func>
	//! \param func Funkcja jednoargumentowa zwracaj¹ca wartoœci
	//! \return Interfejs dostêpu do danych
	static inline typename FuncRet<Func>::type wrap(Func && func, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FuncRet<Func>::type(new FunctionAccessor<typename FuncRet<Func>::R(typename FuncRet<Func>::Arg)>(std::move(func), even, odd, monotony));
	}

	//! \tparam T Typ wartoœci
	//! \tparam U Typ argumentu
	//! \tparam Object Typ obiektu
	template<class T, class U, class Object>
	//! \param obj Obiekt dostarczaj¹cy danych
	//! \param func Metoda obiektu za pomoca której pobieramy dane
	//! \return Interfejs dostêpu do danych
	static inline FunctionAccessorPtr<T, U> lightWrap(const Object * const obj,
		T(Object::*const func)(U) const, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new FunctionAccessor<T(Object::*)(U)const>(obj, func, even, odd, monotony));
	}

	//! \tparam T Typ wartoœci
	//! \tparam U Typ argumentu
	//! \tparam Object Typ obiektu
	template<class T, class U, class Object>
	//! \param obj Obiekt dostarczaj¹cy danych
	//! \param func Metoda obiektu za pomoca której pobieramy dane
	//! \return Interfejs dostêpu do danych wrac z danymi
	static inline FunctionAccessorPtr<T, U> wrap(const Object & obj,
		T(Object::*const func)(U) const, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new DataFunctionAccessor<T(Object::*)(U)const>(obj, func, even, odd, monotony));
	}

	//! \tparam T Typ wartoœci
	//! \tparam U Typ argumentu
	//! \tparam Object Typ obiektu
	template<class T, class U, class Object>
	//! \param obj Obiekt dostarczaj¹cy danych
	//! \param func Metoda obiektu za pomoca której pobieramy dane
	//! \return Interfejs dostêpu do danych wraz z danymi
	static inline FunctionAccessorPtr<T, U> wrap(Object && obj,
		T(Object::*const func)(U) const, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new DataFunctionAccessor<T(Object::*)(U)const>(std::move(obj), func, even, odd, monotony));
	}
	//};

	//struct SubAccessorWrapper
	//{
	template<typename ValueType, typename ArgumentType>
	static inline DiscreteAccessorPtr<ValueType, ArgumentType> lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & channel,
		const std::size_t start, const std::size_t size)
	{
		return DiscreteAccessorPtr<ValueType, ArgumentType>(new DiscreteSubAccessorAdapter<ValueType, ArgumentType>(channel, start, size));
	}

	template<typename ValueType, typename ArgumentType>
	static inline FunctionAccessorPtr<ValueType, ArgumentType> lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & channel,
		const ArgumentType start, const ArgumentType end)
	{
		return FunctionAccessorPtr<ValueType, ArgumentType>(new FunctionSubAccessorAdapter<ValueType, ArgumentType>(channel, start, end));
	}

	template<typename AccessorType, typename std::enable_if<std::is_base_of<IDiscreteAccessorT<typename AccessorType::value_type, typename AccessorType::argument_type>, AccessorType>::value>::type * = 0 >
	static inline DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type> wrap(const utils::shared_ptr<AccessorType> & channel,
		const std::size_t start, const std::size_t size)
	{
		return DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type>(new SafeDiscreteSubAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type>(channel, start, size));
	}

	template<typename AccessorType, typename std::enable_if<std::is_base_of<IFunctionAccessorT<typename AccessorType::value_type, typename AccessorType::argument_type>, AccessorType>::value>::type * = 0 >
	static inline FunctionAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type> wrap(const utils::shared_ptr<AccessorType> & channel,
		const typename AccessorType::argument_type & start, const typename AccessorType::argument_type & end)
	{
		return FunctionAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type>(new SafeFunctionSubAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type>(channel, start, end));
	}
	//};
}

#endif	// __HEADER_GUARD_DATACHANNEL__WRAPPERS_H__