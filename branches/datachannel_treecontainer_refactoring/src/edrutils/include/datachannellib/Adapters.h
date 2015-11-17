/********************************************************************
	created:  2015/07/28	15:50:12
	filename: Adapters.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__ADAPTERS_H__
#define __HEADER_GUARD_DATACHANNEL__ADAPTERS_H__

#include <limits>
#include <functional>
#include <datachannellib/Accessors.h>
#include <datachannellib/IUniformArgumentsFeature.h>
#include <datachannellib/IBoundedArgumentsFeature.h>
#include <datachannellib/IBoundedValuesFeature.h>
#include <datachannellib/IFunctionFeature.h>
#include <datachannellib/Interpolators.h>
#include <datachannellib/Extrapolators.h>

namespace datachannel
{
	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType>
	//! Klasa realizuj�ca generator argument�w
	class UniformArgumentsGenerator
	{
	public:
		//! \param step Krok z jakim generujemy argumenty
		//! \param start Warto�c argumentu od kt�rego zaczynamy
		//! \param ned Warto�c argumentu na kt�rej ko�czymy!!
		UniformArgumentsGenerator(const ArgumentType step,
			const ArgumentType start = ArgumentType(0),
			const ArgumentType end = std::numeric_limits<ArgumentType>::max())
			: start_(start), end_(end), step_(step), limit(&std::min<ArgumentType>)
		{
			if ((step == 0) || (start == end) ||
				((start < end) && (step < 0)) ||
				((start > end) && (step > 0))){
				throw std::runtime_error("Wrong generator setup");
			}

			const auto s = std::abs((end - start) / step);		

			size_ = s;

			if (size_ == 0){
				throw std::runtime_error("Empty generator");
			}

			if (step < 0){
				limit = &std::max<ArgumentType>;
			}

			if (size_ < s){
				++size_;
			}
		}

		//! \param Other Kopiowany generator
		UniformArgumentsGenerator(const UniformArgumentsGenerator & Other)
			: start_(Other.start_), end_(Other.end_), size_(Other.size_),
			step_(Other.step_), feature(Other.feature), limit(Other.limit)
		{

		}

		//! \tparam T
		template<typename T>
		//! \param Other Kopiowany generator
		UniformArgumentsGenerator(const UniformArgumentsGenerator<T> & Other)
			: start_(Other.start_), end_(Other.end_), size_(Other.size_),
			step_(Other.step_), feature(Other.feature), limit(Other.limit)
		{

		}

		//! \param Przenoszony generator
		UniformArgumentsGenerator(UniformArgumentsGenerator && Other)
			: start_(Other.start_), end_(Other.end_), size_(Other.size_),
			step_(Other.step_), feature(std::move(Other.feature)),
			limit(Other.limit)
		{
			Other.start_ = Other.end_ = Other.step_ = ArgumentType();
			Other.size_ = 0;
			Other.limit = nullptr;
		}

		//! Destruktor
		~UniformArgumentsGenerator() {}

		//! \param idx Index dla kt�rego generujemy argument
		inline ArgumentType argument(const std::size_t idx) const
		{
			if (idx >= size_){
				throw std::range_error("Generator range exceeded");
			}

			return limit(start_ + idx * step_, end_);
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline ArgumentType start() const
		{
			return start_;
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline ArgumentType end() const
		{
			return end_;
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
		inline void init(IAccessor<ValueType, ArgumentType> * accessor)
		{
			if (accessor != nullptr){
				if (feature == nullptr){
					feature = utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_);
				}

				accessor->attachFeature(feature);
			}
		}

	private:
		//! Funkcja gwarantuj�ca nieprzekraczalno�� zakres�w start-end
		const ArgumentType& (*limit)(const ArgumentType &, const ArgumentType &);
		//! Granice do inicjalizacji
		utils::shared_ptr<UniformArgumentsFeature<ArgumentType>> feature;
		//! Warto�� pocz�tkowa argumentu
		const ArgumentType start_;
		//! Warto�� ko�cowa argumentu
		const ArgumentType end_;
		//! Krok argument�w
		const ArgumentType step_;

		//TODO
		//doda� kwalfikator const i inicjalizacja w konstruktorze
		//! Ilo�� pr�bek
		std::size_t size_;
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
		static inline Range range(const IDiscreteAccessor<ValueType, ArgumentType> & accessor,
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
		static inline Range range(const IDiscreteAccessor<ValueType, ArgumentType> & accessor,
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
	class FunctionDiscreteAccessorAdapter : public IOptimizedDiscreteAccessor<ValueType, ArgumentType>
	{
	public:

		//! \param accessor Kana� wrapowany		
		//! \param argumentsGenerator Generator argument�w
		FunctionDiscreteAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			const ArgumentsGenerator & argumentsGenerator = ArgumentsGenerator())
			: accessor(accessor), argumentsGenerator(argumentsGenerator)
		{

		}

		//! \param accessor Kana� wrapowany		
		//! \param argumentsGenerator Generator argument�w
		FunctionDiscreteAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			ArgumentsGenerator && argumentsGenerator)
			: accessor(accessor), argumentsGenerator(std::move(argumentsGenerator))
		{

		}

		//! Destruktor wirtualny
		virtual ~FunctionDiscreteAccessorAdapter() {}		

		//! \param idx Indeks pr�bki
		//! \return Pr�bka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{			
			return accessor.sample(argumentsGenerator.argument(idx));
		}

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return argumentsGenerator.size(); }

	private:
		//! Kana� ci�g�y
		const IFunctionAccessor<ValueType, ArgumentType> & accessor;
		//! Generator argument�w
		ArgumentsGenerator argumentsGenerator;
	};

	//! \tparam ChannelType Typ kana�u do zmiany reprezentacji dost�pu do danych
	//! \tparam Interpolator Obiekt realizuj�cy interpolacj� pomi�dzy pr�bkami kana�u
	//! \tparam Extrapolator Obiekt realizuj�cy polityk� zapyta� o argumenty spoza zakresu kana�u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator<ValueType>>
	//! Klasa realizuje dost�p ci�g�y dla kana��w dyskretnych
	class DiscreteFunctionAccessorAdapter : public IGeneratedFunctionAccessor<ValueType, ArgumentType>
	{
	public:
		//! \param accessor Kana� wrapowany
		//! \param interpolator Obiekt interpoluj�cy wewn�trz kana�u
		//! \param extrapolator Obiekt ekstrapoluj�cy poza kana�em
		DiscreteFunctionAccessorAdapter(const IDiscreteAccessor<ValueType, ArgumentType> & accessor,
			Interpolator interpolator = Interpolator(), Extrapolator extrapolator = Extrapolator())
			: accessor(accessor), interpolator(interpolator), extrapolator(extrapolator),
			range(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessor<ValueType, ArgumentType>&,
			const ArgumentType)>(&NearestArgumentsFinder::range<ValueType, ArgumentType>))
		{
			if (accessor.getOrCreateFeature<IFunctionFeature>() == nullptr){
				throw std::runtime_error("Non function data");
			}

			feature = accessor.getOrCreateArgumentFeature<IBoundedArgumentsFeature>();

			auto uaf = accessor.getOrCreateArgumentFeature<IUniformArgumentsFeature>();
			if (uaf != nullptr){
				range = std::bind(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessor<ValueType, ArgumentType>&,
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
					return extrapolator.argumentUnderflow(argument, accessor);
				}
				else if (argument > feature->maxArgument()){
					return extrapolator.argumentOverflow(argument, accessor);
				}
				else{
					const auto r = range(accessor, argument);
					if (r.first == r.second){
						return accessor.value(r.first);
					}
					else{
						return interpolator.interpolate(accessor, argument, r.first, r.second);
					}
				}
			}
			else if (accessor.empty() == false){
				const auto arg = accessor.argument(0);
				if (argument < arg){
					return extrapolator.argumentUnderflow(argument, accessor);
				}
				else if (argument > arg){
					return extrapolator.argumentOverflow(argument, accessor);
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
		const IDiscreteAccessor<ValueType, ArgumentType> & accessor;
		//! Obiekt interpoluj�cy
		Interpolator interpolator;
		//! Obiekt ekstrapoluj�cy
		Extrapolator extrapolator;
		//! Funkcja realizuj�ca pobieranie najbli�szych indeks�w
		std::function<NearestArgumentsFinder::Range(const IDiscreteAccessor<ValueType, ArgumentType> &, const ArgumentType & argument)> range;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla dyskretnych akcesor�w, zaw�aj�cy ich zakres
	class DiscreteSubAccessorAdapter : public IDiscreteAccessor<ValueType, ArgumentType>
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
		DiscreteSubAccessorAdapter(const IDiscreteAccessor<ValueType, ArgumentType> & accessor,
			const std::size_t start, const std::size_t size,
			const Extrapolator & extrapolator = Extrapolator())	: accessor(accessor),
			start(start), size_(size), extrapolator(extrapolator)
		{
			init();
		}

		//! \param accessor Wrapowany accessor
		//! \param start Index startowy
		//! \param size Ilo�c pr�bek od indeksu startowego
		//! \param extrapolator
		DiscreteSubAccessorAdapter(const IDiscreteAccessor<ValueType, ArgumentType> & accessor,
			const std::size_t start, const std::size_t size, Extrapolator && extrapolator)
			: accessor(accessor), start(start), size_(size), extrapolator(std::move(extrapolator))
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
				return extrapolator.argumentOverflow(accessor.argument(ridx), accessor);
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
				return{ accessor.argument(ridx), ((idx < size_) ? accessor.value(ridx) : extrapolator.argumentOverflow(accessor.argument(ridx), accessor)) };			
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
		const IDiscreteAccessor<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const std::size_t start;
		//! Ilo�� pr�bek
		const std::size_t size_;
		//! Extrapolator
		const Extrapolator extrapolator;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename ValueType, typename ArgumentType,
	typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla ci�g�ych akcesor�w, zaw�aj�cy ich zakres
	class FunctionSubAccessorAdapter : public IFunctionAccessor<ValueType, ArgumentType>
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
		FunctionSubAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			const Extrapolator & extrapolator = Extrapolator())
			: accessor(accessor), start(start), end(end), extrapolator(extrapolator)
		{
			init();
		}

		//! \param accessor Wrapowany accessor
		//! \param start Argument od kt�rego zaczynamy
		//! \param end Argument na kt�rym 
		//! \param extrapolator Extrapolator dla brzegowych warto�ci argument�w
		FunctionSubAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			Extrapolator && extrapolator)
			: accessor(accessor), start(start), end(end), extrapolator(std::move(extrapolator))
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
				return extrapolator.argumentUnderflow(argument, accessor);
			}
			else if (argument > end){
				return extrapolator.argumentOverflow(argument, accessor);
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
				return { argument, extrapolator.argumentUnderflow(argument, accessor) };
			}
			else if (argument > end){
				return{ argument, extrapolator.argumentOverflow(argument, accessor) };				
			}
			else{
				return accessor.sample(argument);
			}
		}

	private:
		//! Wrapowany kana�
		const IFunctionAccessor<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const ArgumentType start;
		//! Ilo�� pr�bek
		const ArgumentType end;
		//! Extrapolator
		const Extrapolator extrapolator;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename ArgumentType, typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>()))>::type>
	//! Wrapper dla dyskretnych akcesor�w, zmieniaj�cy ich reprezentacj�
	class DiscreteAccessorAdapter : public IIndependentDiscreteAccessor<DestValueType, ArgumentType>
	{
	public:

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		DiscreteAccessorAdapter(const IDiscreteAccessor<BaseValueType, ArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor())
			: accessor(accessor), valueExtractor(valueExtractor)
		{

		}

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		DiscreteAccessorAdapter(const IDiscreteAccessor<BaseValueType, ArgumentType> & accessor,
			ValueExtractor && valueExtractor)
			: accessor(accessor), valueExtractor(std::move(valueExtractor))
		{

		}

		//! Destruktor wirtualny
		virtual ~DiscreteAccessorAdapter() {}

		//! \param idx Indeks pr�bki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return accessor.argument(idx); }

		//! \param idx Indeks pr�bki
		//! \return Warto�� dla danego indeksu
		virtual value_type value(const size_type idx) const override { return valueExtractor.extract(accessor.value(idx)); }

		//! \return Ilo�� pr�bek w kanale
		virtual size_type size() const override { return accessor.size(); }

		//! \return Czy kana� nie zawiera danych
		virtual bool empty() const override { return accessor.empty(); }

	private:
		//! Accessor
		const IDiscreteAccessor<BaseValueType, ArgumentType> & accessor;
		//! Value extractor
		const ValueExtractor valueExtractor;
	};

	//! \tparam ValueType Typ warto�ci kana�u
	//! \tparam ArgumentType Typ argument�w kana�u
	template<typename BaseValueType, typename ArgumentType, typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>()))>::type>
		//! Wrapper dla funkcji, zmieniaj�cy ich reprezentacj�
	class FunctionAccessorAdapter : public IFunctionAccessor<DestValueType, ArgumentType>
	{
	public:

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		FunctionAccessorAdapter(const IFunctionAccessor<BaseValueType, ArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor())
			: accessor(accessor), valueExtractor(valueExtractor)
		{

		}

		//! \param accessor Kana� kt�ry adaptujemy
		//! \param valueExtractor Wyci�ga/Modyfikuje/Generuje nowe warto�ci na bazie warto�ci adaptowanego kana�u
		FunctionAccessorAdapter(const IFunctionAccessor<BaseValueType, ArgumentType> & accessor,
			ValueExtractor && valueExtractor)
			: accessor(accessor), valueExtractor(std::move(valueExtractor))
		{

		}

		//! Destruktor wirtualny
		virtual ~FunctionAccessorAdapter() {}

		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Warto�� dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return valueExtractor.extract(accessor.value(argument)); }
		//! \param argument Argument dla kt�ego odpytujemy o warto��
		//! \return Pr�bka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const { return{ argument, value(argument) }; }

	private:
		//! Accessor
		const IFunctionAccessor<BaseValueType, ArgumentType> & accessor;
		//! Value extractor
		const ValueExtractor valueExtractor;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__ADAPTERS_H__