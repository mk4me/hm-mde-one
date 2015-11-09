/********************************************************************
	created:  2015/10/28	15:38:33
	filename: function_traits.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__FUNCTION_TRAITS_H__
#define __HEADER_GUARD_UTILS__FUNCTION_TRAITS_H__

#include <type_traits>
#include <utility>
#include <tuple>

namespace std
{
	//implementacja wziêta i rozwiniêta na bazie tych Ÿróde³:
	//https://functionalcpp.wordpress.com/2013/08/05/function-traits/


	//! \tparam F Sprawdzana funkcja
	//! \tparam M Funkcja o sygnaturze membera (this, Args...)
	template<typename F, typename M>
	//! Klasa pomocnicza przy sprawdzaniu czy dana funkcja jest memberem (statyczna)
	struct member_function_helper
	{
		static const bool is_static = !std::is_same<F, M>::value;
		using type = typename std::conditional<is_static, F, M>::type;
	};

	//! \tparam M Czy funkcja jest skladowa klasy
	//! \tparam M Czy metoda ma kwalifikator const
	template<bool M = false, bool S = false,
		bool L = false, bool R = false,
		//weryfikujemy
		//czy wszystko false
	typename std::enable_if<((M == false) && (M == S) && (M == L) && (M == R)) ||
	//czy member
	((M == true) &&
	// czy static
	(((S == true) && (L == false) && (R == false)) ||
	// czy non-static
	((S == false) && ((!L || !R) == true))))>::type * = 0>
	struct member_function_traits
	{
		static const bool member = M;
		static const bool static_member = S;
		static const bool l_ref_member = L;
		static const bool r_ref_member = R;
	};

	//! \tparam M Czy funkcja jest skladowa klasy
	//! \tparam M Czy metoda ma kwalifikator const
	template<bool C = false, bool V = false, bool Var = false>
	struct function_traits_qualifiers
	{		
		static const bool const_function = C;
		static const bool volatile_function = V;
		static const bool variadic_function = Var;
	};

	template<typename>
	struct function_traits_helper;

	//! \tparam R typ zwracany
	//! \tparam Args Argumenty funkcji
	template<class R, class... Args>
	struct function_traits_helper<R(Args...)>
	{
		using return_type = R;
		using decay_return_type = typename std::decay<R>::type;

		static const std::size_t arity = sizeof(Args...);

		template <std::size_t Idx>
		struct argument
		{
			static_assert(Idx < arity, "error: invalid parameter index.");
			using type = typename std::tuple_element<Idx, std::tuple<Args...>>::type;
		};

		template <std::size_t Idx>
		struct decay_argument
		{
			using type = typename std::decay<typename argument<Idx>::type>::type;
		};
	};

	// --------------------------------------------------------------------
	// functor`s
	// --------------------------------------------------------------------
	template<class F>
	struct function_traits : public function_traits_qualifiers<>,
		public member_function_traits<>
	{
	private:
		using call_type = function_traits<decltype(&F::type::operator())>;
	public:
		using return_type = typename call_type::return_type;
		using decay_return_type = typename std::decay<return_type>::type;

		static const std::size_t arity = call_type::arity - 1;	

		template <std::size_t Idx>
		struct argument
		{
			static_assert(Idx < arity, "error: invalid parameter index.");
			using type = typename call_type::template argument<Idx + 1>::type;
		};

		template <std::size_t Idx>
		struct decay_argument
		{			
			using type = typename std::decay<typename argument<Idx>::type>::type;
		};
	};

	template<class F>
	struct function_traits<F&> : public function_traits<F>
	{};

	template<class F>
	struct function_traits<const F&> : public function_traits<F>
	{};

	template<class F>
	struct function_traits<F&&> : public function_traits<F>
	{};

	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	// function
	// --------------------------------------------------------------------
	template<class R, class... Args>
	struct function_traits<R(Args...)> : public function_traits_helper<R(Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<>
	{

	};	

	// variadic function
	template<class R, class... Args>
	struct function_traits<R(Args...,...)> : public function_traits_helper<R(Args...)>,
		public function_traits_qualifiers<false, false, true>, public member_function_traits<>
	{

	};

	// --------------------------------------------------------------------
	// function pointer
	// --------------------------------------------------------------------
	template<class R, class... Args>
	struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
	{};	

	// variadic function pointer
	template<class R, class... Args>
	struct function_traits<R(*)(Args...,...)> : public function_traits<R(Args...)>
	{};

	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	// member function pointer
	// --------------------------------------------------------------------
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)> : public function_traits_helper<typename member_function_helper<R(C::*)(Args...), R(*)(C * const, Args...)>::type>,
		public function_traits_qualifiers<>, public member_function_traits<true, member_function_helper<R(C::*)(Args...), R(*)(C * const, Args...)>::is_static>
	{};

	/*
	// lvalue member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)&> : public function_traits_helper<R(C * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, true>
	{};

	// rvalue member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)&&> : public function_traits_helper<R(C * const, Args...)>,
		public function_traits_qualifiers<>, public member_function_traits<true, false, false, true>
	{};*/

	// const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)const> : public function_traits_helper<R(C const * const, Args...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true>
	{};

	/*
	// lvalue const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)const&> : public function_traits_helper<R(C const * const, Args...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, true>
	{};

	// rvalue const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)const&&> : public function_traits_helper<R(C const * const, Args...)>,
		public function_traits_qualifiers<true>, public member_function_traits<true, false, false, true>
	{};
	*/

	// volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)volatile> : public function_traits_helper<R(volatile C * const, Args...)>,
		public function_traits_qualifiers<false, true>, public member_function_traits<true>
	{};

	/*
	// lvalue volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)volatile&> : public function_traits_helper<R(volatile C * const, Args...)>,
		public function_traits_qualifiers<false, true>, public member_function_traits<true, false, true>
	{};

	// rvalue volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)volatile&&> : public function_traits_helper<R(volatile C * const, Args...)>,
		public function_traits_qualifiers<false, true>, public member_function_traits<true, false, false, true>
	{};
	*/

	// const volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)const volatile> : public function_traits_helper<R(volatile C const * const, Args...)>,
		public function_traits_qualifiers<true, true>, public member_function_traits<true>
	{};	

	/*
	// lvalue const volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)const volatile&> : public function_traits_helper<R(volatile C const * const, Args...)>,
		public function_traits_qualifiers<true, true>, public member_function_traits<true, false, true>
	{};

	// rvalue const volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)const volatile&&> : public function_traits_helper<R(volatile C const * const, Args...)>,
		public function_traits_qualifiers<true, true>, public member_function_traits<true, false, false, true>
	{};*/

	// variadic member function pointer	
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)> : public function_traits_helper<typename member_function_helper<R(C::*)(Args...,...), R(C * const, Args...,...)>::type>,
		public function_traits_qualifiers<false, false, true>, public member_function_traits<true, member_function_helper<R(C::*)(Args...,...), R(C * const, Args...,...)>::is_static>
	{};

	/*
	// variadic lvalue member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)&> : public function_traits_helper<R(C * const, Args...,...)>,
		public function_traits_qualifiers<false, false, true>, public member_function_traits<true, false, true>
	{};

	// variadic rvalue member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...) && > : public function_traits_helper<R(C * const, Args...,...)>,
		public function_traits_qualifiers<false, false, true>, public member_function_traits<true, false, false, true>
	{};*/

	// variadic const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)const> : public function_traits_helper<R(C const * const, Args...,...)>,
		public function_traits_qualifiers<true, false, true>, public member_function_traits<true>
	{};

	/*
	// variadic lvalue const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)const&> : public function_traits_helper<R(C const * const, Args...,...)>,
		public function_traits_qualifiers<true, false, true>, public member_function_traits<true, false, true>
	{};

	// variadic rvalue const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)const&&> : public function_traits_helper<R(C const * const, Args...,...)>,
		public function_traits_qualifiers<true, false, true>, public member_function_traits<true, false, false, true>
	{};*/

	// variadic volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)volatile> : public function_traits_helper<R(volatile C * const, Args...,...)>,
		public function_traits_qualifiers<false, true, true>, public member_function_traits<true>
	{};

	/*
	// variadic lvalue volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)volatile&> : public function_traits_helper<R(volatile C * const, Args...,...)>,
		public function_traits_qualifiers<false, true, true>, public member_function_traits<true, false, true>
	{};

	// variadic rvalue volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)volatile&&> : public function_traits_helper<R(volatile C * const, Args...,...)>,
		public function_traits_qualifiers<false, true, true>, public member_function_traits<true, false, false, true>
	{};*/

	// variadic const volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)const volatile> : public function_traits_helper<R(volatile C const * const, Args...,...)>,
		public function_traits_qualifiers<true, true>, public member_function_traits<true>
	{};

	/*
	// variadic lvalue const volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)const volatile&> : public function_traits_helper<R(volatile C const * const, Args...,...)>,
		public function_traits_qualifiers<true, true, true>, public member_function_traits<true, false, true>
	{};

	// variadic rvalue const volatile member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...,...)const volatile&&> : public function_traits_helper<R(volatile C const * const, Args...,...)>,
		public function_traits_qualifiers<true, true, true>, public member_function_traits<true, false, false, true>
	{};*/

	// --------------------------------------------------------------------	
}

#endif	// __HEADER_GUARD_UTILS__FUNCTION_TRAITS_H__