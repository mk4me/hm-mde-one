/********************************************************************
created:  2015/07/28	15:50:12
filename: Adapter.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__ADAPTER_H__
#define __HEADER_GUARD_DATAACCESSOR__ADAPTER_H__

#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/SafeAccessorWrapper.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>
#include <dataaccessorlib/BoundedArgumentsFeature.h>
#include <dataaccessorlib/BoundedValuesFeature.h>
#include <dataaccessorlib/FunctionFeature.h>
#include <dataaccessorlib/FunctionDescriptionFeature.h>
#include <dataaccessorlib/Interpolators.h>
#include <dataaccessorlib/Extrapolators.h>
#include <utils/MemberExtractor.h>

namespace dataaccessor
{
	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType>
	//! Klasa realizuj¹ca generator argumentów
	class UniformArgumentsGenerator
	{
	public:

		template<typename AT = ArgumentType>
		//! \param step Krok z jakim generujemy argumenty
		//! \param start Wartoœc argumentu od którego zaczynamy
		//! \param end Wartoœc argumentu na której koñczymy!!
		UniformArgumentsGenerator(AT && step,
			const std::size_t size,
			AT && start = AT(0))
			: start_(std::forward<AT>(start)), step_(std::forward<AT>(step)),
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
		template<typename T, ENABLE_IF(std::is_convertible<T, ArgumentType>::value)>
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

		}

		//! Destruktor
		~UniformArgumentsGenerator() {}

		//! \param idx Index dla którego generujemy argument
		inline ArgumentType argument(const std::size_t idx) const
		{
			if (idx >= size_) {
				throw std::range_error("Generator range exceeded");
			}

			return start_ + idx * step_;
		}

		//! \return Iloœc próbek do wygenerowania
		inline ArgumentType start() const
		{
			return start_;
		}

		//! \return Iloœc próbek do wygenerowania
		inline ArgumentType end() const
		{
			return start_ + size_ * step_;
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
			if (accessor != nullptr) {
				if (feature == nullptr) {
					feature = utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_);
				}

				accessor->attachFeature(utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_));
			}
		}

	private:
		//! Granice do inicjalizacji
		utils::shared_ptr<UniformArgumentsFeature<ArgumentType>> feature;
		//! Wartoœæ pocz¹tkowa argumentu
		const ArgumentType start_;
		//! Krok argumentów
		const ArgumentType step_;
		//! Iloœæ próbek
		const std::size_t size_;
	};

	//! Klasa pomocnicza przy znajdowaniu argumentów otaczaj¹cych dany argument w dyskretnym kanale
	struct NearestArgumentsFinder
	{
		//! Typ zakresu indeksów
		using Range = std::pair<std::size_t, std::size_t>;

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
			const ArgumentType & argument)
		{
			UTILS_COMPILER_WARNING("Unoptimized search function");
			std::size_t minIdx = 0;
			std::size_t maxIdx = accessor.size() - 1;

			if (argument == accessor.argument(0)) {
				maxIdx = 0;
			}
			else if (argument == accessor.argument(maxIdx)) {
				minIdx = maxIdx;
			}
			else {
				while (maxIdx > minIdx) {
					const auto midIdx = (maxIdx + minIdx) >> 1;
					const auto midArgument = accessor.argument(midIdx);

					if (midArgument < argument) {
						minIdx = midIdx + 1;
					}
					else if (midArgument > argument) {
						maxIdx = midIdx - 1;
					}
					else {
						maxIdx = minIdx = midIdx;
						break;
					}
				}
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
			const ArgumentType & argument, const ArgumentType & interval)
		{
			const std::size_t idx = (argument - accessor.argument(0)) / interval;
			if (accessor.argument(idx) == argument) {
				return Range(idx, idx);
			}

			return Range(idx, idx + 1);
		}
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam ArgumentsGenerator Generator argumentów dla kolejnych indeksów
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator < ArgumentType >>
		//! Klasa realizuje dyskretny dostêp do ci¹g³ego kana³u danych opisuj¹cego funkcjê
	class FunctionDiscreteAccessorAdapter : public IOptimizedDiscreteAccessorT<ValueType, ArgumentType>,
		private ArgumentsGenerator
	{
	public:
		template<typename AG = ArgumentsGenerator>
		//! \param accessor Kana³ wrapowany		
		//! \param argumentsGenerator Generator argumentów
		FunctionDiscreteAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			AG && argumentsGenerator = AG())
			: ArgumentsGenerator(std::forward<AG>(argumentsGenerator)), accessor(accessor)
		{
			attachFeature(accessor.getOrCreateFeature<IFunctionFeature>());
			//TODO
			//na bazie argumentsGenerator dodaæ boundedarguments feature
		}

		//! Destruktor wirtualny
		virtual ~FunctionDiscreteAccessorAdapter() {}

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{
			return accessor.sample(this->ArgumentsGenerator::argument(idx));
		}

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return this->ArgumentsGenerator::size(); }

	private:
		//! Kana³ ci¹g³y
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
	};

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam ArgumentsGenerator Generator argumentów dla kolejnych indeksów
	template<typename ValueType, typename ArgumentType,
		typename ArgumentsGenerator = UniformArgumentsGenerator < ArgumentType >>
		//! Klasa realizuje dyskretny dostêp do ci¹g³ego kana³u danych opisuj¹cego funkcjê
		using SafeFunctionDiscreteAccessorAdapter = SafeAccessorWrapper < FunctionDiscreteAccessorAdapter < ValueType, ArgumentType, ArgumentsGenerator >,
		IFunctionAccessorT < ValueType, ArgumentType >> ;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam Interpolator Obiekt realizuj¹cy interpolacjê pomiêdzy próbkami kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Interpolator = LerpInterpolator,
		typename Extrapolator = BorderExtrapolator < ValueType >>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
	class DiscreteFunctionAccessorAdapter : public IGeneratedFunctionAccessorT<ValueType, ArgumentType>,
		private Interpolator, private Extrapolator
	{
	public:

		template<typename IT = Interpolator, typename ET = Extrapolator>
		//! \param accessor Kana³ wrapowany
		//! \param interpolator Obiekt interpoluj¹cy wewn¹trz kana³u
		//! \param extrapolator Obiekt ekstrapoluj¹cy poza kana³em
		DiscreteFunctionAccessorAdapter(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			IT && interpolator = IT(), ET && extrapolator = ET())
			: Interpolator(std::forward<IT>(interpolator)), Extrapolator(std::forward<ET>(extrapolator)), accessor(accessor),
			range(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessorT<ValueType, ArgumentType>&,
				const ArgumentType&)>(&NearestArgumentsFinder::range<ValueType, ArgumentType>))
		{
			auto ff = accessor.getOrCreateFeature<IFunctionFeature>();
			if (ff->isFunction() == false) {
				throw std::runtime_error("Non function data");
			}

			attachFeature(ff);

			feature = accessor.getOrCreateFeature<IBoundedArgumentsFeature>();

			auto uaf = accessor.getOrCreateFeature<IUniformArgumentsFeature>();
			if (uaf != nullptr) {
				range = std::bind(static_cast<NearestArgumentsFinder::Range(*)(const IDiscreteAccessorT<ValueType, ArgumentType>&,
					const ArgumentType&, const ArgumentType&)>(&NearestArgumentsFinder::range<ValueType, ArgumentType>),
					std::placeholders::_1, std::placeholders::_2, uaf->argumentsInterval());
			}
		}

		//! Destruktor wirtualny
		virtual ~DiscreteFunctionAccessorAdapter() {}

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		ValueType value(const ArgumentType & argument) const override
		{
			if (feature != nullptr) {

				if (argument < feature->minArgument()) {
					return this->Extrapolator::argumentUnderflow(argument, accessor);
				}
				else if (argument > feature->maxArgument()) {
					return this->Extrapolator::argumentOverflow(argument, accessor);
				}
				else {
					const auto r = range(accessor, argument);
					if (r.first == r.second) {
						return accessor.value(r.first);
					}
					else {
						return this->Interpolator::interpolate(accessor, argument, r.first, r.second);
					}
				}
			}
			else if (accessor.empty() == false) {
				const auto arg = accessor.argument(0);
				if (argument < arg) {
					return this->Extrapolator::argumentUnderflow(argument, accessor);
				}
				else if (argument > arg) {
					return this->Extrapolator::argumentOverflow(argument, accessor);
				}
				else {
					return accessor.value(0);
				}
			}
			else {
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
		using SafeDiscreteFunctionAccessorAdapter = SafeAccessorWrapper < DiscreteFunctionAccessorAdapter < ValueType, ArgumentType, Interpolator, Extrapolator >,
		IDiscreteAccessorT<ValueType, ArgumentType >> ;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Wrapper dla dyskretnych akcesorów, zawê¿aj¹cy ich zakres
	class DiscreteSubAccessorAdapter : public IDiscreteAccessorT<ValueType, ArgumentType>,
		private Extrapolator
	{
	private:

		void init()
		{
			if (start_ + size_ > accessor.size()) {
				throw std::runtime_error("Invalid discrete subaccessor configuration");
			}

			attachFeature(accessor.getOrCreateFeature<IUniformArgumentsFeature>());

			auto ff = accessor.getOrCreateFeature<IFunctionFeature>();
			if (ff->isFunction() == true) {

				attachFeature(ff);

				auto fdf = accessor.feature<IFunctionDescriptionFeature>();

				if (fdf != nullptr && fdf->monotony() != NonMonotonic) {
					auto minSample = accessor.sample(start_);
					auto maxSample = accessor.sample(start_ + size_);
					auto resArg = std::minmax(minSample.first, maxSample.first);
					auto resVal = std::minmax(minSample.second, maxSample.second);
					attachFeature(IFeaturePtr(new BoundedArgumentsFeature<ArgumentType>(resArg.first, resArg.second)));
					attachFeature(IFeaturePtr(new BoundedValuesFeature<ValueType>(resVal.first, resVal.second)));
				}
			}
		}

	public:

		template<typename ET = Extrapolator>
		//! \param accessor Wrapowany accessor
		//! \param start Index startowy
		//! \param size Iloœc próbek od indeksu startowego
		//! \param extrapolator
		DiscreteSubAccessorAdapter(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const std::size_t start, const std::size_t size,
			ET && extrapolator = ET()) :
			Extrapolator(std::forward<ET>(extrapolator)), accessor(accessor),
			start_(start), size_(size)
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
			if (ridx >= accessor.size()) {
				throw std::out_of_range("Argument out of range");
			}

			return accessor.argument(ridx);
		}

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual value_type value(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (ridx >= accessor.size()) {
				throw std::out_of_range("Argument out of range");
			}

			if (idx >= size_) {
				return this->Extrapolator::argumentOverflow(accessor.argument(ridx), accessor);
			}
			else {
				return accessor.value(ridx);
			}
		}

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override
		{
			const auto ridx = realIdx(idx);
			if (idx < size_) {
				return accessor.sample(ridx);
			}
			else if (ridx < accessor.size()) {
				return{ accessor.argument(ridx), ((idx < size_) ? accessor.value(ridx) : this->Extrapolator::argumentOverflow(accessor.argument(ridx), accessor)) };
			}
			else {
				throw std::out_of_range("Argument out of range");
			}
		}

		//! \return Iloœæ próbek w kanale
		virtual size_type size() const override { return size_; }

	private:

		//! \param idx Indeks o jaki pytamy w tym kanale
		//! \return Mapowanie naszego indeksu na indeks wrapowanego kana³u
		inline std::size_t realIdx(const std::size_t idx) const { return start_ + idx; }

	private:
		//! Wrapowany kana³
		const IDiscreteAccessorT<ValueType, ArgumentType> & accessor;
		//! Indeks startowy
		const std::size_t start_;
		//! Iloœæ próbek
		const std::size_t size_;
	};

	//! \tparam ChannelType Typ kana³u do zmiany reprezentacji dostêpu do danych
	//! \tparam Interpolator Obiekt realizuj¹cy interpolacjê pomiêdzy próbkami kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeDiscreteSubAccessorAdapter = SafeAccessorWrapper <DiscreteSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >>;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Wrapper dla ci¹g³ych akcesorów, zawê¿aj¹cy ich zakres
	class FunctionSubAccessorAdapter : public IFunctionAccessorT<ValueType, ArgumentType>,
		private Extrapolator
	{
	private:

		void init()
		{
			if (start > end) {
				throw std::invalid_argument("Invalid arguments for function subaccessor range");
			}

			attachFeature(accessor.getOrCreateFeature<IFunctionFeature>());

			auto fdf = accessor.feature<IFunctionDescriptionFeature>();

			if (fdf != nullptr && fdf->monotony() != NonMonotonic) {
				auto minSample = accessor.sample(start);
				auto maxSample = accessor.sample(end);
				auto resArg = std::minmax(minSample.first, maxSample.first);
				auto resVal = std::minmax(minSample.second, maxSample.second);
				attachFeature(IFeaturePtr(new BoundedArgumentsFeature<ArgumentType>(resArg.first, resArg.second)));
				attachFeature(IFeaturePtr(new BoundedValuesFeature<ValueType>(resVal.first, resVal.second)));
			}
			else {
				//TODO
				//jesli dodano valueboud lub argument bound to dodajemy po aktualizacji
			}
		}

	public:

		template<typename ET = Extrapolator>
		//! \param accessor Wrapowany accessor
		//! \param start Argument od którego zaczynamy
		//! \param end Argument na którym 
		//! \param extrapolator Extrapolator dla brzegowych wartoœci argumentów
		FunctionSubAccessorAdapter(const IFunctionAccessorT<ValueType, ArgumentType> & accessor,
			const ArgumentType & start, const ArgumentType & end,
			ET && extrapolator = ET())
			: Extrapolator(std::forward<ET>(extrapolator)), accessor(accessor), start(start), end(end)
		{
			init();
		}

		//! Destruktor wirtualny
		virtual ~FunctionSubAccessorAdapter() {}

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override
		{
			if (argument < start) {
				return this->Extrapolator::argumentUnderflow(argument, accessor);
			}
			else if (argument > end) {
				return this->Extrapolator::argumentOverflow(argument, accessor);
			}
			else {
				return accessor.value(argument);
			}
		}
		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Próbka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const override
		{
			if (argument < start) {
				return{ argument, this->Extrapolator::argumentUnderflow(argument, accessor) };
			}
			else if (argument > end) {
				return{ argument, this->Extrapolator::argumentOverflow(argument, accessor) };
			}
			else {
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

	//! \tparam ValueType Typ wartoœci
	//! \tparam ArgumentType Typ argumentów	
	//! \tparam Extrapolator Obiekt realizuj¹cy politykê zapytañ o argumenty spoza zakresu kana³u
	template<typename ValueType, typename ArgumentType,
		typename Extrapolator = ExceptionExtrapolator>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeFunctionSubAccessorAdapter = SafeAccessorWrapper < FunctionSubAccessorAdapter < ValueType, ArgumentType, Extrapolator >>;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename BaseArgumentType,
		typename ValueExtractor, typename ArgumentExtractor = utils::TransparentExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
		//! Wrapper dla dyskretnych akcesorów, zmieniaj¹cy ich reprezentacjê
	class DiscreteAccessorAdapter : public IIndependentDiscreteAccessorT<DestValueType, DestArgumentType>,
		private ValueExtractor, private ArgumentExtractor
	{
	public:

		template<typename VT = ValueExtractor, typename AT = ArgumentExtractor>
		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		DiscreteAccessorAdapter(const IDiscreteAccessorT<BaseValueType, BaseArgumentType> & accessor,
			VT && valueExtractor = VT(),
			AT && argumentExtractor = AT())
			: ValueExtractor(std::forward<VT>(valueExtractor)),
			ArgumentExtractor(std::forward<AT>(argumentExtractor)),
			accessor(accessor)
		{

		}

		//! Destruktor wirtualny
		virtual ~DiscreteAccessorAdapter() {}

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return this->ArgumentExtractor::extract(accessor.argument(idx), idx); }

		//! \param idx Indeks próbki
		//! \return Wartoœæ dla danego indeksu
		virtual value_type value(const size_type idx) const override { return this->ValueExtractor::extract(accessor.value(idx), idx); }

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
		typename ValueExtractor, typename ArgumentExtractor = utils::TransparentExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), 0))>::type,
		typename DestArgumentType = std::decay<decltype(std::declval<ArgumentExtractor>().extract(std::declval<BaseArgumentType>(), 0))>::type>
		//! Klasa realizuje dostêp ci¹g³y dla kana³ów dyskretnych
		using SafeDiscreteAccessorAdapter = SafeAccessorWrapper < DiscreteAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, ArgumentExtractor, DestValueType, DestArgumentType >,
		IDiscreteAccessorT < BaseValueType, BaseArgumentType >> ;

	//! \tparam ValueType Typ wartoœci kana³u
	//! \tparam ArgumentType Typ argumentów kana³u
	template<typename BaseValueType, typename BaseArgumentType, typename ValueExtractor,
		typename DestValueType = std::decay<decltype(std::declval<ValueExtractor>().extract(std::declval<BaseValueType>(), std::declval<BaseArgumentType>()))>::type>
		//! Wrapper dla funkcji, zmieniaj¹cy ich reprezentacjê
	class FunctionAccessorAdapter : public IGeneratedFunctionAccessorT<DestValueType, BaseArgumentType>,
		private ValueExtractor
	{
	public:

		template<typename VT = ValueExtractor>
		//! \param accessor Kana³ który adaptujemy
		//! \param valueExtractor Wyci¹ga/Modyfikuje/Generuje nowe wartoœci na bazie wartoœci adaptowanego kana³u
		FunctionAccessorAdapter(const IFunctionAccessorT<BaseValueType, BaseArgumentType> & accessor,
			VT && valueExtractor = VT())
			: ValueExtractor(std::forward<VT>(valueExtractor)), accessor(accessor)
		{
			attachFeature(accessor.getOrCreateFeature<IFunctionFeature>());
		}

		//! Destruktor wirtualny
		virtual ~FunctionAccessorAdapter() {}

		//! \param argument Argument dla któego odpytujemy o wartoœæ
		//! \return Wartoœæ dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return this->ValueExtractor::extract(accessor.value(argument), argument); }

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
		using SafeFunctionAccessorAdapter = SafeAccessorWrapper < FunctionAccessorAdapter < BaseValueType, BaseArgumentType, ValueExtractor, DestValueType>>;
}

#endif	// __HEADER_GUARD_DATAACCESSOR__ADAPTER_H__