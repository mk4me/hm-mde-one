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
#include <utils/Debug.h>

namespace datachannel
{
	//struct DataWrapper
	//{
		//! ---------------------- W³asny ekstraktor ca³ych próbek -------------
		template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
			lightWrap(const Container & data, const SampleExtractor & se = SampleExtractor())
		{
			return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
				decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteAccessor<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), se));
		}

		template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
			lightWrap(const Container & data, SampleExtractor && se)
		{
			return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
				decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteAccessor<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), std::move(se)));
		}

		//! ---------------------- W³asne ekstraktory wartoœci i argumentów
		template<class Container, typename ValueExtractor, typename ArgumentExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
			decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
			lightWrap(const Container & data, const ValueExtractor & ve = ValueExtractor(),
			const ArgumentExtractor & ae = ArgumentExtractor())
		{
			using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
			return lightWrap<Container, CSE>(data, CSE(ve, se));
		}

		template<class Container, typename ValueExtractor, typename ArgumentExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
			decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
			lightWrap(const Container & data, ValueExtractor && ve, ArgumentExtractor && ae)
		{
			using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
			return lightWrap<Container, CSE>(data, CSE(std::move(ve), std::move(se)));
		}

		//---------------------------------------------------------------------

		//! ---------------------- W³asny ekstraktor ca³ych próbek -------------
		template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
			wrap(const Container & data, const SampleExtractor & se = SampleExtractor())
		{
			return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
				decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteData<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), se));
		}

		template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
			wrap(const Container & data, SampleExtractor && se)
		{
			return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
				decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteData<Container, SampleExtractor>(data, ContainersSizeExtractor::size(data), std::move(se)));
		}

		template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
			decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
			wrap(Container && data, const SampleExtractor & se = SampleExtractor())
		{
			const auto size = ContainersSizeExtractor::size(data);
			return DiscreteAccessorPtr<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
				decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>(
				new ContainerDiscreteData<Container, SampleExtractor>(std::move(data), size, se));
		}

		template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
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
		template<class Container, typename ValueExtractor, typename ArgumentExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
			decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
			wrap(const Container & data, const ValueExtractor & ve = ValueExtractor(),
			const ArgumentExtractor & ae = ArgumentExtractor())
		{
			using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
			return wrap<Container, CSE>(data, CSE(ve, ae));
		}

		template<class Container, typename ValueExtractor, typename ArgumentExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
			decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
			wrap(const Container & data, ValueExtractor && ve, ArgumentExtractor && ae)
		{
			using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
			return wrap<Container, CSE>(data, CSE(std::move(ve), std::move(ae)));
		}

		template<class Container, typename ValueExtractor, typename ArgumentExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
			decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
			wrap(Container && data, const ValueExtractor & ve = ValueExtractor(),
			const ArgumentExtractor & ae = ArgumentExtractor())
		{			
			using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
			return wrap<Container, CSE>(std::move(data), CSE(ve, se));
		}

		template<class Container, typename ValueExtractor, typename ArgumentExtractor>
		static inline DiscreteAccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0))),
			decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor::value(std::declval<Container>(), 0)))>
			wrap(Container && data, ValueExtractor && ve, ArgumentExtractor && ae)
		{
			using CSE = ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>;
			return wrap<Container, CSE>(std::move(data), CSE(std::move(ve), std::move(ae)));
		}

		//TODO
		//! -------------------------------- Niezale¿ne wartoœci i próbki ---------------------
	//};



	//! -------------------------------- Funkcje ------------------------------------------

	//struct FunctionWrapper
	//{
		template<class T, class U>
		static inline FunctionAccessorPtr<T, U> wrap(T(*const func)(U),
			const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
		{
			return FunctionAccessorPtr<T, U>(new FunctionAccessor<T(U)>(func, even, odd, monotony));
		}

		template<typename Func>
		static inline typename FuncRet<Func>::type wrap(const Func & func, const bool even = false,
			const bool odd = false, const MonotonyType monotony = NonMonotonic)
		{
			return FuncRet<Func>::type(new FunctionAccessor<typename FuncRet<Func>::R(typename FuncRet<Func>::Arg)>(func, even, odd, monotony));
		}

		template<typename Func>
		static inline typename FuncRet<Func>::type wrap(Func && func, const bool even = false,
			const bool odd = false, const MonotonyType monotony = NonMonotonic)
		{
			return FuncRet<Func>::type(new FunctionAccessor<typename FuncRet<Func>::R(typename FuncRet<Func>::Arg)>(std::move(func), even, odd, monotony));
		}

		template<class T, class U, class Object>
		static inline FunctionAccessorPtr<T, U> wrap(const Object * const obj,
			T(Object::*const func)(U) const, const bool even = false,
			const bool odd = false, const MonotonyType monotony = NonMonotonic)
		{
			return FunctionAccessorPtr<T, U>(new FunctionAccessor<T(Object::*)(U)const> (obj, func, even, odd, monotony));
		}
	//};

	//struct SubAccessorWrapper
	//{
		template<typename ValueType, typename ArgumentType>
		static inline DiscreteAccessorPtr<ValueType, ArgumentType> wrap(const IDiscreteAccessor<ValueType, ArgumentType> & channel,
			const std::size_t start, const std::size_t size)
		{
			return DiscreteAccessorPtr<ValueType, ArgumentType>(new DiscreteSubAccessorAdapter<ValueType, ArgumentType>(channel, start, size));
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionAccessorPtr<ValueType, ArgumentType> wrap(const IFunctionAccessor<ValueType, ArgumentType> & channel,
			const ArgumentType start, const ArgumentType end)
		{
			return FunctionAccessorPtr<ValueType, ArgumentType>(new FunctionSubAccessorAdapter<ValueType, ArgumentType>(channel, start, end));
		}
	//};
}

#endif	// __HEADER_GUARD_DATACHANNEL__WRAPPERS_H__