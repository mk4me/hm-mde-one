/********************************************************************
	created:  2015/07/28	15:02:06
	filename: UniformArgumentsFeature.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSFEATURE_H__

#include <limits>
#include <type_traits>
#include <dataaccessorlib/Accessors.h>

namespace dataaccessor
{
	template<typename ArgumentType>
	class UniformArgumentsFeature;

	//! \tparam ArgumentType Typ argumentu kana³u
	template<typename ArgumentType>
	//! Interfejs kana³u dyskretnego o równo oddalonych argumentach
	class IUniformArgumentsFeature : public FeatureHelperT<UniformArguments, IArgumentFeatureT<ArgumentType>>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IUniformArgumentsFeature() {};

		//! \return Czas trwania kana³u
		virtual ArgumentType argumentsInterval() const = 0;

		template<typename ValueType>
		static inline IUniformArgumentsFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			IUniformArgumentsFeature * ret = nullptr;
			if (discrete != nullptr) {
				ret = create(*discrete);
			}

			return ret;
		}
		
		static inline IUniformArgumentsFeature * create(const IDiscreteArgumentAccessorT<ArgumentType> & accessor)
		{
			IUniformArgumentsFeature * ret = nullptr;

			if (accessor.size() > 1){

				auto argA = accessor.argument(0);
				auto argB = accessor.argument(1);

				ArgumentType interval = (argB - argA);
				bool ok = true;

				for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
				{
					argA = argB;
					argB = accessor.argument(idx);

					if (interval != (argB - argA)){
						ok = false;
						break;
					}
				}
				
				if (ok == true){
					ret = new UniformArgumentsFeature<ArgumentType>(interval);
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

#endif	// __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSFEATURE_H__
