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
	class Exception : public BaseException, private ExceptionLogger
	{
		//static_assert((std::is_base_of<std::exception, Exception>::value), "Base class should inherit from std::exception");
	public:
		explicit Exception(const std::string& message) : BaseException(message){

		}

		//! destruktor wykonuje faktyczne logowanie
		virtual ~Exception() {
			if (ExceptionLogger::_log) {
				LOG_DEBUG(ExceptionLogger::_log, "First chance exception: " << BaseException::what());
			}
		}
	};

	// rozszerzone wyj¹tki z stl-a, u¿ycie: throw core::runtime_error("whatever...");
	
	typedef Exception<std::bad_cast> bad_cast;
	typedef Exception<std::bad_alloc> bad_alloc;
	typedef Exception<std::bad_array_new_length> bad_array_new_length;
	typedef Exception<std::bad_exception> bad_exception;
	typedef Exception<std::bad_function_call> bad_function_call;
	typedef Exception<std::invalid_argument> bad_typeid;
	typedef Exception<std::invalid_argument> bad_weak_ptr;

	typedef Exception<std::logic_error> logic_error;
	typedef Exception<std::out_of_range> out_of_range;
	typedef Exception<std::domain_error> domain_error;
	typedef Exception<std::future_error> future_error;
	typedef Exception<std::invalid_argument> invalid_argument;
	typedef Exception<std::length_error> length_error;

	typedef Exception<std::runtime_error> runtime_error;
	typedef Exception<std::overflow_error> overflow_error;
	typedef Exception<std::range_error> range_error;
	typedef Exception<std::system_error> system_error;
	typedef Exception<std::underflow_error> underflow_error;

}

#endif // HEADER_GUARD 
