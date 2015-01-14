/********************************************************************
	created:	2014/11/07
	created:	12:28
	filename: 	Exceptions.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_utils__Exceptions_H__
#define HEADER_GUARD_utils__Exceptions_H__

#include <corelib/PluginCommon.h>
#include "ILog.h"
#include <type_traits>

namespace  core {

	//! Logger dla wyj¹tków, domyœlnie nie jest ustawiony, powinien byæ ustawiony, gdy zainicjalizowany zostanie system logowania
	struct CORELIB_EXPORT ExceptionLogger
	{
	public:
		//! Ustawienie loga
		static void setLog(LogPtr log) {
			_log = log;
		}
	protected:
		//! globalny log dla wyj¹tków
		static LogPtr _log;
	};

	//! klasa dodaje logowanie do standardowych wyj¹tków z stl-a
	template <typename BaseException>
	class ExceptionImpl : public BaseException, private ExceptionLogger
	{
		static_assert((std::is_pod<BaseException>::value == false), "Base exception type should be compound type");
	public:
		template<class... Args>
		ExceptionImpl(Args &&... arguments)
			: BaseException(std::forward<Args>(arguments)...)
		{

		}

		//! destruktor wykonuje faktyczne logowanie
		virtual ~ExceptionImpl()
		{
			if (ExceptionLogger::_log) {
				LOG_DEBUG(ExceptionLogger::_log, "First chance exception: " << BaseException::what());
			}
		}
	};

	class Exception
	{
	public:

		template<typename ExceptionType, class... Args>
		void Throw(Args &&... arguments)
		{
			ExceptionType e(std::forward<Args>(arguments)...);
			LOG_DEBUG(ExceptionLogger::_log, "First chance exception type [" << std::type_info(ExceptionType).name << "] what : " << e.what());
			throw e;
		}

		template<typename T>
		void Throw(const T & val)
		{
			LOG_DEBUG(ExceptionLogger::_log, "First chance exception type [" << std::type_info(T).name << "] value : " << val );
			throw val;
		}
	};	

	// rozszerzone wyj¹tki z stl-a, u¿ycie: throw core::runtime_error("whatever...");
	
	typedef ExceptionImpl<std::bad_cast> bad_cast;
	typedef ExceptionImpl<std::bad_alloc> bad_alloc;
	typedef ExceptionImpl<std::bad_array_new_length> bad_array_new_length;
	typedef ExceptionImpl<std::bad_exception> bad_exception;
	typedef ExceptionImpl<std::bad_function_call> bad_function_call;
	typedef ExceptionImpl<std::invalid_argument> bad_typeid;
	typedef ExceptionImpl<std::invalid_argument> bad_weak_ptr;

	typedef ExceptionImpl<std::logic_error> logic_error;
	typedef ExceptionImpl<std::out_of_range> out_of_range;
	typedef ExceptionImpl<std::domain_error> domain_error;
	typedef ExceptionImpl<std::future_error> future_error;
	typedef ExceptionImpl<std::invalid_argument> invalid_argument;
	typedef ExceptionImpl<std::length_error> length_error;

	typedef ExceptionImpl<std::runtime_error> runtime_error;
	typedef ExceptionImpl<std::overflow_error> overflow_error;
	typedef ExceptionImpl<std::range_error> range_error;
	typedef ExceptionImpl<std::system_error> system_error;
	typedef ExceptionImpl<std::underflow_error> underflow_error;

}

#endif // HEADER_GUARD 
