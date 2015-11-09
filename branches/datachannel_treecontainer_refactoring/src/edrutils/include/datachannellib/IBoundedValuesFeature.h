/********************************************************************
	created:  2015/07/28	15:05:37
	filename: IBoundedValuesFeature.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__IBOUNDEDVALUESFEATURE_H__
#define __HEADER_GUARD_DATACHANNEL__IBOUNDEDVALUESFEATURE_H__

#include <datachannellib/Accessors.h>

namespace datachannel
{
	//! \rparam ValueType Typ wartoœci kana³u
	template<class ValueType>
	//! Interfejs kana³u z ograniczonymi wartoœciami
	class IBoundedValuesFeature : public FeatureHelper<ValueBounded, IValueFeature<ValueType>>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IBoundedValuesFeature() {};

		//! \return Dolna granica wartoœci
		virtual ValueType minValue() const = 0;

		//! \return Górna granica wartoœci
		virtual ValueType maxValue() const = 0;

		//! Sprawdzamy czy wszystkie argumenty znajduj¹ siê w zadanym przedziale
		template<typename ArgumentType>
		static inline IBoundedValuesFeature * create(
			const IDiscreteAccessor<ValueType, ArgumentType> * discrete,
			const IFunctionAccessor<ValueType, ArgumentType> * function)
		{
			IBoundedValuesFeature * ret = nullptr;
			if (discrete != nullptr && discrete->empty() == false){
				auto min = discrete->value(0);
				auto max = min;

				for (decltype(discrete->size()) idx = 1; idx < discrete->size(); ++idx)
				{
					const auto v = discrete->value(idx);
					if (v < min){
						min = v;
					}
					else if (v > max){
						max = v;
					}
				}

				ret = new BoundedValuesFeature<ArgumentType>(min, max);
			}

			return ret;
		}
	};

	template<typename ValueType>
	class BoundedValuesFeature : public IBoundedValuesFeature<ValueType>
	{
	public:
		BoundedValuesFeature(const ValueType & min, const ValueType & max)
			: min_(min), max_(max)
		{}

		virtual ~BoundedValuesFeature() {}

		//! \return Dolna granica wartoœci
		virtual ValueType minValue() const { return min_; }

		//! \return Górna granica wartoœci
		virtual ValueType maxValue() const { return max_; }

	private:

		const ValueType min_;
		const ValueType max_;
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__IBOUNDEDVALUESFEATURE_H__