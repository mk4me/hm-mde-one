/********************************************************************
	created:	2014/11/07
	created:	12:28
	filename: 	Exceptions.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_utils__Exceptions_H__
#define HEADER_GUARD_utils__Exceptions_H__

#include <loglib/ILog.h>
#include <loglib/Logger.h>
#include <type_traits>
#include <typeinfo>
#include <future>


namespace loglib {

	class Exception
	{
		
	private:

		template<typename MessageType>
		inline static void log(const char * typeName, const MessageType & message)
		{
			static auto el = Logger::getLog();// ::getApplication()->log()->subLog("exception");
			LOG_DEBUG(el, "First chance exception [type -> " << typeName << "]: " << message);
		}
	private:

		//! klasa dodaje logowanie do standardowych wyjątków z stl-a
		template <typename BaseException, bool>
		class WrapperImpl : public BaseException
		{
			static_assert((std::is_pod<BaseException>::value == false), "Base exception type should be compound type");
		protected:
			template<class... Args>
			WrapperImpl(Args &&... arguments)
				: BaseException(std::forward<Args>(arguments)...)
			{

			}

			//! destruktor wykonuje faktyczne logowanie
			virtual ~WrapperImpl()
			{
				log(typeid(BaseException).name(), BaseException::what());
			}
		};

		//! klasa dodaje logowanie do standardowych wyjątków z stl-a
		template <typename BaseException>
		class WrapperImpl<BaseException, true>
		{
			static_assert((std::is_pod<BaseException>::value == true), "Base exception type should be POD type");
		protected:
			WrapperImpl(BaseException value)
				: exceptionValue(value)
			{

			}

			explicit operator BaseException() const { return exceptionValue; }

			//! destruktor wykonuje faktyczne logowanie
			virtual ~WrapperImpl()
			{
				log(typeid(BaseException).name(), exceptionValue);				
			}

		private:

			BaseException exceptionValue;
		};

	public:

		//! klasa dodaje logowanie do standardowych wyjątków z stl-a
		template <typename BaseException>
		class Wrapper : public WrapperImpl < BaseException, std::is_pod<BaseException>::value >
		{
		public:
			template<class... Args>
			Wrapper(Args &&... arguments)
				: WrapperImpl < BaseException, std::is_pod<BaseException>::value >(std::forward<Args>(arguments)...)
			{

			}

			//! destruktor wykonuje faktyczne logowanie
			virtual ~Wrapper()
			{

			}
		};

	public:

		template<typename ExceptionType, class... Args>
		static inline void Throw(Args &&... arguments)
		{
			ExceptionType e(std::forward<Args>(arguments)...);
			log(typeid(ExceptionType).name(), e.what());
			throw e;
		}

		template<typename T>
		static inline void Throw(const T & val)
		{
			log(typeid(T).name(), val);			
			throw val;
		}
	};	

	// rozszerzone wyjątki z stl-a, użycie: throw core::runtime_error("whatever...");
	
	typedef Exception::Wrapper<std::bad_cast> bad_cast;
	typedef Exception::Wrapper<std::bad_alloc> bad_alloc;
	typedef Exception::Wrapper<std::bad_array_new_length> bad_array_new_length;
	typedef Exception::Wrapper<std::bad_exception> bad_exception;
	typedef Exception::Wrapper<std::bad_function_call> bad_function_call;
	typedef Exception::Wrapper<std::invalid_argument> bad_typeid;
	typedef Exception::Wrapper<std::invalid_argument> bad_weak_ptr;

	typedef Exception::Wrapper<std::logic_error> logic_error;
	typedef Exception::Wrapper<std::out_of_range> out_of_range;
	typedef Exception::Wrapper<std::domain_error> domain_error;
	typedef Exception::Wrapper<std::future_error> future_error;
	typedef Exception::Wrapper<std::invalid_argument> invalid_argument;
	typedef Exception::Wrapper<std::length_error> length_error;

	typedef Exception::Wrapper<std::runtime_error> runtime_error;
	typedef Exception::Wrapper<std::overflow_error> overflow_error;
	typedef Exception::Wrapper<std::range_error> range_error;
	typedef Exception::Wrapper<std::system_error> system_error;
	typedef Exception::Wrapper<std::underflow_error> underflow_error;

}

#endif // HEADER_GUARD 
