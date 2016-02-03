/********************************************************************
created:  2015/07/28	15:04:12
filename: FunctionFeature.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__FUNCTIONFEATURE_H__
#define __HEADER_GUARD_DATACHANNEL__FUNCTIONFEATURE_H__

#include <datachannellib/Accessors.h>
#include <set>
#include <utils/Utils.h>
#include <type_traits>

namespace dataaccessor
{

	class FunctionFeature;

	//! Interfejs opisuj�cy cechy kana�y reprezentuj�cego funkcj� - jeden argument jedna warto��,
	//! jak kana� nie ma tego interfejsu to traktujemy go jako relacje i mo�liwa jest tylko wersja dyskretna
	//! - musi j� implementowa�, inaczej FAIL
	class IFunctionFeature : public FeatureHelperT<Function>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFunctionFeature() {}
		//! \return Czy funkcja jest parzysta
		virtual bool even() const { return false; }
		//! \return Czy funkcja jest nieparzysta
		virtual bool odd() const { return false; }
		//! \return Czy funkcja jest monotoniczna
		virtual MonotonyType monotony() const { return NonMonotonic; }

		template<typename ValueType, typename ArgumentType>
		inline static FunctionFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			FunctionFeature * ret = nullptr;
			if (discrete != nullptr && discrete->empty() == false){
				ret = create(*discrete, std::is_arithmetic<ValueType>());
			}

			return ret;
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionFeature * create(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			std::true_type)
		{
			using Discrete = IDiscreteAccessorT<ValueType, ArgumentType>;
			FunctionFeature * ret = nullptr;
			bool even = false;
			bool odd = false;
			MonotonyType monotonic = Constant;

			if (accessor.size() == 1){
				const auto s = accessor.sample(0);
				even = (s.first == Discrete::argument_type());
				odd = (s.second == -s.second);
				ret = new FunctionFeature(even, odd, monotonic);
			}
			else{
				auto sA = accessor.sample(0);
				auto sB = accessor.sample(1);
				if (sA.first != sB.first){

					auto monotony = utils::sign(sB.second - sA.second);

					std::set<Discrete::argument_type> arguments;
					arguments.insert(sA.first);
					arguments.insert(sB.first);
					const auto signum = utils::sign(sB.first - sA.first);
					auto locSignum = signum;
					for (decltype(accessor.size()) idx = 2; idx < (accessor.size() - 1); ++idx)
					{
						sA = sB;
						sB = accessor.sample(idx);
						locSignum = utils::sign(sB.first - sA.first);
						if (locSignum != signum){
							break;
						}

						if (arguments.find(sB.first) == arguments.end()){
							arguments.insert(signum > 0 ? arguments.end() : arguments.begin(), sB.first);
						}
						else{
							--locSignum;
							break;
						}

						if (monotonic != NonMonotonic) {
							auto locMonotony = utils::sign(sB.second - sA.second);
							auto diff = std::abs(locMonotony - monotony);
							if (diff > 1){
								monotonic = NonMonotonic;
							}
							else if (diff == 1 && !((monotonic == NonDecreasing) || (monotonic == NonGrowing))){
								if (monotonic == Decreasing){
									monotonic = NonGrowing;
								}
								else if (monotonic == Growing){
									monotonic = NonDecreasing;
								}
								else if (monotonic == Constant){
									if (locMonotony > 0){
										monotonic = Growing;
									}
									else{
										monotonic = Decreasing;
									}
								}
							}
						}
					}

					if (locSignum == signum){
						ret = new FunctionFeature(even, odd, monotonic);
					}
				}
			}

			return ret;
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionFeature * create(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			std::false_type)
		{
			using Discrete = IDiscreteAccessorT<ValueType, ArgumentType>;
			FunctionFeature * ret = nullptr;
			bool even = false;
			bool odd = false;
			const MonotonyType monotonic = NonMonotonic;

			if (accessor.size() == 1){
				const auto s = accessor.sample(0);
				even = (s.first == Discrete::argument_type());
				odd = (s.second == -s.second);
				ret = new FunctionFeature(even, odd, monotonic);
			}
			else{
				auto sA = accessor.argument(0);
				auto sB = accessor.argument(1);
				bool ok = true;
				if (sA != sB){
					std::set<Discrete::argument_type> arguments;
					arguments.insert(sA);
					arguments.insert(sB);
					for (decltype(accessor.size()) idx = 2; idx < (accessor.size() - 1); ++idx)
					{
						sA = sB;
						sB = accessor.argument(idx);

						if ((ok = arguments.insert(sB).second) == false){
							break;
						}
					}

					if (ok == true){
						ret = new FunctionFeature(even, odd, monotonic);
					}
				}
			}

			return ret;
		}
	};

	class FunctionFeature : public IFunctionFeature
	{
	public:
		FunctionFeature(const bool even, const bool odd,
			const MonotonyType monotonic)
			: even_(even), odd_(odd), monotonic_(monotonic)
		{}

		virtual ~FunctionFeature() {}
		//! \return Czy funkcja jest parzysta
		virtual bool even() const override final { return even_; }
		//! \return Czy funkcja jest nieparzysta
		virtual bool odd() const override final { return odd_; }
		//! \return Czy funkcja jest monotoniczna
		virtual MonotonyType monotony() const override final { return monotonic_; }

	private:
		const bool even_;
		const bool odd_;
		const MonotonyType monotonic_;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__FUNCTIONFEATURE_H__