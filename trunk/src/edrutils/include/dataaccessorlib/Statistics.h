/********************************************************************
created:	2011/12/10
created:	10:12:2011   19:28
filename: 	Statistics.h
author:		Wojciech Kniec

purpose:
*********************************************************************/

#ifndef HEADER_GUARD_DATAACCESSOR__STATISTICS_H__
#define HEADER_GUARD_DATAACCESSOR__STATISTICS_H__

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/mpl/always.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <type_traits>
#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/FunctionFeature.h>

// Implementacje dodatkowych akumulatorow
namespace boost {

	namespace accumulators
	{

		namespace impl
		{
			//! Implementacja akumulatora wyszukującego minimum oraz indeks, dla którego minimum wystąpiło
			template <typename Sample>
			struct min_with_idex_impl
				: accumulator_base
			{
				//! zwracany typ - para : minimum, index
				using result_type = std::pair<Sample, std::size_t>;

				template<typename Args>
				min_with_idex_impl(Args const &args) :
					minimum(numeric::as_max(args[sample | Sample()])),
					count(0), index(0)
				{
				}

				//! wywoływane po dadanu kolejnej próbki
				template<typename Args>
				void operator ()(Args const &args)
				{					
					const auto & current = args[sample];
					if (current < minimum) {
						minimum = current;
						index = count;
					}
					++count;
				}


				//! Odpytanie o aktualny stan akumulatora
				//! \param  brak
				//! \return para zawierająca minimum i index dla niego
				result_type result(dont_care) const
				{
					return std::make_pair(this->minimum, this->index);
				}

			private:
				//! aktualne minimum dla podanych próbek
				Sample minimum;
				//! indeks, dla którego wystąpiło aktualne minimum
				std::size_t index;
				//! liczba podanych próbek (potrzebne by wyliczyć indeks)
				std::size_t count;
			};

			//! Implementacja akumulatora wyszukującego maksimum oraz indeks, dla którego maksimum wystąpiło
			//! implementacja analogiczna do min_with_index_impl
			template <typename Sample>
			struct max_with_idex_impl
				: accumulator_base
			{
				using result_type = std::pair<Sample, std::size_t>;

				template<typename Args>
				max_with_idex_impl(Args const &args) :
					maximum(numeric::as_min(args[sample | Sample()])),
					count(0), index(0)
				{
				}

				template<typename Args>
				void operator ()(Args const &args)
				{
					const auto & current = args[sample];
					if (current > maximum) {
						maximum = current;
						index = count;
					}
					++count;
				}

				result_type result(dont_care) const
				{
					return std::make_pair(this->maximum, this->index);
				}

			private:
				Sample maximum;
				std::size_t index;
				std::size_t count;
			};

		} // namespace impl

		// tagi, pozwalające umieszczać akumulatory w accumulator_set
		namespace tag
		{
			//! tag dla akumulatora minimum z indeksem
			struct min_with_index
				: depends_on<>
			{
				using impl = accumulators::impl::min_with_idex_impl<mpl::_1>;
			};

			//! tag dla akumulatora maksimum z indeksem
			struct max_with_index
				: depends_on<>
			{
				using impl = accumulators::impl::max_with_idex_impl<mpl::_1>;
			};
		}

		// ekstraktory, umozliwiaja odpytanie o statystyki
		namespace extract
		{
			extractor<tag::max_with_index> const max_with_index = {};
			extractor<tag::min_with_index> const min_with_index = {};

			BOOST_ACCUMULATORS_IGNORE_GLOBAL(max_with_index)
				BOOST_ACCUMULATORS_IGNORE_GLOBAL(min_with_index)
		}

		using extract::max_with_index;
		using extract::min_with_index;

	}
} // namespace boost::accumulators

////////////////////////////////////////////////////////////////////////////////
namespace dataaccessor {
	////////////////////////////////////////////////////////////////////////////////

	template<typename ValueType, typename ArgumentType>
	class IStatisticsFeature : public FeatureHelperT<Statistics, IAccessorFeatureT<ValueType, ArgumentType>>
	{
	public:

		template<typename T>
		struct Results
		{
			//! Średnia wartość danych w kanale
			T mean;		
			//! Wariancja danych w kanale
			T variance;
			//! Wariancja danych w kanale
			T standardDeviation;
		};

		using Values = Results<ValueType>;
		using Arguments = Results<ArgumentType>;

	public:
		//! Destruktor wirtualny
		virtual ~IStatisticsFeature() {}
		//! \return Minimalna wartość argumentu
		virtual Values values() const = 0;
		//! \return Maksymalna wartośc argumentu
		virtual Arguments arguments() const = 0;
		
		static IStatisticsFeature<ValueType, ArgumentType> * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			IStatisticsFeature<ValueType, ArgumentType> * ret = nullptr;
			if (discrete != nullptr && discrete->empty() == false){
				const auto stats = StatsHelper::samples(*discrete);
				ret = new StatisticsFeature<ValueType, ArgumentType>(stats.values, stats.arguments);
			}

