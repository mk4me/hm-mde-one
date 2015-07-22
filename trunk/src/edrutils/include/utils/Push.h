/********************************************************************
created:  2011/04/22
created:  22:4:2011   10:05
filename: Push.h
author:	  Piotr Gwiazdowski

purpose:
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__PUSH_H__
#define HEADER_GUARD_UTILS__PUSH_H__

#include <utils/Utils.h>
#include <type_traits>

namespace utils
{
	/**
	*	Użytkowa klasa, używana tam, gdzie męcząca jest zmiana wartości jakiejś zmiennej
	*  a następnie ręczne jej przywracanie; jest to uciążliwe zwłaszcza gdy funkcja ma
	*  wiele punktów wyjścia.
	*/

	class Push
	{
	private:

		template <typename T, typename U>
		class PushT : public NullType
		{
		private:
			//! Poprzednia wartość.
			T oldValue;
			//! Zmienna.
			T& variable;
		public:
			//! \param variable
			//! \param newValue
			PushT(T& variable, const U& newValue) : oldValue(variable), variable(variable)
			{
				variable = newValue;
			}
			//! \param variable
			//! \param newValue
			PushT(T& variable, U&& newValue) : oldValue(std::move(variable)), variable(variable)
			{
				variable = std::move(newValue);
			}
			//! 
			~PushT()
			{
				variable = std::move(oldValue);
			}
		private:
			//! Ukrycie konstruktora kopiującego.
			PushT(const PushT& push);
		};

	public:

		template<typename T, typename U>
		Push(T & variable, const U & value) : impl(std::make_shared<PushT<T, U>>(variable, value)) {}

		template<typename T, typename U>
		Push(T & variable, U && value) : impl(std::make_shared<PushT<T, U>>(variable, std::move(value))) {}

		~Push() {}

	private:

		utils::shared_ptr<NullType> impl;
	};

} // namespace utils


#endif  // HEADER_GUARD_UTILS__PUSH_H__
