/********************************************************************
	created:  2015/07/28	15:02:06
	filename: UniformArgumentsFeature.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__UNIFORMARGUMENTSFEATURE_H__
#define __HEADER_GUARD_DATACHANNEL__UNIFORMARGUMENTSFEATURE_H__

#include <limits>
#include <type_traits>
#include <datachannellib/Accessors.h>

namespace dataaccessor
{
	//! \tparam ArgumentType Typ argumentu kana�u
	template<typename ArgumentType>
	//! Interfejs kana�u dyskretnego o r�wno oddalonych argumentach
	class IUniformArgumentsFeature : public FeatureHelperT<UniformArguments, IArgumentFeatureT<ArgumentType>>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IUniformArgumentsFeature() {};

		//! \return Czas trwania kana�u
		virtual ArgumentType argumentsInterval() const = 0;

		template<typename ValueType>
		static inline IUniformArgumentsFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			IUniformArgumentsFeature * ret = nullptr;

			if (discrete != nullptr && discrete->empty() == false && discrete->size() > 1){

				auto argA = discrete->argument(0);
				auto argB = discrete->argument(1);

				ArgumentType interval = (argB - argA);
				bool ok = true;

				for (decltype(discrete->size()) idx = 2; idx < discrete->size(); ++idx)
				{
					argA = argB;
					argB = discrete->argument(idx);

					if (interval != (argB - argA)){
						ok = false;
						break;
					}
				}
				
				if (ok == true){
					ret = new UniformArgumentsFeature<ArgumentType>(std::abs(interval));
				}
			}

			return ret;
		}
	};

	template<typename ArgumentType>
	class UniformArgumentsFeature : public IUniformArgumentsFeature<ArgumentType>
	{
	public:
		template<typename T>
		UniformArgumentsFeature(T && interval)
			: interval(std::forward<T>(interval))
		{}

		virtual ~UniformArgumentsFeature() {}
		
		virtual ArgumentType argumentsInterval() const override { return interval; }

	private:

		const ArgumentType interval;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__UNIFORMARGUMENTSFEATURE_H__