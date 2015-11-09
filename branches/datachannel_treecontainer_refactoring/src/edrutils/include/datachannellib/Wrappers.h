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
#include <utils/function_traits.h>

namespace datachannel
{
	//! ---------------------- W³asny ekstraktor ca³ych próbek -------------
	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
		decltype(std::declval<SampleExtractor>().sample(Container()).first)>
		wrap(const Container & data, const SampleExtractor & se = SampleExtractor())
	{
		return AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
			decltype(std::declval<SampleExtractor>().sample(Container()).first)>(new ContainerDiscreteAccessor<Container, SampleExtractor>(data, se));
	}

	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
		decltype(std::declval<SampleExtractor>().sample(Container()).first)>
		wrap(const Container & data, SampleExtractor && se)
	{
		return AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
			decltype(std::declval<SampleExtractor>().sample(Container()).first)>(new ContainerDiscreteAccessor<Container, SampleExtractor>(data, std::move(se)));
	}

	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
		decltype(std::declval<SampleExtractor>().sample(Container()).first)>
		wrap(Container && data, const SampleExtractor & se = SampleExtractor())
	{
		return AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
			decltype(std::declval<SampleExtractor>().sample(Container()).first)>(new ContainerDiscreteAccessor<Container, SampleExtractor>(std::move(data), se));
	}

	template<class Container, typename SampleExtractor = DefaultContainerSampleExtractor>
	AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
		decltype(std::declval<SampleExtractor>().sample(Container()).first)>
		wrap(Container && data, SampleExtractor && se)
	{
		return AccessorPtr<decltype(std::declval<SampleExtractor>().sample(Container()).second),
			decltype(std::declval<SampleExtractor>().sample(Container()).first)>(new ContainerDiscreteAccessor<Container, SampleExtractor>(std::move(data), std::move(se)));
	}

	//! ---------------------- W³asne ekstraktory wartoœci i argumentów
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	AccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0)))>
		wrap(const Container & data, const ValueExtractor & ve = ValueExtractor(),
		const ArgumentExtractor & ae = ArgumentExtractor())
	{
		return wrap<Container, ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>>(data, ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>(ve, se));
	}
	
	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	AccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0)))>
		wrap(const Container & data, ValueExtractor && ve = ValueExtractor(),
		ArgumentExtractor && ae = ArgumentExtractor())
	{
		return wrap <Container, ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>>(data, ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>(std::move(ve), std::move(se)));
	}

	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	AccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0)))>
		wrap(Container && data, const ValueExtractor & ve = ValueExtractor(),
		const ArgumentExtractor & ae = ArgumentExtractor())
	{
		return wrap <Container, ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>>(std::move(data), ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>(ve, se));
	}

	template<class Container, typename ValueExtractor, typename ArgumentExtractor>
	AccessorPtr<decltype(std::declval<ValueExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0))),
		decltype(std::declval<ArgumentExtractor>().value(ContainerValueExtractor<ValueExtractor, ArgumentExtractor>().value(std::declval<Container>(), 0)))>
		wrap(Container && data, ValueExtractor && ve = ValueExtractor(),
		ArgumentExtractor && ae = ArgumentExtractor())
	{
		return wrap <Container, ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>>(std::move(data), ContainerSampleExtractor<ValueExtractor, ArgumentExtractor>(std::move(ve), std::move(se)));
	}

	//TODO
	//! -------------------------------- Niezale¿ne wartoœci i próbki ---------------------

	

	//! -------------------------------- Funkcje ------------------------------------------

	template<class T, class U>
	AccessorPtr<typename std::decay<T>::type, typename std::decay<U>::type> wrap(const T(*func)(U),
		const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return AccessorPtr<typename std::decay<T>::type, typename std::decay<U>::type>(new ContinuesFunction(func, even, odd, monotony));
	}

	/*template<typename Func>
	AccessorPtr<typename std::function_traits<Func>::decay_return_type, typename std::function_traits<Func>::decay_argument<0>::type> wrap(
		const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return AccessorPtr<typename std::function_traits<Func>::decay_return_type, typename std::function_traits<Func>::decay_argument<0>::type>(new ContinuesFunction<Func>(even, odd, monotony));
	}*/

	template<typename Func>
	AccessorPtr<typename std::function_traits<Func>::decay_return_type, typename std::function_traits<Func>::decay_argument<0>::type> wrap(const Func & func,
		const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return AccessorPtr<typename std::function_traits<Func>::decay_return_type, typename std::function_traits<Func>::decay_argument<0>::type>(new ContinuesFunction<Func>(func, even, odd, monotony));
	}

	template<typename Func>
	AccessorPtr<typename std::function_traits<Func>::decay_return_type, typename std::function_traits<Func>::decay_argument<0>::type> wrap(Func && func,
		const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return AccessorPtr<typename std::function_traits<Func>::decay_return_type, typename std::function_traits<Func>::decay_argument<0>::type>(new ContinuesFunction<Func>(std::move(func, even, odd, monotony)));
	}

	template<class T, class U, class Object>
	AccessorPtr<typename std::decay<T>::type, typename std::decay<U>::type> wrap(const Object * obj, T(Object::*func)(U) const,
		const bool even = false, const bool odd = false, const MonotonyType monotony = NonMonotonic)
	{
		return AccessorPtr<typename std::decay<T>::type, typename std::decay<U>::type>(new ContinuesFunction(obj, func, even, odd, monotony));
	}	

	template<typename ValueType, typename ArgumentType>
	AccessorPtr<ValueType, ArgumentType> wrap(const IDiscreteAccessor<ValueType, ArgumentType> & channel, const std::size_t start, const std::size_t size)
	{
		return AccessorPtr<ValueType, ArgumentType>(new DiscreteSubAccessorAdapter<ValueType, ArgumentType>(channel, start, size));
	}

	template<typename ValueType, typename ArgumentType>
	AccessorPtr<ValueType, ArgumentType> wrap(const IFunctionAccessor<ValueType, ArgumentType> & channel, const ArgumentType start, const ArgumentType end)
	{
		return AccessorPtr<ValueType, ArgumentType>(new FunctionSubAccessorAdapter<ValueType, ArgumentType>(channel, start, end));
	}
}

#endif	// __HEADER_GUARD_DATACHANNEL__WRAPPERS_H__