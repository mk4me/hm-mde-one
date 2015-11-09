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
	//! \rparam ArgumentType Typ argumentu kana³u
	template<class ArgumentType>
	//! Interfejs kana³u z ograniczonymi wartoœciami
	class IPeriodicFunctionFeature : public FeatureHelper<Periodic, IArgumentFeature<ArgumentType>>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IPeriodicFunctionFeature() {};

		//! \return D³ugoœæ okresu kana³u
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

		//! \return D³ugoœæ okresu kana³u
		virtual ArgumentType period() const override { return period_; }

	private:

		const ArgumentType period_;
	};

}

#endif	// __HEADER_GUARD_DATACHANNEL__IPERIODICFUNCTIONFEATURE_H__