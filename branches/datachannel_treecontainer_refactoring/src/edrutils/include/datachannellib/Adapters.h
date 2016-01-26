/********************************************************************
	created:  2015/07/28	15:50:12
	filename: Adapter.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__ADAPTER_H__
#define __HEADER_GUARD_DATACHANNEL__ADAPTER_H__

#include <datachannellib/Accessors.h>
#include <datachannellib/SafeAccessorWrapper.h>
#include <datachannellib/UniformArgumentsFeature.h>
#include <datachannellib/BoundedArgumentsFeature.h>
#include <datachannellib/BoundedValuesFeature.h>
#include <datachannellib/FunctionFeature.h>
#include <datachannellib/Interpolators.h>
#include <datachannellib/Extrapolators.h>

namespace dataaccessor
{
	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType>
	//! Klasa realizuj�ca generator argument�w
	class UniformArgumentsGenerator
	{
	public:
		//! \param step Krok z jakim generujemy argumenty
		//! \param start Warto�c argumentu od kt�rego zaczynamy
		//! \param end Warto�c argumentu na kt�rej ko�czymy!!
		UniformArgumentsGenerator(const ArgumentType step,
			const std::size_t size,
			const ArgumentType start = ArgumentType(0))
			: start_(start), step_(step),
			size_(size)
		{
			
		}

		//! \param Other Kopiowany generator
		UniformArgumentsGenerator(const UniformArgumentsGenerator & Other)
			: start_(Other.start_), size_(Other.size_),
			step_(Other.step_), feature(Other.feature)
		{

		}

		//! \tparam T
		template<typename T>
		//! \param Other Kopiowany generator
		UniformArgumentsGenerator(const UniformArgumentsGenerator<T> & Other)
			: start_(Other.start_), size_(Other.size_),
			step_(Other.step_)
		{
			if (Other.feature != nullptr) {
				feature = utils::make_shared<UniformArgumentsFeature<ArgumentType>>(Other.feature->argumentsInterval());
			}
		}

		//! \param Przenoszony generator
		UniformArgumentsGenerator(UniformArgumentsGenerator && Other)
			: start_(Other.start_), size_(Other.size_),
			step_(Other.step_), feature(std::move(Other.feature))
		{
			Other.start_ = Other.step_ = ArgumentType();
			Other.size_ = 0;
		}

		//! Destruktor
		~UniformArgumentsGenerator() {}

		//! \param idx Index dla kt�rego generujemy argument
		inline ArgumentType argument(const std::size_t idx) const
		{
			if (idx >= size_){
				throw std::range_error("Generator range exceeded");
			}

			return start_ + idx * step_;
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline ArgumentType start() const
		{
			return start_;
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline ArgumentType end() const
		{
			return start_ + size_ * step_;
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline std::size_t size() const
		{
			return size_;
		}

		//! \return Krok argument�w
		inline ArgumentType step() const
		{
			return step_;
		}

		//! \param ValueType Typ warto�ci kana�u
		template<typename ValueType>
		//! \param accessor Akcesor kt�ry zasilamy t� cech�
		inline void init(IAccessorT<ValueType, ArgumentType> * accessor)
		{
			if (accessor != nullptr){
				if (feature == nullptr){
					feature = utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_);
				}

				accessor->attachFeature(utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_););
			}
		}

	private:
		//! Granice do inicjalizacji
		utils::shared_ptr<UniformArgumentsFeature<ArgumentType>> feature;
		//! Warto�� pocz�tkowa argumentu
		const ArgumentType start_;
		//! Krok argument�w
		const ArgumentType step_;		
		//! Ilo�� pr�bek
		const std::size_t size_;
	};
	
	//! Klasa pomocnicza przy znajdowaniu argument�w otaczaj�cych dany argument w dyskretnym kanale
	class NearestArgumentsFinder
	{
	public:
		//! Typ zakresu indeks�w
		using Range = std::pair<std::size_t, std::size_t>;

	public:

		//! \tparam ArgumentType Typ argumentu o kt�ry pytamy
		//! \tparam ValueType Typ warto�ci
		template<typename ValueType, typename ArgumentType>
		//! \param accessor Kana� dyskretny w kt�rym szukamy otaczaj�cych pr�bek
		//! \param argument Argument wok� kt�rego szukamy pr�bek
		//! \return Zakres pr�bek otaczaj�cych dany argument (r�nica warto�ci indeks�w 1),
		//! kiedy pr�bka znajduje si� w kanale zakres o dw�ch identycznych indeksach (r�nica 0),
		//! kiedy pr�bka poza zakresem kana�u dowolny zakres indeks�w z r�nic� wieksz� ni� 1
		//! Ta implementacja dzia�a w czasie O(log(N)) - podzia� binarny bo dane sa u�o�one chronologicznie					
		static inline Range range(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType argument)
		{
			UTILS_COMPILER_WARNING("Unoptimized search function");
			std::size_t minIdx = 0;
			std::size_t maxIdx = accessor.size() - 1;

			while ((maxIdx - minIdx) > 1){
				const auto midIdx = (maxIdx + minIdx) >> 1;
				const auto midArgument = accessor.argument(midIdx);

				if (midArgument < argument){
					minIdx = midIdx + 1;
				}
				else if (midArgument > argument){
					maxIdx = midIdx - 1;
				}
				else{
					maxIdx = minIdx = midIdx;
					break;
				}
			}

			if ((minIdx == maxIdx) && (argument > accessor.argument(maxIdx) || argument < accessor.argument(minIdx))){
				minIdx = 1;
				maxIdx = 0;
			}

			return Range(minIdx, maxIdx);
		}

		//! \tparam ArgumentType Typ argumentu o kt�ry pytamy
		//! \tparam ValueType Typ warto�ci
		template<typename ValueType, typename ArgumentType>
		//! \param accessor Kana� dyskretny w kt�rym szukamy otaczaj�cych pr�bek
		//! \param argument Argument wok� kt�rego szukamy pr�bek
		//! \return Zakres pr�bek otaczaj�cych dany argument (r�nica warto�ci indeks�w 1),
		//! kiedy pr�bka znajduje si� w kanale zakres o dw�ch identycznych indeksach (r�nica 0),
		//! kiedy pr�bka poza zakresem kana�u dowolny zakres indeks�w z r�nic� wieksz� ni� 1
		static inline Range range(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType argument, const ArgumentType interval)
		{
			const std::size_t idx = (argument - accessor.argument(0)) / interval;
			const auto idxArgument = accessor.argument(idx);
			if (idxArgument == argument){
				return Range(idx, idx);
			}

			if (argument > accessor.argument(idx + 1) || argument < accessor.argument(idx)){
				return Range(1, 0);
			}

			return Range(idx, idx + 1);
		}
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam ArgumentsGenerator Generator argument�w dla kolejnych indeks�w
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator<ArgumentType>>
	//! Klasa realizuje dyskretny dost�p do ci�g�ego kana�u danych opisuj�cego funkcj�
	class FunctionDiscreteAccessorAdapter : public IOptimizedDiscreteAccessorT<ValueType, ArgumentType>,
		private ArgumentsGenerator
	{
	public:

		//! \param accessor Kana� wrapowany		
		//! \param argumentsGenerator Generator argument�w
		FunctionDiscreteAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentsGenerator & argumentsGenerator = ArgumentsGenerator())
			: ArgumentsGenerator(argumentsGenerator), accessor(accessor)
		{

		}

		//! \param accessor Kana� wrapowany		
		//! \param argumentsGenerator Generator argument�w
		FunctionDiscreteAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			ArgumentsGenerator && argumentsGenerator)
			: ArgumentsGenerator(std::move(argumentsGenerator)), accessor(accessor)
		{

		}

		//! Destruktor wirtualny
		virtual ~FunctionDiscreteAccessorAdapter() {}		

		//! \param idx Indeks pr�bki
		//! \return Pr�bka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{			
			return accessor.sample(ArgumentsGenerator::argument(idx));
		}

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return ArgumentsGenerator::size(); }

	private:
		//! Kana� ci�g�y
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam ArgumentsGenerator Generator argument�w dla kolejnych indeks�w
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator < ArgumentType >>
		//! Klasa realizuje dyskretny dost�p do ci�g�ego kana�u danych opisuj�cego funkcj�
		using SafeFunctionDiscreteAccessorAdapter = SafeAccessorWrapper < IFunctionAccessorT<ValueType, ArgumentType>,
		FunctionDiscreteAccessorAdapter < ValueType, ArgumentType, ArgumentsGenerator >> ;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam Interpolator Obiekt realizuj�cy interpolacj� pomi�dzy pr�bkami kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator<ValueType>>
	//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
	class DiscreteFunctionAccessorAdapter : public IGeneratedFunctionAccessorT<ValueType, ArgumentType>,
		private Interpolator, private Extrapolator
	{
	public:
		//! \param accessor Kana� wrapowany
		//! \param interpolator Obiekt interpoluj�cy wewn�trz kana�u
		//! \param extrapolator Obiekt ekstrapoluj�cy poza kana�em
		DiscreteFunctionAccessorAdapter(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			Interpolator interpolator = Interpolator(), Extrapolator extrapolator = Extrapolator())
			: Interpolator(interpolator), Extrapolator(extrapolator), accessor(accessor),
			range(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessorT<ValueType, ArgumentType>&,
			const ArgumentType)>(&NearestArgumentsFinder::range<ValueType, ArgumentType>))
		{
			if (accessor.getOrCreateFeature<IFunctionFeature>() == nullptr){
				throw std::runtime_error("Non function data");
			}

			feature = accessor.getOrCreateFeature<IBoundedArgumentsFeature>();

			auto uaf = accessor.getOrCreateFeature<IUniformArgumentsFeature>();
			if (uaf != nullptr){
				range = std::bind(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessorT<ValueType, ArgumentType>&,
					const ArgumentType, const ArgumentType)>(&NearestArgumentsFinder::range<ValueType, ArgumentType>),
					std::placeholders::_1, std::placeholders::_2, uaf->argumentsInterval());
			}
		}

		//! Destruktor wirtualny
		virtual ~DiscreteFunctionAccessorAdapter() {}

		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		ValueType value(const ArgumentType & argument) const override
		{
			if (feature != nullptr){

				if (argument < feature->minArgument()){
					return Extrapolator::argumentUnderflow(argument, accessor);
				}
				else if (argument > feature->maxArgument()){
					return Extrapolator::argumentOverflow(argument, accessor);
				}
				else{
					const auto r = range(accessor, argument);
					if (r.first == r.second){
						return accessor.value(r.first);
					}
					else{
						return Interpolator::interpolate(accessor, argument, r.first, r.second);
					}
				}
			}
			else if (accessor.empty() == false){
				const auto arg = accessor.argument(0);
				if (argument < arg){
					return Extrapolator::argumentUnderflow(argument, accessor);
				}
				else if (argument > arg){
					return Extrapolator::argumentOverflow(argument, accessor);
				}
				else{
					return accessor.value(0);
				}
			}
			else{
				throw std::range_error("Empty accessor");
			}

			return ValueType();
		}		

	private:
		utils::shared_ptr<IBoundedArgumentsFeature<ArgumentType>> feature;
		//! Wrapowany kana� danych
		const IDiscreteAccessorT<ValueType, ArgumentType> & accessor;
		//! Funkcja realizuj�ca pobieranie najbli�szych indeks�w
		std::function<NearestArgumentsFinder::Range(const IDiscreteAccessorT<ValueType, ArgumentType> &, const ArgumentType & argument)> range;
	};

	//! \tparam ChannelType Typ kana�u do zmiany reprezentacji dost�pu do danych
	//! \tparam Interpolator Obiekt realizuj�cy interpolacj� pomi�dzy pr�bkami kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator < ValueType >>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeDiscreteFunctionAccessorAdapter = SafeAccessorWrapper < IDiscreteAccessorT<ValueType, ArgumentType>,
		DiscreteFunctionAccessorAdapter < ValueType, ArgumentType, Interpolator, Extrapolator >> ;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla dyskretnych akcesor�w, zaw�aj�cy ich zakres
	class DiscreteSubAccessorAdapter : public IDiscreteAccessorT<ValueType, ArgumentType>,
		private Extrapolator
	{
	private:

		void init()
		{
			if (start + size > accessor.size()){
				throw std::runtime_error("Invalid discrete subaccessor configuration");
			}

			auto ff = accessor.getOrCreateFeature<IFunctionFeature>();
			if (ff != nullptr){

				//TODO
				//zweryfikowa� czy mog� to potraktowa� dok�adnie tak samo czy musze sprawdzi� ilo�c pr�bek
				//i parzysto�� czy nieparzysto�� ze wzgl�du na to aktualizowa�
				attachFeature(ff);

				if (ff->monotony() != NonMonotonic){
					auto minSample = accessor.sample(start);
					auto maxSample = accessor.sample(start + size_);
					auto resArg = std::minmax(minSample.first, maxSample.first);
					auto resVal = std::minmax(minSample.second, maxSample.second);
					attachFeature(IFeaturePtr(new BoundedArgumentsFeature(resArg.first, resArg.second)));
					attachFeature(IFeaturePtr(new BoundedValuesFeature(resVal.first, resVal.second)));
				}
			}
		}

	public:
		//! \param accessor Wrapowany accessor
		//! \param start Index startowy
		//! \param size Ilo�c pr�bek od indeksu startowego
		//! \param extrapolator
		DiscreteSubAccessorAdapter(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const std::size_t start, const std::size_t size,
			const Extrapolator & extrapolator = Extrapolator())	:
			Extrapolator(extrapolator), accessor(accessor),
			start(start), size_(size)
		{
			init();
		}

		//! \param accessor Wrapowany accessor
		//! \param start Index startowy
		//! \param size Ilo�c pr�bek od indeksu startowego
		//! \param extrapolator
		DiscreteSubAccessorAdapter(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const std::size_t start, const std::size_t size, Extrapolator && extrapolator)
			: Extrapolator(std::move(extrapolator)), accessor(accessor), start(start),
			size_(size)
		{
			init();
		}

		//! Destruktor wirtualny
		virtual ~DiscreteSubAccessorAdapter() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (ridx >= accessor.size()){
				throw std::out_of_range("Argument out of range");
			}

			return accessor.argument(ridx);
		}

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (ridx >= accessor.size()){
				throw std::out_of_range("Argument out of range");
			}

			if (idx >= size_){
				return Extrapolator::argumentOverflow(accessor.argument(ridx), accessor);
			}
			else{
				return accessor.value(ridx);
			}		
		}

		//! \param idx Indeks pr�bki
		//! \return Pr�bka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (idx < size_){
				return accessor.sample(ridx);
			}
			else if (ridx < accessor.size()){
				return{ accessor.argument(ridx), ((idx < size_) ? accessor.value(ridx) : Extrapolator::argumentOverflow(accessor.argument(ridx), accessor)) };
			}
			else{
				throw std::out_of_range("Argument out of range");
			}
		}

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return size_; }

	private:

		//! \param idx Indeks o jaki pytamy w tym kanale
		//! \return Mapowanie naszego indeksu na indeks wrapowanego kana�u
		inline std::size_t realIdx(const std::size_t idx) const { return start + idx; }

	private:
		//! Wrapowany kana�
		const IDiscreteAccessorT<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const std::size_t start;
		//! Ilo�� pr�bek
		const std::size_t size_;		
	};

	//! \tparam ChannelType Typ kana�u do zmiany reprezentacji dost�pu do danych
	//! \tparam Interpolator Obiekt realizuj�cy interpolacj� pomi�dzy pr�bkami kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeDiscreteSubAccessorAdapter = SafeAccessorWrapper < IDiscreteAccessorT<ValueType, ArgumentType>,
		DiscreteSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >> ;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
	typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla ci�g�ych akcesor�w, zaw�aj�cy ich zakres
	class FunctionSubAccessorAdapter : public IFunctionAccessorT<ValueType, ArgumentType>,
		private Extrapolator
	{
	private:

		void init()
		{
			if (start > end){
				throw std::invalid_argument("Invalid arguments for function subaccessor range");
			}

			auto ff = accessor.getOrCreateFeature<IFunctionFeature>();
			//TODO
			//zweryfikowa� czy mog� to potraktowa� dok�adnie tak samo czy musze sprawdzi� ilo�c pr�bek
			//i parzysto�� czy nieparzysto�� ze wzgl�du na to aktualizowa�
			attachFeature(ff);
			if (ff->monotony() != NonMonotonic){
				auto minSample = accessor.sample(start);
				auto maxSample = accessor.sample(end);
				auto resArg = std::minmax(minSample.first, maxSample.first);
				auto resVal = std::minmax(minSample.second, maxSample.second);
				attachFeature(IFeaturePtr(new BoundedArgumentsFeature(resArg.first, resArg.second)));
				attachFeature(IFeaturePtr(new BoundedValuesFeature(resVal.first, resVal.second)));
			}
			else{
				//TODO
				//jesli dodano valueboud lub argument bound to dodajemy po aktualizacji
			}
		}

	public:
		//! \param accessor Wrapowany accessor
		//! \param start Argument od kt�rego zaczynamy
		//! \param end Argument na kt�rym 
		//! \param extrapolator Extrapolator dla brzegowych warto�ci argument�w
		FunctionSubAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			const Extrapolator & extrapolator = Extrapolator())
			: Extrapolator(extrapolator), accessor(accessor), start(start), end(end)
		{
			init();
		}

		//! \param accessor Wrapowany accessor
		//! \param start Argument od kt�rego zaczynamy
		//! \param end Argument na kt�rym 
		//! \param extrapolator Extrapolator dla brzegowych warto�ci argument�w
		FunctionSubAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			Extrapolator && extrapolator)
			: Extrapolator(std::move(extrapolator)), accessor(accessor), start(start), end(end)
		{
			init();
		}

		//! Destruktor wirtualny
		virtual ~FunctionSubAccessorAdapter() {}

		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override
		{
			if (argument < start){
				return Extrapolator::argumentUnderflow(argument, accessor);
			}
			else if (argument > end){
				return Extrapolator::argumentOverflow(argument, accessor);
			}
			else{
				return accessor.value(argument);
			}		
		}
		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Pr�bka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const override
		{
			if (argument < start){
				return { argument, Extrapolator::argumentUnderflow(argument, accessor) };
			}
			else if (argument > end){
				return{ argument, Extrapolator::argumentOverflow(argument, accessor) };
			}
			else{
				return accessor.sample(argument);
			}
		}

	private:
		//! Wrapowany kana�
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const ArgumentType start;
		//! Ilo�� pr�bek
		const ArgumentType end;
	};

	//! \tparam ValueType Typ warto�ci
	//! \tparam ArgumentType Typ argument�w	
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeFunctionSubAccessorAdapter = SafeAccessorWrapper < IFunctionAccessorT<ValueType, ArgumentType>,
		FunctionSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >> ;

	//! Struktura pomocnicza realizuj�ca prze�roczysto�� argument�w
	struct TransparentArgumentExtractor
	{
		//! \tparam Typ argumentu
		template<typename ArgumentType>
		//! \param Modyfikowany (przepuszczany bez zmian) argument
		//! \return Argument bez zmian
		static inline const ArgumentType & extract(const ArgumentType & argument, ...) { return argument };
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor, typename ArgumentExtractor = TransparentArgumentExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
	//! Wrapper dla dyskretnych akcesor�w, zmieniaj�cy ich reprezentacj�
	class DiscreteAccessorAdapter : public IIndependentDiscreteAccessorT<DestValueType, DestArgumentType>,
		private ValueExtractor, private ArgumentExtractor
	{
	public:

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: ValueExtractor(valueExtractor), ArgumentExtractor(argumentExtractor),
			accessor(accessor)
		{

		}

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			ValueExtractor && valueExtractor, ArgumentExtractor && argumentExtractor)
			: ValueExtractor(std::move(valueExtractor)), ArgumentExtractor(std::move(argumentExtractor)),
			accessor(accessor)
		{

		}

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			const ValueExtractor & valueExtractor,
			const ArgumentExtractor && argumentExtractor)
			: ValueExtractor(valueExtractor), ArgumentExtractor(std::move(argumentExtractor)),
			accessor(accessor)
		{

		}

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			ValueExtractor && valueExtractor, const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: ValueExtractor(std::move(valueExtractor)), ArgumentExtractor(argumentExtractor),
			accessor(accessor)
		{

		}

		//! Destruktor wirtualny
		virtual ~DiscreteAccessorAdapter() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentExtractor::extract(accessor.argument(idx), idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return ValueExtractor::extract(accessor.value(idx), idx); }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return accessor.size(); }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return accessor.empty(); }

	private:
		//! Accessor
		const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor;		
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor, typename ArgumentExtractor = TransparentArgumentExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeDiscreteAccessorAdapter = SafeAccessorWrapper < IDiscreteAccessorT<DestValueType, DestArgumentType>,
		DiscreteAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, ArgumentExtractor, DestValueType, DestArgumentType >> ;

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType,	typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), std::declval<BaseArgumentType>()))>::type>
		//! Wrapper dla funkcji, zmieniaj�cy ich reprezentacj�
	class FunctionAccessorAdapter : public IGeneratedFunctionAccessorT<DestValueType, BaseArgumentType>,
		private ValueExtractor
	{
	public:

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		FunctionAccessorAdapter(const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor())
			: ValueExtractor(valueExtractor), accessor(accessor)
		{

		}

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		FunctionAccessorAdapter(const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor,
			ValueExtractor && valueExtractor)
			: ValueExtractor(std::move(valueExtractor)), accessor(accessor)
		{

		}

		//! Destruktor wirtualny
		virtual ~FunctionAccessorAdapter() {}

		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return ValueExtractor::extract(accessor.value(argument), argument); }		

	private:
		//! Accessor
		const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor;		
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type>
		//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
		using SafeFunctionAccessorAdapter = SafeAccessorWrapper < IFunctionAccessorT<BaseValueType, BaseArgumentType>,
		FunctionAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, DestValueType>> ;

	//! \tparam Idx Indeks jaki chcemy wyci�gn��
	template<std::size_t Idx>
	//! Struktura pomocnicza przy wyci�ganiu warto�ci zamiast ca�ych wektor�w
	struct StaticVectorElementExtractor
	{
		//! \tparam T Typ wektora kt�rego elementy wyci�gamy
		template<typename T>
		//! \param value Rozpakowywana warto�� wektora
		//! \param ... Na potrzeby innych klas
		//! \return Element wektora
		static inline auto extract(const T & value, ...) -> decltype(value[Idx])
		{
			return value[Idx];
		}
	};

	//! Klasa pozwalaj�ca wyci�ga� warto�ci wektor�w zamiast ca�ych wektor�w
	class VectorElementExtractor
	{
	public:
		//! \param idx Wypakowywany indeks
		VectorElementExtractor(const std::size_t idx) : idx(idx) {}
		//! Destruktor
		~VectorElementExtractor() {}

		//! \tparam T Typ wektora kt�rego elementy wyci�gamy
		template<typename T>
		//! \param value Rozpakowywana warto�� wektora
		//! \return Element wektora
		auto extract(const T & value, ...) -> decltype(value[idx])
		{
			return value[idx];
		}

	protected:
		//! Wyci�gany indeks
		const std::size_t idx;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__ADAPTER_H__