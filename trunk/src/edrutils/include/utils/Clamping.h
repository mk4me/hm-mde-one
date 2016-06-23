/********************************************************************
	created:  2016/01/20
	filename: Clamping.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__CLAMPING_H__
#define __HEADER_GUARD_UTILS__CLAMPING_H__

namespace utils
{
	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \return Warto�� w zadanym przedziale
	static inline T clamp(const T& v, const T& min, const T& max)
	{
		return (v < min) ? min : ((v > max) ? max : v);
	}

	//------------------------------------------------------------------------------

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \param length R�nica mi�dzy max i min
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp_underflow(const T& v, const T& min,
		const T& max, const T& length)
	{
		return periodic_clamp_underflow(v, min, max, length, std::is_floating_point<T>());
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp_underflow(const T& v, const T& min,
		const T& max)
	{
		UTILS_COMPILER_WARNING("Using unoptimized periodic_clamp_underflow function");
		return periodic_clamp_underflow(v, min, max, max - min, std::is_floating_point<T>());
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \param length R�nica mi�dzy max i min
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp_underflow(const T& v, const T& min, const T& max,
		const T& length, std::true_type)
	{
		return max + std::fmod(v - min, length);
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \param length R�nica mi�dzy max i min
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp_underflow(const T& v, const T& min, const T& max,
		const T& length, std::false_type)
	{
		return max - ((min - v) % length);
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp_overflow(const T& v, const T& min,
		const T& max)
	{
		UTILS_COMPILER_WARNING("Using unoptimized periodic_clamp_overflow function");
		return periodic_clamp_overflow(v, min, max, max - min, std::is_floating_point<T>());
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \param length R�nica mi�dzy max i min
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp_overflow(const T& v, const T& min, const T& max,
		const T& length, std::true_type)
	{
		return min + std::fmod(v - max, length);
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \param length R�nica mi�dzy max i min
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp_overflow(const T& v, const T& min, const T& max,
		const T& length, std::false_type)
	{
		return min + ((v - max) % length);
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \param length R�nica mi�dzy max i min
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp(const T& v, const T& min, const T& max,
		const T& length)
	{
		return (v < min) ? periodic_clamp_underflow(v, min, max, length)
			: ((v > max) ? periodic_clamp_overflow(v, min, max, length)
				: v);
	}

	//! \tparam T Typ clampowanej warto�ci
	template <typename T>
	//! \param v Clampowana warto��
	//! \param min Minimalna warto��
	//! \param max Maksymalna warto��
	//! \return Warto�� w zadanym przedziale
	static inline T periodic_clamp(const T& v, const T& min, const T& max)
	{
		UTILS_COMPILER_WARNING("Using unoptimized periodic_clamp function");
		return periodic_clamp(v, min, max, max - min);
	}
}

#endif  // __HEADER_GUARD_UTILS__CLAMPING_H__