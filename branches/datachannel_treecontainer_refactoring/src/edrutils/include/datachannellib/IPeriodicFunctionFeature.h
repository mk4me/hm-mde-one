/********************************************************************
	created:  2015/07/28	15:05:18
	filename: IPeriodicFunctionFeature.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__IPERIODICFUNCTIONFEATURE_H__
#define __HEADER_GUARD_DATACHANNEL__IPERIODICFUNCTIONFEATURE_H__

#include <datachannellib/Accessors.h>

namespace datachannel
{
	//! \rparam ArgumentType Typ argumentu kana�u
	template<class ArgumentType>
	//! Interfejs kana�u z ograniczonymi warto�ciami
	class IPeriodicFunctionFeature : public FeatureHelper<Periodic, IArgumentFeature<ArgumentType>>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IPeriodicFunctionFeature() {};

		//! \return D�ugo�� okresu kana�u
		virtual ArgumentType period() const = 0;

		template<typename ValueType>
		inline static IPeriodicFunctionFeature * create(
			const IDiscreteAccessor<ValueType, ArgumentType> * discrete,
			const IFunctionAccessor<ValueType, ArgumentType> * function)
		{
			return nullptr;
		}
	};
	
	template<class ArgumentType>
	class PeriodicFunctionFeature : public IPeriodicFunctionFeature<ArgumentType>
	{
	public:

		PeriodicFunctionFeature(const ArgumentType & period) : period_(period) {}

		virtual ~PeriodicFunctionFeature() {};

		//! \return D�ugo�� okresu kana�u
		virtual ArgumentType period() const override { return period_; }

	private:

		const ArgumentType period_;
	};

}

#endif	// __HEADER_GUARD_DATACHANNEL__IPERIODICFUNCTIONFEATURE_H__