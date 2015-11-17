/********************************************************************
	created:  2015/07/27	09:45:53
	filename: WrapperHelpers.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__WRAPPERHELPERS_H__
#define __HEADER_GUARD_DATACHANNEL__WRAPPERHELPERS_H__

#include <datachannellib/IFunctionFeature.h>
#include <iterator>
#include <type_traits>
#include <utils/function_traits.h>

namespace datachannel
{
	//! \tparam C Typ kontenera, który weryfikujemy
	template<typename C>
	//! Struktura pomocnicza przy weryfikacji tablic (operator [])
	struct empty_member_check
	{
		//! Weryfikacja operatora [] w wersji const
		template<typename U> static typename std::enable_if<std::is_same<bool, decltype(std::declval<const U>().empty())>::value>::type * check(const U *);		
		//! Fallback
		template<typename U> static int check(...);
		//! Informacja o operatorze
		using type = typename std::is_pointer<decltype(check<C>((const C*)0))>::type;

		static const bool valid = type();
	};

	//! \tparam C Typ kontenera, który weryfikujemy
	template<typename C>
	//! Struktura pomocnicza przy weryfikacji tablic (operator [])
	struct size_member_check
	{
		//! Weryfikacja operatora [] w wersji const
		template<typename U> static typename std::enable_if<std::is_integral<decltype(std::declval<const U>().size())>::value>::type * check(const U *);
		//! Fallback
		template<typename U> static int check(...);
		//! Informacja o operatorze
		using type = typename std::is_pointer<decltype(check<C>((const C*)0))>::type;

		static const bool valid = type();
	};

	//! Extractor pustoœci kontenera
	struct ContainersEmptinessExtractor
	{
	private:

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C kontener który testujemy pod katem pustoœci
		//! \return Czy kontener jest pusty
		static inline bool empty_impl(const C & c, std::true_type)
		{
			return c.empty();
		}

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C kontener który testujemy pod katem pustoœci
		//! \return Czy kontener jest pusty
		static inline bool empty_impl(const C & c, std::false_type)
		{
			return c.size() == 0;
		}

	public:
		//! \tparam C Typ kontenera
		template<typename C, typename std::enable_if<std::is_class<C>::value>::type * = 0>
		//! \param C kontener który testujemy pod katem pustoœci
		//! \return Czy kontener jest pusty
		static inline bool empty(const C & c)
		{
			static_assert(empty_member_check<C>::valid || size_member_check<C>::valid, "Object must have at least valid empty or size function members");
			return empty_impl(c, empty_member_check<C>::type());
		}

		//! \tparam T Typ elementu tablicy
		//! \tparam N Rozmiar tablicy
		template<typename T, std::size_t N>
		//! \param dummy
		//! \return Czy kontener jest pusty
		static inline bool empty(const T(&)[N])
		{
			//TODO
			//czy tutaj zawsze false nie moze leciec?
			return N == 0;
		}
	};

	//! Extractor rozmiaru kontenera
	struct ContainersSizeExtractor
	{
		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C Kontener dla którego pobieramy iloœæ elementów
		//! \return Rozmiar kontenera
		static inline auto size(const C & c) -> decltype(c.size())
		{
			static_assert(std::is_integral<decltype(std::declval<C>().size())>::value, "Value return by size must be integral type");
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

	//! \tparam A Typ tablicowy, który weryfikujemy
	template<typename A>
	//! Struktura pomocnicza przy weryfikacji tablic (operator [])
	struct array_check
	{
		//! Weryfikacja operatora [] w wersji const
		template<typename U> static typename std::enable_if<(sizeof(decltype(std::declval<const U>()[0]))>=0)>::type * check(const U *);
		//! Weryfikacja operatora []
		template<typename U> static typename std::enable_if<(sizeof(decltype(std::declval<U>()[0]))>=0)>::type * check(U *);
		//! Fallback
		template<typename U> static int check(...);
		//! Informacja o operatorze
		static const bool valid = std::is_pointer<decltype(check<A>((A*)0))>::value || std::is_pointer<decltype(check<A>((const A*)0))>::value;
	};

	//! \tparam A Typ tablicy ajki weryfikujemy
	template<typename A>
	//! W³aœciwy trait przepuszczaj¹cy typy z mo¿liwoœci¹ indeksowania operatorem []
	struct is_array : public std::conditional<(std::is_array<A>::value || array_check<A>::valid), std::true_type, std::false_type>::type {};

	//! Taka czêœciowa specjalizacja pozwala nam unikn¹æ odwo³añ do function_traits dla typów nie reprezentuj¹cych funkcje/funktory
	template<typename Func, bool = std::is_functor<Func>::value>
	struct is_one_argument_functor : public std::conditional<(std::function_traits<Func>::arity == 1), std::true_type, std::false_type>::type {};

	template<typename Func>
	struct is_one_argument_functor<Func, false> : public std::false_type{};

	//! Taka czêsciowa specjalizacja dzia³a jak enable_if, daj¹c nam potrzebne typy,
	//! lub brak ich definicji - SFINAE i eliminacja niepasuj¹cych sygnatur wrpaerów
	template<typename Func, bool = is_one_argument_functor<Func>::value>
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
	private:		

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C Kontener dla którego pobieramy element
		//! \param idx Indeks elementu który chcemy pobraæ z kontenera
		//! \param dummy Przeci¹¿enie dla kontenerów bez operatora [] - realizacja po interatorach
		//! \return Element kontenera o podanym indeksie
		static inline auto value_impl(const C & c, const std::size_t idx, std::false_type) -> decltype(*(c.begin()))
		{
			static_assert(sizeof(std::iterator_traits<decltype(c.begin())>::value_type) >= 0, "Object begin member should provide valid iterator with full trait specialisation");
			auto it = c.begin();
			std::advance(it, idx);
			return *it;
		}

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C Kontener dla którego pobieramy element
		//! \param idx Indeks elementu który chcemy pobraæ z kontenera
		//! \param dummy Przeci¹¿enie dla kontenerów/typów z operatorem []
		//! \return Element kontenera o podanym indeksie
		static inline auto value_impl(const C & c, const std::size_t idx, std::true_type) -> decltype((c[idx]))
		{			
			return c[idx];
		}

	public:

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param C Kontener dla któego pobieramy element
		//! \param idx Indeks elementu który chcemy pobraæ z kontenera
		//! \return Element kontenera o podanym indeksie
		static inline auto value(const C & c, const std::size_t idx) -> decltype((value_impl(c, idx, is_array<C>::type())))
		{
			return value_impl(c, idx, is_array<C>::type());
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
		static inline auto sample(const C & container, const std::size_t idx) -> decltype((ContainerValueExtractor::value(container, idx)))
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
		using CValue = decltype((std::declval<CValueExtractor>().value(std::declval<C>(), 0)));

		//! \tparam C Typ kontenera
		template<typename C>
		struct Sample
		{
			typedef typename IAccessor<typename std::decay<decltype(std::declval<SValueExtractor>().value(std::declval<CValue<C>>()))>::type,
				typename std::decay<decltype(std::declval<SArgumentExtractor>().value(std::declval<CValue<C>>()))>::type>::sample_type type;
		};
		//! Typ próbki akcesora - wynika z wartoœci zwracanych przez ekstraktory wartoœci i argumentów dla próbek kontenera
		//using Sample = typename IAccessor<typename std::decay<decltype(std::declval<SValueExtractor>().value(std::declval<CValue<C>>()))>::type,
		//	typename std::decay<decltype(std::declval<SArgumentExtractor>().value(std::declval<CValue<C>>()))>::type>::sample_type;

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

		ContainerSampleExtractor(const SValueExtractor & svalueExtractor,
			const SArgumentExtractor & sargumentExtractor,
			const CValueExtractor & cvalueExtractor = CValueExtractor())
			: CValueExtractor(cvalueExtractor),
			SValueExtractor(svalueExtractor),
			SArgumentExtractor(sargumentExtractor)
		{

		}

		ContainerSampleExtractor(SValueExtractor && svalueExtractor,
			SArgumentExtractor && sargumentExtractor,
			CValueExtractor && cvalueExtractor)
			: CValueExtractor(std::move(cvalueExtractor)),
			SValueExtractor(std::move(svalueExtractor)),
			SArgumentExtractor(std::move(sargumentExtractor))
		{

		}

		//! Destruktor
		~ContainerSampleExtractor() {}

		//! \tparam C Typ kontenera
		template<typename C>
		//! \param container Kontener z którego wypakowujemy próbki accessora
		//! \param idx Indeks próbki któr¹ chemy wypakowaæ
		//! \return Próbka accessora któr¹ wypakowaliœmy z kontenera
		inline typename Sample<C>::type sample(const C & container, const std::size_t idx) const
		{
			using RCVal = decltype((CValueExtractor::value(container, idx)));
			RCVal cval = CValueExtractor::value(container, idx);
			return typename Sample<C>::type(SArgumentExtractor::value(cval), SValueExtractor::value(cval));
		}	
	};

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora próbek akcesora	
	template<typename Container,
		typename SampleExtractor>
	//! Implementacja dyskrentego akseora dla kontenerów danych
	class ContainerDiscreteAccessor : public IOptimizedDiscreteAccessor<
		typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().second)>::type,
		typename std::decay<decltype(std::declval<decltype(std::declval<SampleExtractor>().sample(std::declval<Container>(), 0))>().first)>::type>,
		private SampleExtractor
	{
	public:

		ContainerDiscreteAccessor(const Container & container, const std::size_t size,
			const SampleExtractor & sampleExtractor = SampleExtractor())
			: SampleExtractor(sampleExtractor), container(container),
			size_(size), empty_(size == 0)
		{

		}		

		ContainerDiscreteAccessor(const Container & container, const std::size_t size,
			SampleExtractor && sampleExtractor)
			: SampleExtractor(std::move(sampleExtractor)), container(container),
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

	//TODO - dodaæ obs³ugê go³ych wskaŸników? Czy ma to sens? Co z nimi robiæ?
	// kopiowaæ obiekty pod nimi? Trzymaæ tylko wskaŸniki?
	//! \tparam Container Typ kontenera
	template<typename Container>
	//! Klasa realizuje przechowywanie faktycznych danych czy to w formie kontenera czy smart wskaxnika do niego
	class ContainerCarrier
	{
	private:
		//! Klasa bazowa gwarantuj¹ca dostêp do kontenera
		class ContainerCarrierBase
		{
		public:
			//! Destruktor wirtualny
			virtual ~ContainerCarrierBase() {}
			//! \return Kontener
			virtual const Container & container() const = 0;
		};

		//! Forward declaration
		template<typename T>
		class ContainerCarrierImpl;

		//! Specjalizacja dla prostego przechowywania przez wartoœæ
		template<>
		class ContainerCarrierImpl<Container> : public ContainerCarrierBase
		{
		public:
			ContainerCarrierImpl(const Container & container) : container_(container) {}
			ContainerCarrierImpl(Container && container) : container_(std::move(container)) {}

			virtual ~ContainerCarrierImpl() {}

			//! \return Kontener
			virtual const Container & container() const override final { return container_; }

		private:
			const Container container_;
		};

		//! Specjalizacja dla prostego przechowywania przez wartoœæ
		template<typename T, std::size_t N>
		class ContainerCarrierImpl<T[N]> : public ContainerCarrierBase
		{
		public:

			using LC = T[N];

		private:

			ContainerCarrierImpl(const LC & container, std::true_type)
				: container_(new LC)
			{
				std::memcpy(const_cast<T*>(container_.get()), &container, sizeof(container));
			}

			ContainerCarrierImpl(const LC & container, std::false_type)
				: container_(new LC)
			{
				static_assert(std::is_default_constructible<T>::value && std::is_copy_assignable<T>::value, "Type T must be default constructible and trivially assignable");
				auto T* a = const_cast<T*>(container_.get());
				for (auto & val : container)
				{
					*a = val;
					++a;
				}
			}

		public:
			ContainerCarrierImpl(const T(&container)[N]) : ContainerCarrierImpl(container, std::is_pod<T>::value) {}

			virtual ~ContainerCarrierImpl() {}

			//! \return Kontener
			virtual const LC & container() const override final { return container_.get(); }

		private:
			const utils::unique_ptr<const LC> container_;
		};

		//! Wersja dla smart pointerów
		template<template<typename> class SmartPtr>
		class ContainerCarrierImpl<SmartPtr<const Container>> : public ContainerCarrierBase
		{
		private:
			//! Sprawdzenie czy nie pusty wskaŸnik do kontenera
			inline void verify() const { if (container_ == nullptr) throw std::invalid_argument("Uninitialized container"); }

		public:
			ContainerCarrierImpl(const SmartPtr<const Container> & container) : container_(container) { verify(); }
			ContainerCarrierImpl(SmartPtr<const Container> && container) : container_(std::move(container)) { verify(); }

			virtual ~ContainerCarrierImpl() {}

			//! \return Kontener
			virtual const Container & container() const override final { return *container_; }

		private:
			const SmartPtr<const Container> container_;
		};

	public:

		template<typename T>
		ContainerCarrier(const T & container) : containerCarier_(new ContainerCarrierImpl<T>(container)) {}

		template<typename T>
		ContainerCarrier(T && container) : containerCarier_(new ContainerCarrierImpl<T>(std::move(container))) {}

		~ContainerCarrier() {}

		inline const Container & container() const { return containerCarier_->container(); }

	private:
		const utils::shared_ptr<const ContainerCarrierBase> containerCarier_;
	};	

	//! \tparam Container Typ kontenera
	//! \tparam SampleExtractor Typ ekstraktora próbek akcesora	
	template<typename Container,
		typename SampleExtractor>
		//! Implementacja dyskrentego wrappera danych
	class ContainerDiscreteData : private ContainerCarrier<Container>,
		public ContainerDiscreteAccessor<Container, SampleExtractor>
	{
	public:
		template<typename T>
		ContainerDiscreteData(const T & container, const std::size_t size,
			const SampleExtractor & sampleExtractor = SampleExtractor())
			: ContainerCarrier<Container>(container),
			ContainerDiscreteAccessor<Container, SampleExtractor>(ContainerCarrier<Container>::container(),
			size, sampleExtractor)
		{

		}

		template<typename T>
		ContainerDiscreteData(const T & container, const std::size_t size,
			SampleExtractor && sampleExtractor)
			: ContainerCarrier<Container>(container),
			ContainerDiscreteAccessor<Container, SampleExtractor>(ContainerCarrier<Container>::container(),
			size, std::move(sampleExtractor))
		{

		}		

		template<typename T>
		ContainerDiscreteData(T && container, const std::size_t size,
			const SampleExtractor & sampleExtractor = SampleExtractor())
			: ContainerCarrier<Container>(std::move(container)),
			ContainerDiscreteAccessor<Container, SampleExtractor>(ContainerCarrier<Container>::container(),
			size, sampleExtractor)
		{

		}

		template<typename T>
		ContainerDiscreteData(T && container, const std::size_t size,
			SampleExtractor && sampleExtractor)
			: ContainerCarrier<Container>(std::move(container)),
			ContainerDiscreteAccessor<Container, SampleExtractor>(ContainerCarrier<Container>::container(),
			size, std::move(sampleExtractor))
		{

		}

		//! Destruktor wirtualny
		virtual ~ContainerDiscreteData() {}
	};

	//! \tparam ValuesContainer Typ kontenera z wartoœciami
	//! \tparam ArgumentsContainer Typ kontenera z argumentami
	//! \tparam ValueExtractor Typ obiektu wypakowuj¹cego wartoœci z kontenera wartoœci
	//! \tparam ArgumentExtractor Typ obiektu wypakowuj¹cego argumenty z kontenera argumentów
	template<typename ValuesContainer, typename ArgumentsContainer,
		typename ValueExtractor, typename ArgumentExtractor>
	//! Implementacja realizuje dostêp do kana³u dyskretnego w oparciu o niezale¿ne kontenery wartosci i próbek
	class IndependentContainersDiscreteAccessor : public IIndependentDiscreteAccessor<
		typename std::decay<decltype(std::declval<ValueExtractor>().value(std::declval<ValuesContainer>(), 0))>::type,
		typename std::decay<decltype(std::declval<ArgumentExtractor>().value(std::declval<ArgumentsContainer>(), 0))>::type>,
		private ValueExtractor, private ArgumentExtractor
	{
	public:

		IndependentContainersDiscreteAccessor(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t vsize,
			const std::size_t asize,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: ValueExtractor(valueExtractor), ArgumentExtractor(argumentExtractor),
			values(values), arguments(arguments), size_(std::min(vsize, asize)),
			empty_(size_ == 0)
		{

		}

		IndependentContainersDiscreteAccessor(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t size,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: ValueExtractor(valueExtractor), ArgumentExtractor(argumentExtractor),
			values(values), arguments(arguments), size_(size),
			empty_(size == 0)
		{

		}

		IndependentContainersDiscreteAccessor(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t vsize,
			const std::size_t asize,
			ValueExtractor && valueExtractor,
			ArgumentExtractor && argumentExtractor)
			: ValueExtractor(std::move(valueExtractor)),
			ArgumentExtractor(std::move(argumentExtractor)),
			values(values), arguments(arguments), size_(std::min(vsize, asize)),
			empty_(size_ == 0)
		{

		}

		IndependentContainersDiscreteAccessor(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t size,
			ValueExtractor && valueExtractor,
			ArgumentExtractor && argumentExtractor)
			: ValueExtractor(std::move(valueExtractor)),
			ArgumentExtractor(std::move(argumentExtractor)),
			values(values), arguments(arguments), size_(size),
			empty_(size == 0)
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
	class TaggedContainerCarrier : public ContainerCarrier<Container>
	{
	public:
		using ContainerCarrier::ContainerCarrier;

		virtual ~TaggedContainerCarrier() {}
	};

	struct Tag {
		class ValueContainer;
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

		IndependentContainersDiscreteData(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t vsize,
			const std::size_t asize,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(values),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(arguments),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), vsize, asize,
			valueExtractor, argumentExtractor)
		{

		}

		IndependentContainersDiscreteData(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t size,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(values),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(arguments),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), size,
			valueExtractor, argumentExtractor)
		{

		}

		IndependentContainersDiscreteData(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t vsize,
			const std::size_t asize,
			ValueExtractor && valueExtractor,
			ArgumentExtractor && argumentExtractor)
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(values),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(arguments),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), vsize, asize,
			valueExtractor, argumentExtractor)
		{

		}

		IndependentContainersDiscreteData(const ValuesContainer & values,
			const ArgumentsContainer & arguments, const std::size_t size,
			ValueExtractor && valueExtractor,
			ArgumentExtractor && argumentExtractor)
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(values),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(arguments),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), vsize, asize,
			valueExtractor, argumentExtractor)
		{

		}

		//-----------------------------------------------------------------

		IndependentContainersDiscreteData(ValuesContainer && values,
			ArgumentsContainer && arguments, const std::size_t vsize,
			const std::size_t asize,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(std::move(values)),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(std::move(arguments)),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), vsize, asize,
			valueExtractor, argumentExtractor)
		{

		}

		IndependentContainersDiscreteData(ValuesContainer && values,
			ArgumentsContainer && arguments, const std::size_t size,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(std::move(values)),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(std::move(arguments)),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), size,
			valueExtractor, argumentExtractor)
		{

		}

		IndependentContainersDiscreteData(ValuesContainer && values,
			ArgumentsContainer && arguments, const std::size_t vsize,
			const std::size_t asize,
			ValueExtractor && valueExtractor,
			ArgumentExtractor && argumentExtractor)
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(std::move(values)),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(std::move(arguments)),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), vsize, asize,
			valueExtractor, argumentExtractor)
		{

		}

		IndependentContainersDiscreteData(ValuesContainer && values,
			ArgumentsContainer && arguments, const std::size_t size,
			ValueExtractor && valueExtractor,
			ArgumentExtractor && argumentExtractor)
			: TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>(std::move(values)),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>(std::move(arguments)),
			IndependentContainersDiscreteData<ValuesContainer, ArgumentsContainer,
			ValueExtractor, ArgumentExtractor>(TaggedContainerCarrier<ValuesContainer, Tag::ValueContainer>::container(),
			TaggedContainerCarrier<ArgumentsContainer, Tag::ArgumentContainer>::container(), vsize, asize,
			valueExtractor, argumentExtractor)
		{

		}

		//! Destruktor wirtualny
		virtual ~IndependentContainersDiscreteData() {}
	};

	template<typename ArgumentsGenerator, typename ValueType>
	class ConstDiscreteData : public IIndependentDiscreteAccessor<typename std::decay<ValueType>::type, typename std::decay<decltype(std::declval<ArgumentsGenerator>().argument(0))>::type>,
		private ArgumentsGenerator
	{
	public:		

		ConstDiscreteData(const ArgumentsGenerator & argumentsGenerator,
			const ValueType & value) : ArgumentsGenerator(argumentsGenerator), value(value)
		{

		}

		ConstDiscreteData(ArgumentsGenerator && argumentsGenerator,
			const ValueType & value) : ArgumentsGenerator(std::move(argumentsGenerator)),
			value(value)
		{

		}

		ConstDiscreteData(const ArgumentsGenerator & argumentsGenerator,
			ValueType && value) : ArgumentsGenerator(argumentsGenerator), value(std::move(value))
		{

		}

		ConstDiscreteData(ArgumentsGenerator && argumentsGenerator,
			ValueType && value) : ArgumentsGenerator(std::move(argumentsGenerator)),
			value(std::move(value))
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
		const ValueType value_;		
	};

	template<typename ValueType, typename ArgumentType>
	class ConstFunctionData : public IGeneratedFunctionAccessor<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>
	{
	public:

		ConstFunctionData(const ValueType & value) : value_(value)
		{

		}

		ConstFunctionData(ValueType && value) : value_(std::move(value))
		{

		}

		//! Destruktor wirtualny
		virtual ~ConstFunctionData() {}
		
		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual value_type value(const argument_type & argument) const override { return value_; }

	private:

		const value_type value_;
	};

	//! \tparam
	template<typename>
	//! Forward declaration opisu funkcji ci¹g³ej
	class FunctionAccessor;

	template<typename ValueType, typename ArgumentType>
	class FunctionAccessorBase : public IGeneratedFunctionAccessor<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>
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
			const bool odd = false,	const MonotonyType monotony = NonMonotonic)
			: FunctionAccessorBase(even, odd, monotony), f_(std::forward<Func>(func)) {}

		//! \tparam Func Typ funktora
		template<typename Func>
		//! \param func Funktor generuj¹cy wartoœci funkcji
		FunctionAccessor(const Func & func, const bool even = false,
			const bool odd = false,	const MonotonyType monotony = NonMonotonic)
			: FunctionAccessorBase(even, odd, monotony), f_(func) {}

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
	template<typename Ret, typename Arg, typename Object>
	//! Implementacja wrappera dla funkcji ci¹g³ej
	class FunctionAccessor<Ret(Object::*)(Arg)const> : public FunctionAccessorBase<Ret, Arg>
	{
	public:
		//! \param obj Obiekt na którym wo³amy funkcjê
		FunctionAccessor(const Object * const obj, Ret(Object::*const func)(Arg)const,
			const bool even = false, const bool odd = false,
			const MonotonyType monotony = NonMonotonic)
			: FunctionAccessorBase(even, odd, monotony), obj(obj), func(func) {}

		//! Destruktor
		virtual ~FunctionAccessor() {}
		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return (obj->*func)(argument); }

	private:
		//! Obiekt
		const Object * const obj;
		//! Metoda
		Ret(Object::* const func)(Arg)const;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__WRAPPERHELPERS_H__