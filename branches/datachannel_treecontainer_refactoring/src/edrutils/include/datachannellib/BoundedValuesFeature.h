/********************************************************************
created:  2015/07/28	15:05:37
filename: BoundedValuesFeature.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__BOUNDEDVALUESFEATURE_H__
#define __HEADER_GUARD_DATACHANNEL__BOUNDEDVALUESFEATURE_H__

#include <datachannellib/Accessors.h>
#include <datachannellib/FunctionDescriptionFeature.h>

namespace dataaccessor
{
	//! \rparam ValueType Typ wartoœci kana³u
	template<class ValueType>
	//! Interfejs kana³u z ograniczonymi wartoœciami
	class IBoundedValuesFeature : public FeatureHelperT<ValueBounded, IValueFeatureT<ValueType>>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IBoundedValuesFeature() {};

		//! \return Dolna granica wartoœci
		virtual ValueType minValue() const = 0;

		//! \return Górna granica wartoœci
		virtual ValueType maxValue() const = 0;

		//! Sprawdzamy czy wszystkie argumenty znajduj¹ siê w zadanym przedziale
		static inline IBoundedValuesFeature * create(const IDiscreteValueAccessorT<ValueType> & accessor)
		{
			IBoundedValuesFeature * ret = nullptr;
			if (accessor.empty() == false) {

				auto feature = utils::dynamic_pointer_cast<IFunctionDescriptionFeature>(accessor.feature(IFunctionDescriptionFeature::ID));

				auto min = accessor.value(0);
				auto max = min;

				if (feature != nullptr && feature->monotony() != NonMonotonic) {
					max = accessor.value(accessor.size() - 1);
					auto r = std::minmax(min, max);
					min = r.first;
					max = r.second;
				}
				else {

					for (decltype(accessor.size()) idx = 1; idx < accessor.size(); ++idx)
					{
						const auto v = accessor.value(idx);
						if (v < min) {
							min = v;
						}
						else if (v > max) {
							max = v;
						}
					}
				}

				ret = new BoundedValuesFeature<ValueType>(min, max);
			}

			return ret;
		}

		//! Sprawdzamy czy wszystkie argumenty znajduj¹ siê w zadanym przedziale
		template<typename ArgumentType>
		static inline IBoundedValuesFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			IBoundedValuesFeature * ret = nullptr;
			if (discrete != nullptr) {
				discrete->getOrCreateFeature<IFunctionFeature>();
				ret = create(*discrete);
			}

			return ret;
		}
	};

	template<typename ValueType>
	class BoundedValuesFeature : public IBoundedValuesFeature<ValueType>
	{
	public:
		template<typename T, typename U>
		BoundedValuesFeature(T && min, U && max)
			: min_(std::forward<T>(min)), max_(std::forward<U>(max))
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

#endif	// __HEADER_GUARD_DATACHANNEL__BOUNDEDVALUESFEATURE_H__