			return ret;
		}

	};

	template<typename ValueType, typename ArgumentType>
	class StatisticsFeature : public IStatisticsFeature<ValueType, ArgumentType>
	{
	public:
		StatisticsFeature(const Results<ValueType> & values,
			const Results<ArgumentType> & arguments)
			: values_(values), arguments_(arguments)
		{

		}

		virtual ~StatisticsFeature() {}

		//! \return Minimalna wartość argumentu
		virtual Values values() const override { return values_; }
		//! \return Maksymalna wartośc argumentu
		virtual Arguments arguments() const override { return arguments_; }

	private:
		const Values values_;
		const Arguments arguments_;
	};

	//! Klasa ze statystykami kanału, leniwie aktualizowana w momencie zapytania
	class StatsHelper
	{
	public:

		template<typename ValueType, typename ArgumentType>//, typename IndexType>
		struct CompleteResult
		{
			typename IStatisticsFeature<ValueType, ArgumentType>::Values values;
			typename IStatisticsFeature<ValueType, ArgumentType>::Arguments arguments;
		};

		template<typename ValueType, typename ArgumentType>
		static typename IStatisticsFeature<ValueType, ArgumentType>::Values
			values(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const IDiscreteAccessor::size_type to,
			const IDiscreteAccessor::size_type from = 0)
		{
			using namespace boost::accumulators;			
			using Stats = boost::accumulators::accumulator_set<ValueType, features< tag::mean, tag::variance >>;

			typename IStatisticsFeature<ValueType, ArgumentType>::Values result;

			auto ff = accessor.feature<IFunctionDescriptionFeature>();

			if (ff != nullptr && ff->monotony() == Constant){
				result.mean = accessor.value(0);
				result.variance = result.standardDeviation = ValueType(0);				
			}
			else{
				Stats sts;

				for (auto i = from; i < to; ++i){
					sts(accessor.value(i));
				}

				result.mean = mean(sts);				
				result.variance = variance(sts);
				result.standardDeviation = std::sqrt(result.variance);
			}		

			return result;
		}

		template<typename ValueType, typename ArgumentType>
		static typename IStatisticsFeature<ValueType, ArgumentType>::Values
			values(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return values(accessor, accessor.size());
		}

		template<typename ValueType, typename ArgumentType>
		static typename IStatisticsFeature<ValueType, ArgumentType>::Arguments
			arguments(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,		
			const IDiscreteAccessor::size_type to,
			const IDiscreteAccessor::size_type from = 0)
		{
			using namespace boost::accumulators;		

			typename IStatisticsFeature<ValueType, ArgumentType>::Arguments result;

			using Stats = boost::accumulators::accumulator_set<ArgumentType, features< tag::mean, tag::variance >>;

			Stats sts;

			for (auto i = from; i < to; ++i){
				sts(accessor.argument(i));
			}

			result.mean = mean(sts);
			result.variance = variance(sts);
			result.standardDeviation = std::sqrt(result.variance);			

			return result;
		}

		template<typename ValueType, typename ArgumentType>
		static typename IStatisticsFeature<ValueType, ArgumentType>::Arguments
			arguments(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return arguments(accessor, accessor.size());
		}

		template<typename ValueType, typename ArgumentType>
		static CompleteResult<ValueType, ArgumentType>
			samples(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			const IDiscreteAccessor::size_type to,
			const IDiscreteAccessor::size_type from = 0)
		{
			using namespace boost::accumulators;
			using ArgStats = boost::accumulators::accumulator_set<ArgumentType, features< tag::mean, tag::variance >>;
			CompleteResult<ValueType, ArgumentType> result;

			ArgStats argSts;

			auto fdf = accessor.feature<IFunctionDescriptionFeature>();

			if (fdf != nullptr && fdf->monotony() == Constant){
				result.values.mean = accessor.value(0);
				result.values.variance = result.values.standardDeviation = ValueType(0);

				for (auto i = from; i < to; ++i){					
					argSts(accessor.argument(i));
				}			
			}
			else{
				using ValStats = boost::accumulators::accumulator_set<ValueType, features< tag::mean, tag::variance >>;

				ValStats valSts;

				for (auto i = from; i < to; ++i){
					const auto s = accessor.sample(i);
					argSts(s.first);
					valSts(s.second);
				}

				result.values.mean = mean(valSts);				
				result.values.variance = variance(valSts);
				result.values.standardDeviation = std::sqrt(result.values.variance);
			}

			result.arguments.mean = mean(argSts);			
			result.arguments.variance = variance(argSts);
			result.arguments.standardDeviation = std::sqrt(result.arguments.variance);

			return result;
		}

		template<typename ValueType, typename ArgumentType>
		static CompleteResult<ValueType, ArgumentType>
			samples(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return samples(accessor, accessor.size());
		}
	};

	////////////////////////////////////////////////////////////////////////////////
} //namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif
