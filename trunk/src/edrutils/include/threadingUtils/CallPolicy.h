/********************************************************************
	created:  2014/10/09	14:18:19
	filename: CallPolicy.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__CALLPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__CALLPOLICY_H__

#include <type_traits>

namespace threadingUtils
{
	//! Polityka surowego wywo�ania
	struct RawCallPolicy
	{
		//! \tparam F Typ metody jak� wo�amy
		//! \tparam Args Typy argument�w z jakimi wo�amy metod�
		//! \param f Wo�ana metoda
		//! \param arguments Argumenty metody
		template<class F, class... Args>
		static typename std::result_of<F(Args...)>::type call(F&& f, Args&&... arguments)
		{
			return std::forward<F>(f)(std::forward<Args>(arguments)...);
		}
	};

	//! Polityka wo�ania metod gasz�ca wszelkie wyj�tki
	struct ExceptionSafeCallPolicy
	{
		//! \tparam F Typ metody jak� wo�amy
		//! \tparam Args Typy argument�w z jakimi wo�amy metod�
		//! \param f Wo�ana metoda
		//! \param arguments Argumenty metody
		template<class F, class... Args>
		static typename std::result_of<F(Args...)>::type call(F&& f, Args&&... arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;

			try{
				return std::forward<F>(f)(std::forward<Args>(arguments)...);
			}
			catch (...){

			}

			return result_type();
		}

		//! \tparam F Typ metody jak� wo�amy
		//! \tparam Args Typy argument�w z jakimi wo�amy metod�
		//! \param f Wo�ana metoda
		//! \param arguments Argumenty metody
		template<class F, class... Args>
		static typename std::result_of<F(Args...)>::type call(typename std::result_of<F(Args...)>::type && failReturn, F&& f, Args&&... arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;

			try{
				return std::forward<F>(f)(std::forward<Args>(arguments)...);
			}
			catch (...){

			}

			return std::forward<result_type>(failReturn);
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__CALLPOLICY_H__