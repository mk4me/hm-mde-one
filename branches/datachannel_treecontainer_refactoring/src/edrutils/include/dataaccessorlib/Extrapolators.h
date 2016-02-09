/********************************************************************
	created:  2015/07/29	14:29:09
	filename: Extrapolators.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__EXTRAPOLATORS_H__
#define __HEADER_GUARD_DATAACCESSOR__EXTRAPOLATORS_H__

#include <dataaccessorlib/Interpolators.h>

namespace dataaccessor
{
	//! Typ obsuguj¹cy zapytania o wartoœci kana³u dla czasu spoza zakresu pokrytego przez kana³
	//! Rzuca wyj¹tkami
	class ExceptionExtrapolator
	{
	public:
		//! \tparam ValueType Typ wartoœci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \return Wartoœæ dla argumentu spoza kana³u
		inline static ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			throw std::runtime_error("Argument below allowed/available value");
			return ValueType();
		}

		//! \tparam ValueType Typ wartoœci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \return Wartoœæ dla argumentu spoza kana³u
		inline static ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			throw std::runtime_error("Argument over allowed/available value");
			return ValueType();
		}
	};

	//! \tparam Interpolator Typ interpolatora
	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType, typename Interpolator = LerpInterpolator>
	//! Klasa realizuj¹ca okresowy obraz kana³u
	class PeriodicExtrapolator
	{
	public:

		//! Konstruktor
		//! \param minArg Minimalny argument
		//! \param maxArg Maksymalny argument
		//! \param Interpolator Interpolator u¿ywany do interpolacji wewn¹trz kana³u
		template<typename I = Interpolator>
		PeriodicExtrapolator(const ArgumentType & minArg, const ArgumentType & maxArg,
			I && interpolator = I())
			: interpolator(std::forward<I>(interpolator)), minArg(minArg), maxArg(maxArg),
			length(maxArg - minArg)
		{
			if (minArg >= maxArg){
				throw std::runtime_error("Invalid periodic extrapolator configuration");
			}
		}

		//! \param Other Kopiowany extrapolator okresowy
		PeriodicExtrapolator(const PeriodicExtrapolator & Other)
			: interpolator(Other.interpolator),	minArg(Other.minArg),
			maxArg(Other.maxArg), length(Other.length)
		{
		
		}

		//! \param Other przenoszony extrapolator okresowy
		PeriodicExtrapolator(PeriodicExtrapolator && Other)
			: interpolator(std::move(Other.interpolator)), minArg(Other.minArg),
			maxArg(Other.maxArg), length(Other.length)
		{
		
		}

		//! Destruktor
		~PeriodicExtrapolator() {}

		//! \tparam ValueType Typ wartoœci		
		template<typename ValueType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \param accessor Kana³ dla którego pytaliœmy o argument
		//! \return Wartoœæ dla argumentu spoza kana³u		
		inline ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor) const
		{
			const auto arg = utils::periodic_clamp_underflow(argument, minArg, maxArg, length);				
			return interpolator.interpolate(arg, accessor);
		}

		//! \tparam ValueType Typ wartoœci		
		template<typename ValueType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \param accessor Kana³ dla którego pytaliœmy o argument
		//! \return Wartoœæ dla argumentu spoza kana³u	
		inline ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor) const
		{
			const auto arg = utils::periodic_clamp_overflow(argument, minArg, maxArg, length);
			return interpolator.interpolate(arg, accessor);
		}

	private:
		//! Interpolator
		const Interpolator interpolator;
		//! Najmniejszy argument
		const ArgumentType minArg;
		//! Najwiêkszy argument
		const ArgumentType maxArg;
		//! D³ugoœæ - max argument - min argument
		const ArgumentType length;
	};

	//! \tparam ValueType Typ wartoœci
	template<typename ValueType>
	//! Typ obsuguj¹cy zapytania o wartoœci kana³u dla wartoœci spoza zakresu pokrytego przez kana³
	//! Powiela skrajne próbki
	class BorderExtrapolator
	{
	public:
		//! \tparam ValueType Typ wartoœci
		//! \tparam ArgumentType Typ argumentu
		template<typename ArgumentType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \param accessor Kana³ dla którego pytaliœmy o argument
		//! \return Wartoœæ dla argumentu spoza kana³u		
		static inline ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return accessor.value(0);
		}

		//! \tparam ValueType Typ wartoœci
		//! \tparam ArgumentType Typ argumentu
		template<typename ArgumentType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \param accessor Kana³ dla którego pytaliœmy o argument
		//! \return Wartoœæ dla argumentu spoza kana³u	
		static inline ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return accessor.value(accessor.size() - 1);
		}	
	};

	//! Typ realizuj¹cy liniow¹ ekstrapolacjê
	class LinearExtrapolator
	{
	public:

		//! \tparam ValueType Typ wartoœci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \param accessor Kana³ dla którego pytaliœmy o argument
		//! \return Wartoœæ dla argumentu spoza kana³u		
		static inline ValueType argumentUnderflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			return LerpInterpolator::interpolate(accessor, argument, 0, 1);
		}

		//! \tparam ValueType Typ wartoœci
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param argument Argument który nie mieœci siê w dozwolnym zakresie
		//! \param accessor Kana³ dla którego pytaliœmy o argument
		//! \return Wartoœæ dla argumentu spoza kana³u	
		static inline ValueType argumentOverflow(const ArgumentType & argument, const IDiscreteAccessorT<ValueType, ArgumentType> & accessor)
		{
			const auto s = accessor.size() - 2;
			return LerpInterpolator::interpolate(accessor, argument, s, s + 1);
		}
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__EXTRAPOLATORS_H__