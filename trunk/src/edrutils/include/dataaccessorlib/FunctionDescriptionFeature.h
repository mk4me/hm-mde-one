/********************************************************************
	created:  2016/02/05
	filename: FunctionDescriptionFeature.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__FUNCTIONDESCRIPTIONFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__FUNCTIONDESCRIPTIONFEATURE_H__

#include <dataaccessorlib/Accessors.h>
#include <set>
#include <utils/Utils.h>
#include <type_traits>

namespace dataaccessor
{

	class FunctionDescriptionFeature;

	//! Interfejs opisuj¹cy cechy kana³y reprezentuj¹cego funkcjê - jeden argument jedna wartoœæ,
	//! jak kana³ nie ma tego interfejsu to traktujemy go jako relacje i mo¿liwa jest tylko wersja dyskretna
	//! - musi j¹ implementowaæ, inaczej FAIL
	class IFunctionDescriptionFeature : public FeatureHelperT<FunctionDescription>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFunctionDescriptionFeature() {}
		//! \return Czy funkcja jest parzysta
		virtual bool even() const { return false; }
		//! \return Czy funkcja jest nieparzysta
		virtual bool odd() const { return false; }
		//! \return Czy funkcja jest monotoniczna
		virtual MonotonyType monotony() const { return NonMonotonic; }

		template<typename ValueType, typename ArgumentType>
		inline static FunctionDescriptionFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			FunctionDescriptionFeature * ret = nullptr;

			if (discrete != nullptr && discrete->getOrCreateFeature<dataaccessor::IFunctionFeature>()->isFunction() == true) {
				ret = create(*discrete, std::is_arithmetic<ValueType>());
			}

			return ret;
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionDescriptionFeature * create(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			std::true_type)
		{
			using Discrete = IDiscreteAccessorT<ValueType, ArgumentType>;
			FunctionDescriptionFeature * ret = nullptr;
			bool even = false;
			bool odd = false;
			MonotonyType monotonic = Constant;

			auto hasUF = (accessor.getOrCreateFeature<dataaccessor::IUniformArgumentsFeature>() != nullptr);

			if (accessor.size() == 1) {
				const auto s = accessor.sample(0);
				even = (s.first == Discrete::argument_type());
				odd = (s.second == -s.second);
				ret = new FunctionDescriptionFeature(even, odd, monotonic);
			}
			else {
				auto sA = accessor.sample(0);
				auto sB = accessor.sample(1);
				if (sA.first != sB.first) {

					auto monotony = utils::sign(sB.second - sA.second);

					std::set<Discrete::argument_type> arguments;
					arguments.insert(sA.first);
					arguments.insert(sB.first);
					const auto signum = utils::sign(sB.first - sA.first);
					auto locSignum = signum;
					for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
					{
						sA = sB;
						sB = accessor.sample(idx);
						locSignum = utils::sign(sB.first - sA.first);
						if (locSignum != signum) {
							break;
						}

						if (arguments.find(sB.first) == arguments.end()) {
							arguments.insert(signum > 0 ? arguments.end() : arguments.begin(), sB.first);
						}
						else {
							--locSignum;
							break;
						}

						if (monotonic != NonMonotonic) {
							auto locMonotony = utils::sign(sB.second - sA.second);
							auto diff = std::abs(locMonotony - monotony);
							if (diff > 1) {
								monotonic = NonMonotonic;
								if (hasUF == true) {
									break;
								}
							}
							else if (diff == 1 && !((monotonic == NonDecreasing) || (monotonic == NonGrowing))) {
								if (monotonic == Decreasing) {
									monotonic = NonGrowing;
								}
								else if (monotonic == Growing) {
									monotonic = NonDecreasing;
								}
								else if (monotonic == Constant) {
									if (locMonotony > 0) {
										monotonic = Growing;
									}
									else {
										monotonic = Decreasing;
									}
								}
							}
						}
					}

					if (locSignum == signum) {
						ret = new FunctionDescriptionFeature(even, odd, monotonic);
					}
				}
			}

			return ret;
		}

		template<typename ValueType, typename ArgumentType>
		static inline FunctionDescriptionFeature * create(const IDiscreteAccessorT<ValueType, ArgumentType> & accessor,
			std::false_type)
		{
			using Discrete = IDiscreteAccessorT<ValueType, ArgumentType>;
			FunctionDescriptionFeature * ret = nullptr;
			bool even = false;
			bool odd = false;
			const MonotonyType monotonic = NonMonotonic;

			if (accessor.size() == 1) {
				const auto s = accessor.sample(0);
				even = (s.first == Discrete::argument_type());
				odd = (s.second == -s.second);
				ret = new FunctionDescriptionFeature(even, odd, monotonic);
			}
			else {
				auto sA = accessor.argument(0);
				auto sB = accessor.argument(1);
				bool ok = true;
				if (sA != sB) {
					std::set<Discrete::argument_type> arguments;
					arguments.insert(sA);
					arguments.insert(sB);
					for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
					{
						sA = sB;
						sB = accessor.argument(idx);

						if ((ok = arguments.insert(sB).second) == false) {
							break;
						}
					}

					if (ok == true) {
						ret = new FunctionDescriptionFeature(even, odd, monotonic);
					}
				}
			}

			return ret;
		}
	};

	class FunctionDescriptionFeature : public IFunctionDescriptionFeature
	{
	public:
		FunctionDescriptionFeature(const bool even, const bool odd,
			const MonotonyType monotonic)
			: even_(even), odd_(odd), monotonic_(monotonic)
		{}

		virtual ~FunctionDescriptionFeature() {}
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

#endif  // __HEADER_GUARD_DATAACCESSOR__FUNCTIONDESCRIPTIONFEATURE_H__