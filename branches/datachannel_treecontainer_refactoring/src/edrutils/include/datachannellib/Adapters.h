/********************************************************************
	created:  2015/07/28	15:50:12
	filename: Adapter.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__ADAPTER_H__
#define __HEADER_GUARD_DATACHANNEL__ADAPTER_H__

#include <limits>
#include <functional>
#include <utils/ValueCarrier.h>
#include <datachannellib/Accessors.h>
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
	//! Klasa realizuj¹ca generator argumentów
	class UniformArgumentsGenerator
	{
	private:

		using Func = const ArgumentType&(*)(const ArgumentType &, const ArgumentType &);

		static inline std::size_t init(const ArgumentType step,
			const ArgumentType start, const ArgumentType end)
		{
			if ((step == ArgumentType(0)) || (start == end) ||
				((start < end) && (step < ArgumentType(0))) ||
				((start > end) && (step > ArgumentType(0)))) {
				throw std::runtime_error("Wrong generator setup");
			}

			auto s = std::abs((end - start) / step);

			if (s == 0) {
				throw std::runtime_error("Empty generator");
			}

			std::size_t size = s;

			if (size < s * step) {
				size += ArgumentType(1);
			}

			return size;
		}

	public:
		//! \param step Krok z jakim generujemy argumenty
		//! \param start Wartoœc argumentu od którego zaczynamy
		//! \param ned Wartoœc argumentu na której koñczymy!!
		UniformArgumentsGenerator(const ArgumentType step,
			const ArgumentType start = ArgumentType(0),
			const ArgumentType end = std::numeric_limits<ArgumentType>::max())
			: start_(start), end_(end), step_(step),
			size_(init(step, start, end)),
			limit((step > ArgumentType(0)) ? static_cast<Func>(std::min<ArgumentType>) : static_cast<Func>(std::max<ArgumentType>))
		{
			
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
		inline void init(IAccessorT<ValueType, ArgumentType> * accessor)
		{
			if (accessor != nullptr){
				if (feature == nullptr){
					feature = utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_);
				}

				accessor->attachFeature(feature);
			}
		}

	private:
		//! Granice do inicjalizacji
		utils::shared_ptr<UniformArgumentsFeature<ArgumentType>> feature;
		//! Wartoœæ pocz¹tkowa argumentu
		const ArgumentType start_;
		//! Wartoœæ koñcowa argumentu
		const ArgumentType end_;
		//! Krok argumentów
		const ArgumentType step_;		
		//! Iloœæ próbek
		const std::size_t size_;
		//! Funkcja gwarantuj¹ca nieprzekraczalnoœæ zakresów start-end
		const ArgumentType& (*limit)(const ArgumentType &, const ArgumentType &);
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

		//! \tparam ArgumentType Typ argumentu o który pytamy
		//! \tparam ValueType Typ wartoœci
		template<typename ValueType, typename ArgumentType>
		//! \param accessor Kana³ dyskretny w którym szukamy otaczaj¹cych próbek
		//! \param argument Argument wokó³ którego szukamy próbek
		//! \return Zakres próbek otaczaj¹cych dany argument (ró¿nica wartoœci indeksów 1),
		//! kiedy próbka znajduje siê w kanale zakres o dwóch identycznych indeksach (ró¿nica 0),
		//! kiedy próbka poza zakresem kana³u dowolny zakres indeksów z ró¿nic¹ wieksz¹ niæ 1
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

	template<typename Accessor, typename Impl>
	class SafeAccessorAdapter : public Impl, private utils::ValueCarrier<Accessor>
	{
	public:
		template<typename U, class... Args>
		SafeAccessorAdapter(const U & accessor,	Args&&... arguments)
			: utils::ValueCarrier<Accessor>(accessor),
			Impl(utils::ValueCarrier<Accessor>::ref(), std::move(arguments)...)
		{
		}

		template<typename U, class... Args>
		SafeAccessorAdapter(const U & accessor, Args... arguments)
			: utils::ValueCarrier<Accessor>(accessor),
			Impl(utils::ValueCarrier<Accessor>::ref(), std::forward<Args>(arguments)...)
		{
		}		

		//! Destruktor wirtualny
		virtual ~SafeAccessorAdapter() {}
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam ArgumentsGenerator Generator argumentów dla kolejnych indeksów
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator<ArgumentType>>
	//! Klasa realizuje dyskretny dostêp do ci¹g³ego kana³u danych opisuj¹cego funkcjê
	class FunctionDiscreteAccessorAdapter : public IOptimizedDiscreteAccessorT<ValueType, ArgumentType>
	{
	public:

		//! \param accessor Kana³ wrapowany		
		//! \param argumentsGenerator Generator argumentów
		FunctionDiscreteAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentsGenerator & argumentsGenerator = ArgumentsGenerator())
			: accessor(accessor), argumentsGenerator(argumentsGenerator)
		{

		}

		//! \param accessor Kana³ wrapowany		
		//! \param argumentsGenerator Generator argumentów
		FunctionDiscreteAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
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
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
		//! Generator argumentów
		ArgumentsGenerator argumentsGenerator;
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam ArgumentsGenerator Generator argumentów dla kolejnych indeksów
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator < ArgumentType >>
		//! Klasa realizuje dyskretny dostêp do ci¹g³ego kana³u danych opisuj¹cego funkcjê
		using SafeFunctionDiscreteAccessorAdapter = SafeAccessorAdapter < IFunctionAccessorT<ValueType, ArgumentType>,
		FunctionDiscreteAccessorAdapter < ValueType, ArgumentType, ArgumentsGenerator >> ;

	//! \tparam ChannelType Typ kana³u do zmiany reprezentacji dostêpu do danych
	//! \tparam Interpolator Obiekt realizuj¹cy interpolacjê pomiêdzy próbkami kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator<ValueType>>
	//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
	class DiscreteFunctionAccessorAdapter : public IGeneratedFunctionAccessorT<ValueType, ArgumentType>,
		private Interpolator, private Extrapolator
	{
	public:
		//! \param accessor Kana³ wrapowany
		//! \param interpolator Obiekt interpoluj¹cy wewn¹trz kana³u
		//! \param extrapolator Obiekt ekstrapoluj¹cy poza kana³em
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

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
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
		//! Wrapowany kana³ danych
		const IDiscreteAccessorT<ValueType, ArgumentType> & accessor;
		//! Funkcja realizuj¹ca pobieranie najbli¿szych indeksów
		std::function<NearestArgumentsFinder::Range(const IDiscreteAccessorT<ValueType, ArgumentType> &, const ArgumentType & argument)> range;
	};

	//! \tparam ChannelType Typ kana³u do zmiany reprezentacji dostêpu do danych
	//! \tparam Interpolator Obiekt realizuj¹cy interpolacjê pomiêdzy próbkami kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator < ValueType >>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeDiscreteFunctionAccessorAdapter = SafeAccessorAdapter < IDiscreteAccessorT<ValueType, ArgumentType>,
		DiscreteFunctionAccessorAdapter < ValueType, ArgumentType, Interpolator, Extrapolator >> ;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla dyskretnych akcesorów, zawê¿aj¹cy ich zakres
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
		//! \param size Iloœc próbek od indeksu startowego
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
				return Extrapolator::argumentOverflow(accessor.argument(ridx), accessor);
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
				return{ accessor.argument(ridx), ((idx < size_) ? accessor.value(ridx) : Extrapolator::argumentOverflow(accessor.argument(ridx), accessor)) };
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
		const IDiscreteAccessorT<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const std::size_t start;
		//! Iloœæ próbek
		const std::size_t size_;		
	};

	//! \tparam ChannelType Typ kana³u do zmiany reprezentacji dostêpu do danych
	//! \tparam Interpolator Obiekt realizuj¹cy interpolacjê pomiêdzy próbkami kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeDiscreteSubAccessorAdapter = SafeAccessorAdapter < IDiscreteAccessorT<ValueType, ArgumentType>,
		DiscreteSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >> ;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename ValueType, typename ArgumentType,
	typename Extrapolator = ExceptionExtrapolator>
	//! Wrapper dla ci¹g³ych akcesorów, zawê¿aj¹cy ich zakres
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
		FunctionSubAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			const Extrapolator & extrapolator = Extrapolator())
			: Extrapolator(extrapolator), accessor(accessor), start(start), end(end)
		{
			init();
		}

		//! \param accessor Wrapowany accessor
		//! \param start Argument od którego zaczynamy
		//! \param end Argument na którym 
		//! \param extrapolator Extrapolator dla brzegowych wartoœci argumentów
		FunctionSubAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType start, const ArgumentType end,
			Extrapolator && extrapolator)
			: Extrapolator(std::move(extrapolator)), accessor(accessor), start(start), end(end)
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
				return Extrapolator::argumentUnderflow(argument, accessor);
			}
			else if (argument > end){
				return Extrapolator::argumentOverflow(argument, accessor);
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
		//! Wrapowany kana³
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const ArgumentType start;
		//! Iloœæ próbek
		const ArgumentType end;
	};

	//! \tparam ChannelType Typ kana³u do zmiany reprezentacji dostêpu do danych
	//! \tparam Interpolator Obiekt realizuj¹cy interpolacjê pomiêdzy próbkami kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeFunctionSubAccessorAdapter = SafeAccessorAdapter < IFunctionAccessorT<ValueType, ArgumentType>,
		FunctionSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >> ;

	//! Struktura pomocnicza realizuj¹ca przeŸroczystoœæ argumentów
	struct TransparentArgumentExtractor
	{
		//! \tparam Typ argumentu
		template<typename ArgumentType>
		//! \param Modyfikowany (przepuszczany bez zmian) argument
		//! \return Argument bez zmian
		static inline const ArgumentType & extract(const ArgumentType & argument, ...) { return argument };
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor, typename ArgumentExtractor = TransparentArgumentExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
	//! Wrapper dla dyskretnych akcesorów, zmieniaj¹cy ich reprezentacjê
	class DiscreteAccessorAdapter : public IIndependentDiscreteAccessorT<DestValueType, DestArgumentType>,
		private ValueExtractor, private ArgumentExtractor
	{
	public:

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor(),
			const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: ValueExtractor(valueExtractor), ArgumentExtractor(argumentExtractor),
			accessor(accessor)
		{

		}

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			ValueExtractor && valueExtractor, ArgumentExtractor && argumentExtractor)
			: ValueExtractor(std::move(valueExtractor)), ArgumentExtractor(std::move(argumentExtractor)),
			accessor(accessor)
		{

		}

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			const ValueExtractor & valueExtractor,
			const ArgumentExtractor && argumentExtractor)
			: ValueExtractor(valueExtractor), ArgumentExtractor(std::move(argumentExtractor)),
			accessor(accessor)
		{

		}

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			ValueExtractor && valueExtractor, const ArgumentExtractor & argumentExtractor = ArgumentExtractor())
			: ValueExtractor(std::move(valueExtractor)), ArgumentExtractor(argumentExtractor),
			accessor(accessor)
		{

		}

		//! Destruktor wirtualny
		virtual ~DiscreteAccessorAdapter() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return ArgumentExtractor::extract(accessor.argument(idx), idx); }

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual value_type value(const size_type idx) const override { return ValueExtractor::extract(accessor.value(idx), idx); }

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return accessor.size(); }

		//! \return Czy kana³ nie zawiera danych
		virtual bool empty() const override { return accessor.empty(); }

	private:
		//! Accessor
		const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor;		
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor, typename ArgumentExtractor = TransparentArgumentExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeDiscreteAccessorAdapter = SafeAccessorAdapter < IDiscreteAccessorT<DestValueType, DestArgumentType>,
		DiscreteAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, ArgumentExtractor, DestValueType, DestArgumentType >> ;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename BaseArgumentType,	typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), std::declval<BaseArgumentType>()))>::type>
		//! Wrapper dla funkcji, zmieniaj¹cy ich reprezentacjê
	class FunctionAccessorAdapter : public IGeneratedFunctionAccessorT<DestValueType, BaseArgumentType>,
		private ValueExtractor
	{
	public:

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		FunctionAccessorAdapter(const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor,
			const ValueExtractor & valueExtractor = ValueExtractor())
			: ValueExtractor(valueExtractor), accessor(accessor)
		{

		}

		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		FunctionAccessorAdapter(const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor,
			ValueExtractor && valueExtractor)
			: ValueExtractor(std::move(valueExtractor)), accessor(accessor)
		{

		}

		//! Destruktor wirtualny
		virtual ~FunctionAccessorAdapter() {}

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return ValueExtractor::extract(accessor.value(argument), argument); }		

	private:
		//! Accessor
		const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor;		
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeFunctionAccessorAdapter = SafeAccessorAdapter < IFunctionAccessorT<BaseValueType, BaseArgumentType>,
		FunctionAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, DestValueType>> ;

	//! \tparam Idx Indeks jaki chcemy wyci¹gn¹æ
	template<std::size_t Idx>
	//! Struktura pomocnicza przy wyci¹ganiu wartoœci zamiast ca³ych wektorów
	struct StaticVectorElementExtractor
	{
		//! \tparam T Typ wektora którego elementy wyci¹gamy
		template<typename T>
		//! \param value Rozpakowywana wartoœæ wektora
		//! \return Element wektora
		static inline auto extract(const T & value, ...) -> decltype(value[Idx])
		{
			return value[Idx];
		}
	};

	//! Klasa pozwalaj¹ca wyci¹gaæ wartoœci wektorów zamiast ca³ych wektorów
	class VectorElementExtractor
	{
	public:
		//! \param idx Wypakowywany indeks
		VectorElementExtractor(const std::size_t idx) : idx(idx) {}
		//! Destruktor
		~VectorElementExtractor() {}

		//! \tparam T Typ wektora którego elementy wyci¹gamy
		template<typename T>
		//! \param value Rozpakowywana wartoœæ wektora
		//! \return Element wektora
		auto extract(const T & value, ...) -> decltype(value[idx])
		{
			return value[idx];
		}

	private:
		//! Wyci¹gany indeks
		const std::size_t idx;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__ADAPTER_H__