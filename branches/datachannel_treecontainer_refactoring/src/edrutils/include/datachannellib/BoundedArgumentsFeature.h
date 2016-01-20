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

		template<typename ValueType>
		static IBoundedArgumentsFeature<ArgumentType> * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			IBoundedArgumentsFeature<ArgumentType> * ret = nullptr;
			if (discrete != nullptr && discrete->empty() == false){

				auto feature = discrete->getOrCreateFeature<IFunctionFeature>();

				auto min = discrete->argument(0);
				auto max = min;

				if (feature != nullptr){				
					max = discrete->argument(discrete->size() - 1);
					auto r = std::minmax(min, max);
					min = r.first;
					max = r.second;
				}
				else{					

					for (decltype(discrete->size()) idx = 1; idx < discrete->size(); ++idx)
					{
						const auto a = discrete->argument(idx);
						if (a < min){
							min = a;
						}
						else if (a > max){
							max = a;
						}
					}
				}

				ret = new BoundedArgumentsFeature<ArgumentType>(min, max);
			}

			return ret;
		}
	};

	template<typename ArgumentType>
	class BoundedArgumentsFeature : public IBoundedArgumentsFeature<ArgumentType>
	{
	public:
		BoundedArgumentsFeature(const ArgumentType & min, const ArgumentType & max)
			: min_(min), max_(max)
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