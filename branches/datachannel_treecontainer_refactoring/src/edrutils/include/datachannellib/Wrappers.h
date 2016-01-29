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
	//! ------------------------------- Generowane argumenty ---------------------------------------------
	//! \tparam ValuesContainer Typ kontenera z danymi
	//! \tparam ArgumentsGenerator Typ generuj¹cy argumenty
	template<typename ValuesContainer, typename ArgumentsGenerator,
		typename ValuesExtractor = ContainerValueExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValuesExtractor>().value(std::declval<ValuesContainer>(), 0)),
		decltype(std::declval<ArgumentsGenerator>().argument(0)) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci¹gaj¹cy próbki z danych
		//! \return Lekki interfejs do danych
		static inline Ptr lightWrap(ValuesContainer && data, ArgumentsGenerator && ag,
			ValuesExtractor && ve = ValuesExtractor())
	{
		return Ptr(new ContainerDiscreteGeneratedAccessor<ValuesContainer, ArgumentsGenerator, ValuesExtractor>(data,
			utils::ContainerSizeExtractor::size(data), std::forward<ArgumentsGenerator>(ag), std::forward<ValuesExtractor>(ve)));
	}

	//! ---------------------------- Generowane argumenty + dane ------------------------------------------
	//! \tparam ValuesContainer Typ kontenera z danymi
	//! \tparam ArgumentsGenerator Typ generuj¹cy argumenty
	template<typename Value, typename ArgumentsGenerator, typename ValuesExtractor = ContainerValueExtractor,
		typename ContainerExtractor = utils::ValueCarrierDefaultExtractor<Value>,
		typename Container = typename utils::remove_toplevel<decltype(std::declval<ContainerExtractor>().extract(std::declval<Value>()))>::type,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValuesExtractor>().value(std::declval<Container>(), 0)),
		decltype(std::declval<ArgumentsGenerator>().argument(0)) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci¹gaj¹cy próbki z danych
		//! \return Lekki interfejs do danych
		static inline Ptr wrap(Value && value, ArgumentsGenerator && ag,
			ValuesExtractor && ve = ValuesExtractor(), ContainerExtractor && ce = ContainerExtractor())
	{
		utils::ValueCarrier<Container> cvc(std::forward<Value>(value), std::forward<ContainerExtractor>(ce));
		const auto & c = cvc.ref();
		return Ptr(new ContainerDiscreteGeneratedData<Container, ArgumentsGenerator, ValuesExtractor>(std::move(cvc),
			utils::ContainerSizeExtractor::size(c), std::forward<ArgumentsGenerator>(ag), std::forward<ValuesExtractor>(ve)));
	}

	//! ---------------------- W³asny ekstraktor ca³ych próbek -------------
	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci¹gaj¹cy próbki z danych
		//! \return Lekki interfejs do danych
		static inline Ptr lightWrap(const Container & data, SampleExtractor && se = SampleExtractor())
	{
		return Ptr(new ContainerDiscreteAccessor<Container, SampleExtractor>(data, utils::ContainerSizeExtractor::size(data), std::forward<SampleExtractor>(se)));
	}

	//! ---------------------- W³asne ekstraktory wartoœci i argumentów
	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))) >>
		//! \param data Dane do opakowania
		//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
		//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
		//! \return Lekki interfejs do danych
		static inline Ptr lightWrap(const Container & data, ValueExtractor && ve,
			ArgumentExtractor && ae)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return lightWrap<Container, CSE>(data, CSE(std::forward<ValueExtractor>(ve),
			std::forward<ArgumentExtractor>(se)));
	}

	//! ---------------------- W³asny ekstraktor ca³ych próbek -------------
	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ wyci¹gaj¹cy próbki z kontenera
	template<class Value, typename SampleExtractor = DefaultContainerSampleExtractor,
		typename ContainerExtractor = utils::ValueCarrierDefaultExtractor<Value>,
		typename Container = typename utils::remove_toplevel<decltype(std::declval<ContainerExtractor>().extract(std::declval<Value>()))>::type,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first) >>
		//! \param data Dane do opakowania
		//! \param se Obiekt wyci¹gaj¹cy próbki z danych
		//! \return Interfejs do danych wraz z danymi
		static inline Ptr wrap(Value && value, SampleExtractor && se = SampleExtractor(), ContainerExtractor && ce = ContainerExtractor())
	{
		utils::ValueCarrier<Container> cvc(std::forward<Value>(value), std::forward<ContainerExtractor>(ce));
		const auto & c = cvc.ref();
		return Ptr(new ContainerDiscreteData<Container, SampleExtractor>(std::move(cvc), utils::ContainerSizeExtractor::size(c), std::forward<SampleExtractor>(se)));
	}

	//! ---------------------- W³asne ekstraktory wartoœci i argumentów
	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Container, typename ValueExtractor, typename ArgumentExtractor,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))) >>
		//! \param data Dane do opakowania
		//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
		//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
		//! \return Interfejs do danych wraz z danymi
		static inline Ptr wrap(Container && data, ValueExtractor && ve,
			ArgumentExtractor && ae)
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(std::forward<Container>(data),
			CSE(std::forward<ValueExtractor>(ve), std::forward<ArgumentExtractor>(ae)));
	}

	//! \tparam Container Typ kontenera
	//! \tparam ValueExtractor Typ wyci¹gaj¹cy wartoœci z kontenera
	//! \tparam ArgumentExtractor Typ wyci¹gaj¹cy argumenty z kontenera
	template<class Value, typename ValueExtractor, typename ArgumentExtractor,
		typename ContainerExtractor = utils::ValueCarrierDefaultExtractor<Value>,
		typename Container = typename utils::remove_toplevel<decltype(std::declval<ContainerExtractor>().extract(std::declval<Value>()))>::type,
		typename Ptr = DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))) >>
		//! \param data Dane do opakowania
		//! \param ve Obiekt wyci¹gaj¹cy wartoœci z danych
		//! \param ae Obiekt wyci¹gaj¹cy argumenty z danych
		//! \return Interfejs do danych wraz z danymi
		static inline Ptr wrap(Value && value, ValueExtractor && ve,
			ArgumentExtractor && ae, ContainerExtractor && ce = ContainerExtractor(),
			ENABLE_IF(is_valid_discrete_accessor_ptr<Ptr>::value))
	{
		using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
		return wrap<Container, CSE>(utils::ValueCarrier<Container>(std::forward<Value>(value), std::forward<ContainerExtractor>(ce)),
			CSE(std::forward<ValueExtractor>(ve), std::forward<ArgumentExtractor>(ae)));
	}

	//TODO
	//! -------------------------------- Niezale¿ne wartoœci i próbki ---------------------



	//! -------------------------------- Rozk³ad wektora na sk³adowe
	struct Vector
	{
		//! Dyskretny
		template<typename AccessorType, ENABLE_IF(is_valid_discrete_accessor_ptr<AccessorType>::value),
			typename Ptr = DiscreteAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), 0)), typename AccessorPointerDesc<AccessorType>::argument_type >>
			static inline Ptr wrap(const AccessorType & accessor, const std::size_t Idx)
		{
			return Ptr(new SafeDiscreteAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, utils::ArrayElementExtractor>(accessor, utils::ArrayElementExtractor(idx)));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = DiscreteAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<ValueType>(), 0)), ArgumentType >>
			static inline Ptr lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor, const std::size_t Idx)
		{
			return Ptr(new DiscreteAccessorAdapter<ValueType, ArgumentType, utils::ArrayElementExtractor>(accessor, utils::ArrayElementExtractor(idx)));
		}


		//! Ci¹g³y
		template<typename AccessorType, ENABLE_IF(is_valid_function_accessor_ptr<AccessorType>::value),
			typename Ptr = FunctionAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), std::declval<typename AccessorPointerDesc<AccessorType>::argument_type>())), typename AccessorPointerDesc<AccessorType>::argument_type >>
			static inline Ptr wrap(const AccessorType & accessor, const std::size_t Idx)
		{
			return Ptr(new SafeFunctionAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, utils::ArrayElementExtractor>(accessor, utils::ArrayElementExtractor(idx)));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = FunctionAccessorPtr<decltype(std::declval<utils::ArrayElementExtractor>().extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType >>
			static inline Ptr lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & accessor, const std::size_t Idx)
		{
			return Ptr(new FunctionAccessorAdapter<ValueType, ArgumentType, utils::ArrayElementExtractor>(accessor, utils::ArrayElementExtractor(idx)));
		}

	};

	template<std::size_t Idx>
	struct StaticVector
	{
		//! Dyskretny
		template<typename AccessorType, ENABLE_IF(is_valid_discrete_accessor_ptr<AccessorType>::value),
			typename Ptr = DiscreteAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), 0)), typename AccessorPointerDesc<AccessorType>::argument_type >>
			static inline Ptr wrap(const AccessorType & accessor)
		{
			return Ptr(new SafeDiscreteAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, utils::StaticArrayElementExtractor<Idx>>(accessor));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = DiscreteAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<ValueType>(), 0)), ArgumentType >>
			static inline Ptr lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return Ptr(new DiscreteAccessorAdapter<ValueType, ArgumentType, utils::StaticArrayElementExtractor<Idx>>(accessor));
		}

		//! Ci¹g³y

		template<typename AccessorType, ENABLE_IF(is_valid_function_accessor_ptr<AccessorType>::value),
			typename Ptr = FunctionAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<typename AccessorPointerDesc<AccessorType>::value_type>(), std::declval<typename AccessorPointerDesc<AccessorType>::argument_type>())), typename AccessorPointerDesc<AccessorType>::argument_type >>
			static inline Ptr wrap(const AccessorType & accessor)
		{
			return Ptr(new SafeFunctionAccessorAdapter<typename AccessorPointerDesc<AccessorType>::value_type, typename AccessorPointerDesc<AccessorType>::argument_type, utils::StaticArrayElementExtractor<Idx>>(accessor));
		}

		template<typename ValueType, typename ArgumentType,
			typename Ptr = FunctionAccessorPtr<decltype(utils::StaticArrayElementExtractor<Idx>::extract(std::declval<ValueType>(), std::declval<ArgumentType>())), ArgumentType >>
			static inline Ptr lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & accessor)
		{
			return Ptr(new FunctionAccessorAdapter<ValueType, ArgumentType, utils::StaticArrayElementExtractor<Idx>>(accessor));
		}
	};

	//! -------------------------------- Funkcje ------------------------------------------
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
	static inline typename FuncRet<Func>::type wrap(Func && func, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FuncRet<Func>::type(new FunctionAccessor<typename FuncRet<Func>::R(typename FuncRet<Func>::Arg)>(std::forward<Func>(func), even, odd, monotony));
	}

	//! \tparam T Typ wartoœci
	//! \tparam U Typ argumentu
	//! \tparam Object Typ obiektu
	template<class T, class U, class Object>
	//! \param obj Obiekt dostarczaj¹cy danych
	//! \param func Metoda obiektu za pomoca której pobieramy dane
	//! \return Interfejs dostêpu do danych wrac z danymi
	static inline FunctionAccessorPtr<T, U> lightWrap(const Object & obj,
		T(Object::*const func)(U) const, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new FunctionAccessor<T(Object::*)(U)const>(obj, func, even, odd, monotony));
	}

	//! \tparam T Typ wartoœci
	//! \tparam U Typ argumentu
	//! \tparam Object Typ obiektu
	template<class T, class U, class Object, typename V>
	//! \param obj Obiekt dostarczaj¹cy danych
	//! \param func Metoda obiektu za pomoca której pobieramy dane
	//! \return Interfejs dostêpu do danych wraz z danymi
	static inline FunctionAccessorPtr<T, U> wrap(V && value,
		T(Object::*const func)(U) const, const bool even = false,
		const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return FunctionAccessorPtr<T, U>(new DataFunctionAccessor<T(Object::*)(U)const>(std::forward<V>(value), func, even, odd, monotony));
	}

	template<typename ValueType, typename ArgumentType,
		typename Ptr = DiscreteAccessorPtr < ValueType, ArgumentType >>
		static inline Ptr lightWrap(const IDiscreteAccessorT<ValueType, ArgumentType> & channel,
			const std::size_t start, const std::size_t size)
	{
		return Ptr(new DiscreteSubAccessorAdapter<ValueType, ArgumentType>(channel, start, size));
	}

	template<typename ValueType, typename ArgumentType,
		typename Ptr = FunctionAccessorPtr < ValueType, ArgumentType >>
		static inline Ptr lightWrap(const IFunctionAccessorT<ValueType, ArgumentType> & channel,
			const ArgumentType start, const ArgumentType end)
	{
		return Ptr(new FunctionSubAccessorAdapter<ValueType, ArgumentType>(channel, start, end));
	}

	template<typename AccessorType, ENABLE_IF(std::is_base_of<IDiscreteAccessorT<typename AccessorType::value_type, typename AccessorType::argument_type>, AccessorType>::value),
		typename Ptr = DiscreteAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type >>
		static inline Ptr wrap(const utils::shared_ptr<AccessorType> & channel,
			const std::size_t start, const std::size_t size)
	{
		return Ptr(new SafeDiscreteSubAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type>(channel, start, size));
	}

	template<typename AccessorType, ENABLE_IF(std::is_base_of<IFunctionAccessorT<typename AccessorType::value_type, typename AccessorType::argument_type>, AccessorType>::value),
		typename Ptr = FunctionAccessorPtr<typename AccessorType::value_type, typename AccessorType::argument_type >>
		static inline Ptr wrap(const utils::shared_ptr<AccessorType> & channel,
			const typename AccessorType::argument_type & start, const typename AccessorType::argument_type & end)
	{
		return Ptr(new SafeFunctionSubAccessorAdapter<typename AccessorType::value_type, typename AccessorType::argument_type>(channel, start, end));
	}
}

#endif	// __HEADER_GUARD_DATACHANNEL__WRAPPERS_H__