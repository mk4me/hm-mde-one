/********************************************************************
created:  2015/07/28	15:04:47
filename: BoundedArgumentsFeature.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__BOUNDEDARGUMENTSFEATURE_H__
#define __HEADER_GUARD_DATACHANNEL__BOUNDEDARGUMENTSFEATURE_H__

#include <datachannellib/Accessors.h>
#include <datachannellib/FunctionFeature.h>

namespace dataaccessor
{
	//! \tparam ArgumentType Typ argumentu kana³u danych
	template<typename ArgumentType>
	//! Interfesj opisuj¹cy zakres argumentów
	class IBoundedArgumentsFeature : public FeatureHelperT<ArgumentBounded, IArgumentFeatureT<ArgumentType>>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IBoundedArgumentsFeature() {}
		//! \return Minimalna wartoœæ argumentu
		virtual ArgumentType minArgument() const = 0;
		//! \return Maksymalna wartoœc argumentu
		virtual ArgumentType maxArgument() const = 0;

		static IBoundedArgumentsFeature<ArgumentType> * create(
			const IDiscreteArgumentAccessorT<ArgumentType> & accessor)
		{
			IBoundedArgumentsFeature<ArgumentType> * ret = nullptr;
			if (accessor.empty() == false) {

				auto feature = accessor.featureExists(dataaccessor::Function);

				auto min = accessor.argument(0);
				auto max = min;

				if (feature == true) {
					max = accessor.argument(accessor.size() - 1);
					auto r = std::minmax(min, max);
					min = r.first;
					max = r.second;
				}
				else {

					for (decltype(accessor.size()) idx = 1; idx < accessor.size(); ++idx)
					{
						const auto a = accessor.argument(idx);
						if (a < min) {
							min = a;
						}
						else if (a > max) {
							max = a;
						}
					}
				}

				ret = new BoundedArgumentsFeature<ArgumentType>(min, max);
			}

			return ret;
		}

		template<typename ValueType>
		static IBoundedArgumentsFeature<ArgumentType> * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			IBoundedArgumentsFeature<ArgumentType> * ret = nullptr;
			if (discrete != nullptr) {
				discrete->getOrCreateFeature<IFunctionFeature>();
				ret = create(*discrete);
			}

			return ret;
		}
	};

	template<typename ArgumentType>
	class BoundedArgumentsFeature : public IBoundedArgumentsFeature<ArgumentType>
	{
	public:
		template<typename T, typename U>
		BoundedArgumentsFeature(T && min, U && max)
			: min_(std::forward<T>(min)), max_(std::forward<U>(max))
		{}

		virtual ~BoundedArgumentsFeature() {}

		//! \return Minimalna wartoœæ argumentu
		virtual ArgumentType minArgument() const override { return min_; }
		//! \return Maksymalna wartoœc argumentu
		virtual ArgumentType maxArgument() const override { return max_; }

	private:

		const ArgumentType min_;
		const ArgumentType max_;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__BOUNDEDARGUMENTSFEATURE_H__