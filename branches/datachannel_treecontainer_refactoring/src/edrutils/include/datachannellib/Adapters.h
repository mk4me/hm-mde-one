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
	//! Klasa realizuj¹ca generator argumentów
	class UniformArgumentsGenerator
	{
	public:
		//! \param step Krok z jakim generujemy argumenty
		//! \param start Wartoœc argumentu od którego zaczynamy
		//! \param ned Wartoœc argumentu na której koñczymy!!
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

		//! \param idx Index dla którego generujemy argument
		inline ArgumentType argument(const std::size_t idx) const
		{
			if (idx >= size_){
				throw std::range_error("Generator range exceeded");
			}

			return limit(start_ + idx * step_, end_);
		}

		//! \return Iloœc próbek do wygenerowania
		inline ArgumentType start() const
		{
			return start_;
		}

		//! \return Iloœc próbek do wygenerowania
		inline ArgumentType end() const
		{
			return end_;
		}

		//! \return Iloœc próbek do wygenerowania
		inline std::size_t size() const
		{
			return size_;
		}

		//! \return Krok argumentów
		inline ArgumentType step() const
		{
			return step_;
		}

		//! \param ValueType Typ wartoœci kana³u
		template<typename ValueType>
		//! \param accessor Akcesor który zasilamy t¹ cech¹
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
		//! Funkcja gwarantuj¹ca nieprzekraczalnoœæ zakresów start-end
		const ArgumentType& (*limit)(const ArgumentType &, const ArgumentType &);
		//! Granice do inicjalizacji
		utils::shared_ptr<UniformArgumentsFeature<ArgumentType>> feature;
		//! Wartoœæ pocz¹tkowa argumentu
		const ArgumentType start_;
		//! Wartoœæ koñcowa argumentu
		const ArgumentType end_;
		//! Krok argumentów
		const ArgumentType step_;

		//TODO
		//dodaæ kwalfikator const i inicjalizacja w konstruktorze
		//! Iloœæ próbek
		std::size_t size_;
	};
	
	//! Klasa pomocnicza przy znajdowaniu argumentów otaczaj¹cych dany argument w dyskretnym kanale
	class NearestArgumentsFinder
	{
	public:
		//! Typ zakresu indeksów
		using Range = std::pair<std::size_t, std::size_t>;

	public:

		//! \tparam ArgumentType Typ argumentu o który pytamy
		//! \tparam ValueType Typ wartoœci
		template<typename ValueType, typename ArgumentType>
		//! \param accessor Kana³ dyskretny w którym szukamy otaczaj¹cych próbek
		//! \param argument Argument wokó³ którego szukamy próbek
		//! \return Zakres próbek otaczaj¹cych dany argument (ró¿nica wartoœci indeksów 1),
		//! kiedy próbka znajduje siê w kanale zakres o dwóch identycznych indeksach (ró¿nica 0),
		//! kiedy próbka poza zakresem kana³u dowolny zakres indeksów z ró¿nic¹ wieksz¹ niæ 1
		//! Ta implementacja dzia³a w czasie O(log(N)) - podzia³ binarny bo dane sa u³o¿one chronologicznie					
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

		//! \tparam ArgumentType Typ argumentu o który pytamy
		//! \tparam ValueType Typ wartoœci
		template<typename ValueType, typename ArgumentType>
		//! \param accessor Kana³ dyskretny w którym szukamy otaczaj¹cych próbek
		//! \param argument Argument wokó³ którego szukamy próbek
		//! \return Zakres próbek otaczaj¹cych dany argument (ró¿nica wartoœci indeksów 1),
		//! kiedy próbka znajduje siê w kanale zakres o dwóch identycznych indeksach (ró¿nica 0),
		//! kiedy próbka poza zakresem kana³u dowolny zakres indeksów z ró¿nic¹ wieksz¹ niæ 1
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

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam ArgumentsGenerator Generator argumentów dla kolejnych indeksów
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator<ArgumentType>>
	//! Klasa realizuje dyskretny dostêp do ci¹g³ego kana³u danych opisuj¹cego funkcjê
	class FunctionDiscreteAccessorAdapter : public IOptimizedDiscreteAccessor<ValueType, ArgumentType>
	{
	public:

		//! \param accessor Kana³ wrapowany		
		//! \param argumentsGenerator Generator argumentów
		FunctionDiscreteAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			const ArgumentsGenerator & argumentsGenerator = ArgumentsGenerator())
			: accessor(accessor), argumentsGenerator(argumentsGenerator)
		{

		}

		//! \param accessor Kana³ wrapowany		
		//! \param argumentsGenerator Generator argumentów
		FunctionDiscreteAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			ArgumentsGenerator && argumentsGenerator)
			: accessor(accessor), argumentsGenerator(std::move(argumentsGenerator))
		{

		}

		//! Destruktor wirtualny
		virtual ~FunctionDiscreteAccessorAdapter() {}		

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{			
			return accessor.sample(argumentsGenerator.argument(idx));
		}

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return argumentsGenerator.size(); }

	private:
		//! Kana³ ci¹g³y
		const IFunctionAccessor<ValueType, ArgumentType> & accessor;
		//! Generator argumentów
		ArgumentsGenerator argumentsGenerator;
	};

	//! \tparam ChannelType Typ kana³u do zmiany reprezentacji dostêpu do danych
	//! \tparam Interpolator Obiekt realizuj¹cy interpolacjê pomiêdzy próbkami kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator<ValueType>>
	//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
	class DiscreteFunctionAccessorAdapter : public IGeneratedFunctionAccessor<ValueType, ArgumentType>
	{
	public:
		//! \param accessor Kana³ wrapowany
		//! \param interpolator Obiekt interpoluj¹cy wewn¹trz kana³u
		//! \param extrapolator Obiekt ekstrapoluj¹cy poza kana³em
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

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
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
		//! Wrapowany kana³ danych
		const IDiscreteAccessor<ValueType, ArgumentType> & accessor;
		//! Obiekt interpoluj¹cy
		Interpolator interpolator;
		//! Obiekt ekstrapoluj¹cy
		Extrapolator extrapolator;
		//! Funkcja realizuj¹ca pobieranie najbli¿szych indeksów
		std::function<NearestArgumentsFinder::Range(const IDiscreteAccessor<ValueType, ArgumentType> &, const ArgumentType & argument)> range;
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla dyskretnych akcesorów, zawê¿aj¹cy ich zakres
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
				//zweryfikowaæ czy mogê to potraktowaæ dok³adnie tak samo czy musze sprawdziæ iloœc próbek
				//i parzystoœæ czy nieparzystoœæ ze wzglêdu na to aktualizowaæ
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
		//! \param size Iloœc próbek od indeksu startowego
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
		//! \param size Iloœc próbek od indeksu startowego
		//! \param extrapolator
		DiscreteSubAccessorAdapter(const IDiscreteAccessor<ValueType, ArgumentType> & accessor,
			const std::size_t start, const std::size_t size, Extrapolator && extrapolator)
			: accessor(accessor), start(start), size_(size), extrapolator(std::move(extrapolator))
		{
			init();
		}

		//! Destruktor wirtualny
		virtual ~DiscreteSubAccessorAdapter() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (ridx >= accessor.size()){
				throw std::out_of_range("Argument out of range");
			}

			return accessor.argument(ridx);
		}

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
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

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
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

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return size_; }

	private:

		//! \param idx Indeks o jaki pytamy w tym kanale
		//! \return Mapowanie naszego indeksu na indeks wrapowanego kana³u
		inline std::size_t realIdx(const std::size_t idx) const { return start + idx; }

	private:
		//! Wrapowany kana³
		const IDiscreteAccessor<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const std::size_t start;
		//! Iloœæ próbek
		const std::size_t size_;
		//! Extrapolator
		const Extrapolator extrapolator;
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename ValueType, typename ArgumentType,
	typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla ci¹g³ych akcesorów, zawê¿aj¹cy ich zakres
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
			//zweryfikowaæ czy mogê to potraktowaæ dok³adnie tak samo czy musze sprawdziæ iloœc próbek
			//i parzystoœæ czy nieparzystoœæ ze wzglêdu na to aktualizowaæ
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
		//! \param start Argument od którego zaczynamy
		//! \param end Argument na którym 
		//! \param extrapolator Extrapolator dla brzegowych wartoœci argumentów
		FunctionSubAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			const Extrapolator & extrapolator = Extrapolator())
			: accessor(accessor), start(start), end(end), extrapolator(extrapolator)
		{
			init();
		}

		//! \param accessor Wrapowany accessor
		//! \param start Argument od którego zaczynamy
		//! \param end Argument na którym 
		//! \param extrapolator Extrapolator dla brzegowych wartoœci argumentów
		FunctionSubAccessorAdapter(const IFunctionAccessor<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			Extrapolator && extrapolator)
			: accessor(accessor), start(start), end(end), extrapolator(std::move(extrapolator))
		{
			init();
		}

		//! Destruktor wirtualny
		virtual ~FunctionSubAccessorAdapter() {}

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
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
		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Próbka dla zadanego argumentu
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
		//! Wrapowany kana³
		const IFunctionAccessor<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const ArgumentType start;
		//! Iloœæ próbek
		const ArgumentType end;
		//! Extrapolator
		const Extrapolator extrapolator;
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename ArgumentType, typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>()))>::type>
	//! Wrapper dla dyskretnych akcesorów, zmieniaj¹cy ich reprezentacjê
	class DiscreteAccessorAdapter : public IIndependentDiscreteAccessor<DestValueType, ArgumentType>
	{
	public:

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		DiscreteAccessorAdapter(const IDiscreteAccessor<BaseValueType, ArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor())
			: accessor(accessor), valueExtractor(valueExtractor)
		{

		}

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		DiscreteAccessorAdapter(const IDiscreteAccessor<BaseValueType, ArgumentType> & accessor,
			ValueExtractor && valueExtractor)
			: accessor(accessor), valueExtractor(std::move(valueExtractor))
		{

		}

		//! Destruktor wirtualny
		virtual ~DiscreteAccessorAdapter() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return accessor.argument(idx); }

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual value_type value(const size_type idx) const override { return valueExtractor.extract(accessor.value(idx)); }

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return accessor.size(); }

		//! \return Czy kana³ nie zawiera danych
		virtual bool empty() const override { return accessor.empty(); }

	private:
		//! Accessor
		const IDiscreteAccessor<BaseValueType, ArgumentType> & accessor;
		//! Value extractor
		const ValueExtractor valueExtractor;
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename ArgumentType, typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>()))>::type>
		//! Wrapper dla funkcji, zmieniaj¹cy ich reprezentacjê
	class FunctionAccessorAdapter : public IFunctionAccessor<DestValueType, ArgumentType>
	{
	public:

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		FunctionAccessorAdapter(const IFunctionAccessor<BaseValueType, ArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor())
			: accessor(accessor), valueExtractor(valueExtractor)
		{

		}

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		FunctionAccessorAdapter(const IFunctionAccessor<BaseValueType, ArgumentType> & accessor,
			ValueExtractor && valueExtractor)
			: accessor(accessor), valueExtractor(std::move(valueExtractor))
		{

		}

		//! Destruktor wirtualny
		virtual ~FunctionAccessorAdapter() {}

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return valueExtractor.extract(accessor.value(argument)); }
		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Próbka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const { return{ argument, value(argument) }; }

	private:
		//! Accessor
		const IFunctionAccessor<BaseValueType, ArgumentType> & accessor;
		//! Value extractor
		const ValueExtractor valueExtractor;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__ADAPTERS_H__