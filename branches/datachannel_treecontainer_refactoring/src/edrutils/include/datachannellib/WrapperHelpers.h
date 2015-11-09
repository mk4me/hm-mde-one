/********************************************************************
	created:  2015/07/27	09:45:53
	filename: WrapperHelpers.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__WRAPPERHELPERS_H__
#define __HEADER_GUARD_DATACHANNEL__WRAPPERHELPERS_H__

#include <datachannellib/IFunctionFeature.h>

namespace datachannel
{
	//! Extractor pusto�ci kontenera
	struct ContainersEmptinessExtractor
	{
		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C kontener kt�ry testujemy pod katem pusto�ci
		//! \return Czy kontener jest pusty
		static inline bool empty(const C & c)
		{
			return c.empty();
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam N Rozmiar tablicy
		template<typename T, std::size_t N>
		//! \param dummy
		//! \return Czy kontener jest pusty
		static inline bool empty(const T(&)[N])
		{
			return false;
		}
	};

	//! Extractor rozmiaru kontenera
	struct ContainersSizeExtractor
	{
		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C Kontener dla kt�rego pobieramy ilo�� element�w
		//! \return Rozmiar kontenera
		static inline std::size_t size(const C & c)
		{
			return c.size();
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam N Rozmiar tablicy
		template<typename T, std::size_t N>
		//! \param dummy
		//! \return Rozmiar kontenera
		static inline std::size_t size(const T(&)[N])
		{
			return N;
		}
	};

	//! Extractor warto�ci kontenera
	struct ContainerValueExtractor
	{
		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C Kontener dla kt�ego pobieramy element
		//! \param idx Indeks elementu kt�ry chcemy pobra� z kontenera
		//! \return Element kontenera o podanym indeksie
		static inline const typename C::value_type & value(const C & c, const std::size_t idx)
		{
			auto it = c.begin();
			std::advance(it, idx);
			return *it;
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam N Rozmiar tablicy
		template<typename T, std::size_t N>
		//! \param array Tablica z kt�rej wyci�gamy element
		//! \param idx Indeks elementu kt�ry chcemy pobra� z kontenera
		//! \return Element kontenera o podanym indeksie
		static inline const T & value(const std::array<T,N> & array, const std::size_t idx)
		{
			return array[idx];
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam N Rozmiar tablicy
		template<typename T, std::size_t N>
		//! \param array Tablica z kt�rej wyci�gamy element
		//! \param idx Indeks elementu kt�ry chcemy pobra� z kontenera
		//! \return Element kontenera o podanym indeksie
		static inline const T & value(const T(&array)[N], const std::size_t idx)
		{
			return array[idx];
		}		
	};

	//! Struktura pomocnicza przy wyci�ganiu argumentu z pary
	struct PairArgumentExtractor
	{
		//! \tparam T Pierwszy typ w parze
		//! \tparam U Drugi typ w parze
		template<typename T, typename U>
		//! \param sample Pr�bka
		//! \return argument pr�bki
		static inline const T & value(const std::pair<T, U> & sample)
		{
			return sample.first;
		}
	};

	//! Struktura pomocnicza przy wyci�ganiu warto�ci z pary
	struct PairValueExtractor
	{
		//! \tparam T Pierwszy typ w parze
		//! \tparam U Drugi typ w parze
		template<typename T, typename U>
		//! \param sample Pr�bka
		//! \return Warto�� pr�bki
		static inline const U & value(const std::pair<T, U> & sample)
		{
			return sample.second;
		}
	};
	
	class DefaultContainerSampleExtractor
	{
	public:

		template<typename Container>
		using Sample = decltype(std::declval<ContainerValueExtractor>().value(std::declval<Container>(), 0));

		template<typename Container>
		using ArgumentType = decltype(std::declval<Sample<Container>>().first);

		template<typename Container>
		using ValueType = decltype(std::declval<Sample<Container>>().second);

	public:

		template<typename Container>//, typename std::enable_if<std::is_same<IAccessor<ValueType<Container>, ArgumentType<Container>>::sample_type, Sample<Container>>()>::type>
		static inline const Sample<Container> & sample(const Container & container, const std::size_t idx)
		{			
			return ContainerValueExtractor.value(container, idx);
		}
	};

	template<typename SValueExtractor,
		typename SArgumentExtractor,
		typename CValueExtractor = ContainerValueExtractor>
	class ContainerSampleExtractor
	{
	public:

		template<typename Container>
		using CValue = decltype(std::declval<CValueExtractor>().value(std::declval<Container>(), 0));

		template<typename Container>
		using Sample = typename IAccessor<decltype(std::declval<SValueExtractor>().value(std::declval<CValue<Container>>())),
			decltype(std::declval<SArgumentExtractor>().value(std::declval<CValue<Container>>()))>::sample_type;

	public:

		ContainerSampleExtractor(const ContainerSampleExtractor & Other)
			: svalueExtractor(Other.svalueExtractor),
			sargumentExtractor(Other.sargumentExtractor),
			cvalueExtractor(Other.cvalueExtractor)
		{

		}

		ContainerSampleExtractor(ContainerSampleExtractor && Other)
			: svalueExtractor(std::move(Other.svalueExtractor)),
			sargumentExtractor(std::move(Other.sargumentExtractor)),
			cvalueExtractor(std::move(Other.cvalueExtractor))
		{

		}

		ContainerSampleExtractor(const SValueExtractor & svalueExtractor = SValueExtractor(),
			const SArgumentExtractor & sargumentExtractor = SArgumentExtractor(),
			const CValueExtractor & cvalueExtractor = CValueExtractor())
			: cvalueExtractor(cvalueExtractor),
			svalueExtractor(svalueExtractor),
			sargumentExtractor(sargumentExtractor)
		{

		}

		ContainerSampleExtractor(SValueExtractor && svalueExtractor,
			SArgumentExtractor && sargumentExtractor,
			const CValueExtractor & cvalueExtractor = CValueExtractor())
			: cvalueExtractor(cvalueExtractor),
			svalueExtractor(svalueExtractor),
			sargumentExtractor(sargumentExtractor)
		{

		}

		virtual ~ContainerSampleExtractor() {}

		template<typename Container>
		inline Sample<Container> sample(const Container & container, const std::size_t idx)
		{
			const auto cval = cvalueExtractor.value(container, idx);
			return Sample<Container>(sargumentExtractor.value(cval), svalueExtractor.value(cval));
		}

	private:		
		SValueExtractor svalueExtractor;
		SArgumentExtractor sargumentExtractor;
		CValueExtractor cvalueExtractor;
	};

	template<typename Container,
		typename SampleExtractor,		
		typename SizeExtractor = ContainersSizeExtractor>
	class ContainerDiscreteAccessor : public IOptimizedDiscreteAccessor<
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).second),
		decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0).first)>
	{
	public:

		ContainerDiscreteAccessor(const Container & container,		
			const SampleExtractor & sampleExtractor = SampleExtractor(),			
			SizeExtractor sizeExtractor = SizeExtractor())
			: container(container), sampleExtractor(sampleExtractor),
			size_(sizeExtractor.size(container)), empty_(size_ == 0)
		{

		}

		ContainerDiscreteAccessor(Container && container,
			const SampleExtractor & sampleExtractor = SampleExtractor(),
			SizeExtractor sizeExtractor = SizeExtractor())
			: container(std::move(container)), sampleExtractor(sampleExtractor),
			size_(sizeExtractor.size(container)), empty_(size_ == 0)
		{

		}

		ContainerDiscreteAccessor(const Container & container,
			SampleExtractor && sampleExtractor,
			SizeExtractor sizeExtractor = SizeExtractor())
			: container(container), sampleExtractor(std::move(sampleExtractor)),
			size_(sizeExtractor.size(container)), empty_(size_ == 0)
		{

		}

		ContainerDiscreteAccessor(Container && container,
			SampleExtractor && sampleExtractor,
			SizeExtractor sizeExtractor = SizeExtractor())
			: container(std::move(container)), sampleExtractor(std::move(sampleExtractor)),
			size_(sizeExtractor.size(container)), empty_(size_ == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~ContainerDiscreteAccessor() {}		

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{			
			return sampleExtractor.sample(container, idx);
		}

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:

		const Container container;
		const SampleExtractor sampleExtractor;
		const std::size_t size_;
		const bool empty_;
	};

	template<typename ValuesContainer, typename ArgumentsContainer,
		typename ValueExtractor, typename ArgumentExtractor,
		typename VSizeExtractor = ContainersSizeExtractor,
		typename ASizeExtractor = ContainersSizeExtractor>
	class IndependentContainersDiscreteAccessor : public IIndependentDiscreteAccessor<
		decltype(std::declval<ValueExtractor>().value(std::declval<ValuesContainer>(), 0)),
		decltype(std::declval<ArgumentExtractor>().value(std::declval<ArgumentsContainer>(), 0))>
	{
	public:

		IndependentContainersDiscreteAccessor(const ValuesContainer & values,
			const ArgumentsContainer & arguments,
			ValueExtractor valueExtractor = ValueExtractor(),
			ArgumentExtractor argumentExtractor = ArgumentExtractor(),
			VSizeExtractor vsizeExtractor = VSizeExtractor(),
			ASizeExtractor asizeExtractor = SizeExtractor())
			: values(values), arguments(arguments), sampleExtractor(sampleExtractor),
			valueExtractor(valueExtractor),	argumentExtractor(argumentExtractor),
			size_(std::min(vsizeExtractor.size(values), asizeExtractor.size(arguments))),
			empty_(size_ == 0)
		{

		}

		IndependentContainersDiscreteAccessor(ValuesContainer && values,
			const ArgumentsContainer & arguments,
			ValueExtractor valueExtractor = ValueExtractor(),
			ArgumentExtractor argumentExtractor = ArgumentExtractor(),
			VSizeExtractor vsizeExtractor = VSizeExtractor(),
			ASizeExtractor asizeExtractor = SizeExtractor())
			: values(std::move(values)), arguments(arguments), sampleExtractor(sampleExtractor),
			valueExtractor(valueExtractor), argumentExtractor(argumentExtractor),
			size_(std::min(vsizeExtractor.size(values), asizeExtractor.size(arguments))),
			empty_(size_ == 0)
		{

		}

		IndependentContainersDiscreteAccessor(const ValuesContainer & values,
			ArgumentsContainer && arguments,
			ValueExtractor valueExtractor = ValueExtractor(),
			ArgumentExtractor argumentExtractor = ArgumentExtractor(),
			VSizeExtractor vsizeExtractor = VSizeExtractor(),
			ASizeExtractor asizeExtractor = SizeExtractor())
			: values(values), arguments(std::move(arguments)), sampleExtractor(sampleExtractor),
			valueExtractor(valueExtractor), argumentExtractor(argumentExtractor),
			size_(std::min(vsizeExtractor.size(values), asizeExtractor.size(arguments))),
			empty_(size_ == 0)
		{

		}

		IndependentContainersDiscreteAccessor(ValuesContainer && values,
			ArgumentsContainer && arguments,
			ValueExtractor valueExtractor = ValueExtractor(),
			ArgumentExtractor argumentExtractor = ArgumentExtractor(),
			VSizeExtractor vsizeExtractor = VSizeExtractor(),
			ASizeExtractor asizeExtractor = SizeExtractor())
			: values(std::move(values)), arguments(std::move(arguments)), sampleExtractor(sampleExtractor),
			valueExtractor(valueExtractor), argumentExtractor(argumentExtractor),
			size_(std::min(vsizeExtractor.size(values), asizeExtractor.size(arguments))),
			empty_(size_ == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~IndependentContainersDiscreteAccessor() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return argumentExtractor.value(container, idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return return valueExtractor.value(container, idx); }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:

		const ValuesContainer values;
		const ArgumentsContainer arguments;
		const ValueExtractor valueExtractor;
		const ArgumentExtractor argumentExtractor;
		const std::size_t size_;
		const bool empty_;		
	};

	template<typename ArgumentsGenerator, typename ValueType>
	class ConstDiscreteAccessor : public IIndependentDiscreteAccessor<ValueType, decltype(std::declval<ArgumentsGenerator>().argument(0))>
	{
	public:		

		ConstDiscreteAccessor(const ArgumentsGenerator & argumentsGenerator,
			const ValueType & value) : argumentsGenerator(argumentsGenerator), value(value)
		{

		}

		ConstDiscreteAccessor(ArgumentsGenerator && argumentsGenerator,
			const ValueType & value) : argumentsGenerator(std::move(argumentsGenerator)),
			value(value)
		{

		}

		ConstDiscreteAccessor(const ArgumentsGenerator & argumentsGenerator,
			ValueType && value) : argumentsGenerator(argumentsGenerator), value(std::move(value))
		{

		}

		ConstDiscreteAccessor(ArgumentsGenerator && argumentsGenerator,
			ValueType && value) : argumentsGenerator(std::move(argumentsGenerator)),
			value(std::move(value))
		{

		}

		//! Destruktor wirtualny
		virtual ~ConstDiscreteAccessor() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return argumentsGenerator.argument(idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return value_; }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return argumentsGenerator.size(); }

	private:

		const ArgumentsGenerator argumentsGenerator;
		const ValueType value_;		
	};

	template<typename ValueType, typename ArgumentType>
	class ConstContinousAccessor : public IGeneratedFunctionAccessor<ValueType, ArgumentType>
	{
	public:

		ConstContinousAccessor(const ValueType & value) : value_(value)
		{

		}

		ConstContinousAccessor(ValueType && value) : value_(std::move(value))
		{

		}

		//! Destruktor wirtualny
		virtual ~ConstContinousAccessor() {}
		
		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual ValueType value(const ArgumentType & argument) const override { return value_; }

	private:

		const ValueType value_;
	};

	//! \tparam
	template<typename>
	//! Forward declaration opisu funkcji ci�g�ej
	class ContinuesFunction;

	template<typename ValueType, typename ArgumentType>
	class ContinousFunctionBase : public IGeneratedFunctionAccessor<ValueType, ArgumentType>
	{
	public:

		//! \param even
		//! \param odd
		//! \param monotony
		ContinousFunctionBase(const bool even = false, const bool odd = false,
			const MonotonyType monotony = NonMonotonic)
		{
			attachFeature(IFeaturePtr(new FunctionFeature(even, odd, monotony)));
		}

		//! Virtual destructor
		virtual ~ContinousFunctionBase() {}
	};

	//! \tparam Ret Typ warto�ci zwracanej z funckji
	//! \tparam Arg Typ warto�ci argumentu funkcji
	template<typename Ret, typename Arg>
	//! Implementacja wrappera dla funkcji ci�g�ej
	class ContinuesFunction<Ret(Arg)> : public ContinousFunctionBase<typename std::decay<Ret>::type, typename std::decay<Arg>::type>
	{
	public:
		//! \tparam Func Typ funktora
		template<typename Func>
		//! \param func Funktor generuj�cy warto�ci funkcji
		ContinuesFunction(Func && func, const bool even = false,
			const bool odd = false,	const MonotonyType monotony = NonMonotonic)
			: ContinousFunctionBase(even, odd, monotony), f_(std::forward<Func>(func)) {}

		//! \tparam Func Typ funktora
		template<typename Func>
		//! \param func Funktor generuj�cy warto�ci funkcji
		ContinuesFunction(const Func & func, const bool even = false,
			const bool odd = false,	const MonotonyType monotony = NonMonotonic)
			: ContinousFunctionBase(even, odd, monotony), f_(func) {}

		//! Destruktor
		virtual ~ContinuesFunction() {}
		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual Ret value(const Arg & argument) const override { return f_(argument); }

	private:
		//! Funktor realizuj�cy ci�g�� funkcj� - ci�g�� w sensie daje warto�ci dla ka�dej warto�ci zadanego typu argumentu
		std::function<Ret(Arg)> f_;
	};

	//! \tparam Ret Typ warto�ci zwracanej z funckji
	//! \tparam Arg Typ warto�ci argumentu funkcji
	template<typename Ret, typename Arg, typename Object>
	//! Implementacja wrappera dla funkcji ci�g�ej
	class ContinuesFunction<Ret(Object::*)(Arg)const> : public ContinousFunctionBase<typename std::decay<Ret>::type, typename std::decay<Arg>::type>
	{
	public:
		//! \param obj Obiekt na kt�rym wo�amy funkcj�
		ContinuesFunction(const Object * obj, Ret(Object::*func)(Arg)const,
			const bool even = false, const bool odd = false,
			const MonotonyType monotony = NonMonotonic)
			: ContinousFunctionBase(even, odd, monotony), obj(obj), func(func) {}

		//! Destruktor
		virtual ~ContinuesFunction() {}
		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual Ret value(const Arg & argument) const override { return (obj->*func)(argument); }

	private:
		//! Obiekt
		const Object * obj;
		const Ret(Object::*func)(Arg)const;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__WRAPPERHELPERS_H__