/********************************************************************
created:  2015/07/27	09:45:53
filename: WrapperHelpers.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__WRAPPERHELPERS_H__
#define __HEADER_GUARD_DATAACCESSOR__WRAPPERHELPERS_H__

#include <dataaccessorlib/SafeAccessorWrapper.h>
#include <dataaccessorlib/FunctionFeature.h>
#include <iterator>
#include <utils/function_traits.h>
#include <utils/container_traits.h>

namespace dataaccessor
{
	//! Taka czêsciowa specjalizacja dzia³a jak enable_if, daj¹c nam potrzebne typy,
	//! lub brak ich definicji - SFINAE i eliminacja niepasuj¹cych sygnatur wrpaerów
	template<typename Func, bool = std::is_one_argument_functor<Func>::value>
	struct FuncRet
	{
		using R = typename std::function_traits<Func>::return_type;
		using Arg = typename std::function_traits<Func>:: template argument<0>::type;
		using type = FunctionAccessorPtr<R, Arg>;
	};

	template<typename Func>
	struct FuncRet<Func, false> {};

	//! Extractor wartoœci kontenera
	struct ContainerValueExtractor
	{
	public:

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C Kontener dla któego pobieramy element
		//! \param idx Indeks elementu który chcemy pobraæ z kontenera
		//! \return Element kontenera o podanym indeksie
		static inline auto value(const C & c, const std::size_t idx) -> decltype(utils::ContainerElementExtractor::extract(c, idx))
		{
			return utils::ContainerElementExtractor::extract(c, idx);
		}
	};

	//! Struktura pomocnicza przy wyci¹ganiu argumentu z pary
	struct PairArgumentExtractor
	{
		//! \tparam T Pierwszy typ w parze
		//! \tparam U Drugi typ w parze
		template<typename T, typename U>
		//! \param sample Próbka
		//! \return argument próbki
		static inline const T & value(const std::pair<T, U> & sample)
		{
			return sample.first;
		}
	};

	//! Struktura pomocnicza przy wyci¹ganiu wartoœci z pary
	struct PairValueExtractor
	{
		//! \tparam T Pierwszy typ w parze
		//! \tparam U Drugi typ w parze
		template<typename T, typename U>
		//! \param sample Próbka
		//! \return Wartoœæ próbki
		static inline const U & value(const std::pair<T, U> & sample)
		{
			return sample.second;
		}
	};

	//! Domyœlny extraktor próbek z kontenerów
	struct DefaultContainerSampleExtractor
	{
		//! \tparam C Typ kontenera
		template<typename C>
		//! \param container Kontener z którego pobieramy próbkê
		//! \param idx Numer próbki któr¹ chcemy pobraæ
		//! \return Próbka o zadanym indeksie
		static inline auto sample(const C & container, const std::size_t idx) -> decltype(ContainerValueExtractor::value(container, idx))
		{
			return ContainerValueExtractor::value(container, idx);
		}
	};

	//! \tparam SValueExtractor Typ extraktora vartoœci z próbek kontenera
	//! \tparam SArgumentExtractor Typ extraktora argumentów z próbek kontenera
	//! \tparam CValueExtractor Typ extraktora próbek z kontenera
	template<typename SValueExtractor,
		typename SArgumentExtractor,
		typename CValueExtractor = ContainerValueExtractor>
		//! Klasa pomocnicza przy wypakowywaniu próbek z kontenerów
	class ContainerSampleExtractor : private CValueExtractor,
		private SValueExtractor, private SArgumentExtractor
	{
	public:

		//! \tparam C Typ kontenera
		template<typename C>
		//! Typ wartoœci w kontenerze - z nich wyci¹gamy argumenty i wartoœci
		using CValue = decltype(std::declval<CValueExtractor>().value(std::declval<C>(), 0));

		//! \tparam C Typ kontenera
		template<typename C>
		struct Sample
		{
			//! Musi tak byæ, w przeciwnym razie VS ma problemy
			typedef typename IAccessorT<typename std::decay<decltype(std::declval<SValueExtractor>().value(std::declval<CValue<C>>()))>::type,
				typename std::decay<decltype(std::declval<SArgumentExtractor>().value(std::declval<CValue<C>>()))>::type>::sample_type type;
		};

	public:

		ContainerSampleExtractor(const ContainerSampleExtractor & Other)
			: SValueExtractor(Other),
			SArgumentExtractor(Other),
			CValueExtractor(Other)
		{

		}

		ContainerSampleExtractor(ContainerSampleExtractor && Other)
			: SValueExtractor(std::move(Other)),
			SArgumentExtractor(std::move(Other)),
			CValueExtractor(std::move(Other))
		{

		}

		template<typename SVT = SValueExtractor,
			typename SAT = SArgumentExtractor,
			typename CVT = CValueExtractor>
			ContainerSampleExtractor(SVT && svalueExtractor,
				SAT && sargumentExtractor,
				CVT && cvalueExtractor = CValueExtractor())
			: CValueExtractor(std::forward<CVT>(cvalueExtractor)),
			SValueExtractor(std::forward<SAT>(svalueExtractor)),
			SArgumentExtractor(std::forward<SVT>(sargumentExtractor))
		{

		}

		//! Destruktor
		virtual ~ContainerSampleExtractor() {}

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param container Kontener z którego wypakowujemy próbki accessora
		//! \param idx Indeks próbki któr¹ chemy wypakowaæ
		//! \return Próbka accessora któr¹ wypakowaliœmy z kontenera
		inline typename Sample<C>::type sample(const C & container, const std::size_t idx) const
		{
			using RCVal = decltype(CValueExtractor::value(container, idx));
			RCVal cval = CValueExtractor::value(container, idx);
			return typename Sample<C>::type(SArgumentExtractor::value(cval), SValueExtractor::value(cval));
		}
	};

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora próbek akcesora	
	template<typename Container,
		typename SampleExtractor = DefaultContainerSampleExtractor>
		//! Implementacja dyskrentego akseora dla kontenerów danych
	class ContainerDiscreteAccessor : public IOptimizedDiscreteAccessorT<
		typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().second)>::type,
		typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().first)>::type>,
		private SampleExtractor
	{
	public:

		template<typename ST = SampleExtractor>
		ContainerDiscreteAccessor(const Container & container, const std::size_t size,
			ST && sampleExtractor = ST())
			: SampleExtractor(std::forward<ST>(sampleExtractor)), container(container),
			size_(size), empty_(size == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~ContainerDiscreteAccessor() {}

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{
			return SampleExtractor::sample(container, idx);
		}

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana³ nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:
		//! Kontener z danymi
		const Container & container;
		//! Rozmiar kontenrea
		const std::size_t size_;
		//! Czy kontener jest pusty
		const bool empty_;
	};

	template<typename Container,
		typename SampleExtractor = DefaultContainerSampleExtractor>
		using ContainerDiscreteData = SafeAccessorWrapper<ContainerDiscreteAccessor<Container, SampleExtractor>, Container>;

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora próbek akcesora	
	template<typename Container,
		typename ArgumentsGenerator,
		typename ValueExtractor = ContainerValueExtractor>
		//! Implementacja dyskrentego akseora dla kontenerów danych
	class ContainerDiscreteGeneratedAccessor : public IIndependentDiscreteAccessorT<
		typename std::decay<decltype(std::declval<ValueExtractor>().value(std::declval<Container>(), 0))>::type,
		typename std::decay<decltype(std::declval<ArgumentsGenerator>().argument(0))>::type>,
		private ArgumentsGenerator, private ValueExtractor
	{
	public:
		template<typename AT = ArgumentsGenerator,
			typename VT = ValueExtractor>
			ContainerDiscreteGeneratedAccessor(const Container & container, const std::size_t size,
				AT && argumentsGenerator = AT(),
				VT && valuesExtractor = VT())
			: ArgumentsGenerator(std::forward<AT>(argumentsGenerator)),
			ValueExtractor(std::forward<VT>(valuesExtractor)),
			container(container), size_(size), empty_(size == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~ContainerDiscreteGeneratedAccessor() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentsGenerator::argument(idx); }

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual value_type value(const size_type idx) const override { return ValueExtractor::value(container, idx); }

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana³ nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:
		//! Kontener z danymi
		const Container & container;
		//! Rozmiar kontenrea
		const std::size_t size_;
		//! Czy kontener jest pusty
		const bool empty_;
	};

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora próbek akcesora	
	template<typename Container,
		typename ArgumentsGenerator,
		typename ValueExtractor = ContainerValueExtractor>
		using ContainerDiscreteGeneratedData = SafeAccessorWrapper<ContainerDiscreteGeneratedAccessor<Container, ArgumentsGenerator, ValueExtractor>, Container>;

	//! \tparam ValuesContainer Typ kontenera z wartoœciami
	//! \tparam ArgumentsContainer Typ kontenera z argumentami
	//! \tparam ValueExtractor Typ obiektu wypakowuj¹cego wartoœci z kontenera wartoœci
	//! \tparam ArgumentExtractor Typ obiektu wypakowuj¹cego argumenty z kontenera argumentów
	template<typename ValuesContainer, typename ArgumentsContainer,
		typename ValueExtractor, typename ArgumentExtractor>
		//! Implementacja realizuje dostêp do kana³u dyskretnego w oparciu o niezale¿ne kontenery wartosci i próbek
	class IndependentContainersDiscreteAccessor : public IIndependentDiscreteAccessorT<
		typename std::decay<decltype(std::declval<ValueExtractor>().value(std::declval<ValuesContainer>(), 0))>::type,
		typename std::decay<decltype(std::declval<ArgumentExtractor>().value(std::declval<ArgumentsContainer>(), 0))>::type>,
		private ArgumentExtractor, private ValueExtractor
	{
	public:

		template<typename VT = ValueExtractor,
			typename AT = ArgumentExtractor>
			IndependentContainersDiscreteAccessor(const ValuesContainer & values,
				const ArgumentsContainer & arguments, const std::size_t vsize,
				const std::size_t asize, VT && valueExtractor = VT(),
				AT && argumentExtractor = AT())
			: ArgumentsGenerator(std::forward<AT>(argumentsGenerator)),
			ValueExtractor(std::forward<ValueExtractor>(valuesExtractor)),
			values(values), arguments(arguments), size_(std::min(vsize, asize)),
			empty_(size_ == 0)
		{

		}

		//! Destruktor wirtualny
		virtual ~IndependentContainersDiscreteAccessor() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentExtractor::value(container, idx); }

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual value_type value(const size_type idx) const override { return return ValueExtractor::value(container, idx); }

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return size_; }

		//! \return Czy kana³ nie zawiera danych
		virtual bool empty() const override { return empty_; }

	private:
		//! Kontener z wartoœciami
		const ValuesContainer & values;
		//! Kontener z argumentami
		const ArgumentsContainer & arguments;
		//! Rozmiar danych
		const std::size_t size_;
		//! Czy pusty akcesor
		const bool empty_;
	};

	//! Klasa realizuj¹ca zadanie wrappera dla kontenera - tag pozwala na dziedziczenie
	//! po tej klasie przy takich samych typach kontenerów wartoœci i argumentów
	//! za spraw¹ taga mamy 2 niezale¿ne typy
	template<typename Container, typename>
	class TaggedContainerCarrier : public utils::ValueCarrier<Container>
	{
	public:
		//! TODO VS szaleje z tak¹ konstrukcj¹ wiêc tworzymy konstruktor forwarduj¹cy wszystko dalej do klasy bazowej
		//! Wci¹gamy konstruktory
		//using utils::ValueCarrier::ValueCarrier;
		template<class ...Args>
		TaggedContainerCarrier(Args && ...args) : utils::ValueCarrier<Container>(std::forward<Args>(args)...) {}

		//! Destruktor wirtualny
		virtual ~TaggedContainerCarrier() {}
	};

	//! Typy tagów
	struct Tag {
		//! Wartoœci
		class ValueContainer;
		//! Argumenty
		class ArgumentContainer;
	};

	//! \tparam ValuesContainer Typ kontenera z wartoœciami
	//! \tparam ArgumentsContainer Typ kontenera z argumentami
	//! \tparam ValueExtractor Typ obiektu wypakowuj¹cego wartoœci z kontenera wartoœci
	//! \tparam ArgumentExtractor Typ obiektu wypakowuj¹cego argumenty z kontenera argumentów
	template<typename ValuesContainer, typename ArgumentsContainer,
		typename ValueExtractor, typename ArgumentExtractor>
		//! Implementacja realizuje dostêp do kana³u dyskretnego w oparciu o niezale¿ne kontenery wartosci i próbek
	class IndependentContainersDiscreteData : private TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>,
		private TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>,
		public IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
		ValueExtractor, ArgumentExtractor>
	{
	public:

		template<typename VCT = utils::ValueCarrier<ValuesContainer>,
			typename ACT = utils::ValueCarrier<ArgumentsContainer>,
			typename VT = ValueExtractor,
			typename AT = ArgumentExtractor>
			IndependentContainersDiscreteData(VCT && values,
				ACT && arguments, const std::size_t vsize,
				const std::size_t asize, VT && valueExtractor = VT(),
				AT && argumentExtractor = AT())
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(std::forward<VCT>(values)),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(std::forward<ACT>(arguments)),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::ref(),
				TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::ref(), vsize, asize,
				std::forward<VT>(valueExtractor), std::forward<AT>(argumentExtractor))
		{

		}

		//! Destruktor wirtualny
		virtual ~IndependentContainersDiscreteData() {}
	};

	template<typename ArgumentsGenerator, typename ValueType>
	class ConstDiscreteData : public IIndependentDiscreteAccessorT<typename std::decay<ValueType>::type, typename std::decay<decltype(std::declval<ArgumentsGenerator>().argument(0))>::type>,
		private ArgumentsGenerator
	{
	public:

		template<typename AT = ArgumentsGenerator,
			typename VT = ValueType>
			ConstDiscreteData(AT && argumentsGenerator, VT && value) :
			ArgumentsGenerator(std::forward<AT>(argumentsGenerator)),
			value(std::forward<VT>(value))
		{

		}

		//! Destruktor wirtualny
		virtual ~ConstDiscreteData() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentsGenerator::argument(idx); }

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual value_type value(const size_type idx) const override { return value_; }

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return ArgumentsGenerator::size(); }

	private:
		//! Wartoœæ
		const ValueType value_;
	};

	template<typename ValueType, typename ArgumentType>
	class ConstFunctionData : public IGeneratedFunctionAccessorT<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>
	{
	public:
		template<typename VT = ValueType>
		//! \param value Wartoœæ kana³u
		ConstFunctionData(VT && value) : value_(std::forward<VT>(value))
		{

		}

		//! Destruktor wirtualny
		virtual ~ConstFunctionData() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual value_type value(const argument_type & argument) const override { return value_; }

	private:
		//! Wartoœæ
		const value_type value_;
	};

	//! \tparam
	template<typename>
	//! Forward declaration opisu funkcji ci¹g³ej
	class FunctionAccessor;

	template<typename ValueType, typename ArgumentType>
	class FunctionAccessorBase : public IGeneratedFunctionAccessorT<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>
	{
	public:

		//! \param even
		//! \param odd
		//! \param monotony
		FunctionAccessorBase(const bool even = false, const bool odd = false,
			const MonotonyType monotony = NonMonotonic)
		{
			attachFeature(IFeaturePtr(new FunctionFeature(even, odd, monotony)));
		}

		//! Virtual destructor
		virtual ~FunctionAccessorBase() {}
	};

	//! \tparam Ret Typ wartoœci zwracanej z funckji
	//! \tparam Arg Typ wartoœci argumentu funkcji
	template<typename Ret, typename Arg>
	//! Implementacja wrappera dla funkcji ci¹g³ej
	class FunctionAccessor<Ret(Arg)> : public FunctionAccessorBase<Ret, Arg>
	{
	public:
		//! \tparam Func Typ funktora
		template<typename Func>
		//! \param func Funktor generuj¹cy wartoœci funkcji
		FunctionAccessor(Func && func, const bool even = false,
			const bool odd = false, const MonotonyType monotony = NonMonotonic)
			: FunctionAccessorBase(even, odd, monotony), f_(std::forward<Func>(func)) {}

		//! Destruktor
		virtual ~FunctionAccessor() {}
		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return f_(argument); }

	private:
		//! Funktor realizuj¹cy ci¹g³¹ funkcjê - ci¹g³¹ w sensie daje wartoœci dla ka¿dej wartoœci zadanego typu argumentu
		std::function<Ret(Arg)> f_;
	};

	//! \tparam Ret Typ wartoœci zwracanej z funckji
	//! \tparam Arg Typ wartoœci argumentu funkcji
	//! \tparam Object Typ obiektu który dostarcza danych
	template<typename Ret, typename Arg, typename Object>
	//! Implementacja wrappera dla funkcji ci¹g³ej
	class FunctionAccessor<Ret(Object::*)(Arg)const> : public FunctionAccessorBase<Ret, Arg>
	{
	public:
		//! \param obj Obiekt na którym wo³amy funkcjê
		FunctionAccessor(const Object & obj, Ret(Object::*const func)(Arg)const,
			const bool even = false, const bool odd = false,
			const MonotonyType monotony = NonMonotonic)
			: FunctionAccessorBase(even, odd, monotony), obj(obj), func(func) {}

		//! Destruktor
		virtual ~FunctionAccessor() {}
		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return (obj.*func)(argument); }

	private:
		//! Obiekt
		const Object & obj;
		//! Metoda
		Ret(Object::* const func)(Arg)const;
	};

	//! \tparam Ret Typ wartoœci zwracanej z funckji
	//! \tparam Arg Typ wartoœci argumentu funkcji
	//! \tparam Object Typ obiektu który dostarcza danych
	template<typename Ret, typename Arg, typename Object>
	//! Implementacja wrappera dla funkcji ci¹g³ej
	using DataFunctionAccessor = SafeAccessorWrapper<FunctionAccessor<Ret(Object::*)(Arg)const>, Object>;
}

#endif	// __HEADER_GUARD_DATAACCESSOR__WRAPPERHELPERS_H__