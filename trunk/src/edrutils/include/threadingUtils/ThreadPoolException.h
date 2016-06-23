/********************************************************************
	created:  2014/10/28	16:13:30
	filename: ThreadPoolException.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__THREADPOOLEXCEPTION_H__
#define __HEADER_GUARD_THREADINGUTILS__THREADPOOLEXCEPTION_H__

#include <stdexcept>

namespace threadingUtils
{
	//! Wyj�tek rzucany kiedy chcemy pobra� w�tek/grup� z managera a nie ma ju� �adnych wolnych lub ilo�� wolnych jest niewystarczaj�ca (dla grupy)
	class ThreadPoolEmptyException : public std::runtime_error
	{
	public:
		ThreadPoolEmptyException(const char * msg) : std::runtime_error(msg) {}
		ThreadPoolEmptyException(const std::string & msg) : std::runtime_error(msg) {}
		virtual ~ThreadPoolEmptyException() {}
	};

	//! Wyj�tek rzucany kiedy chcemy pobra� w�tek/grup� z managera a nie ma ju� �adnych wolnych lub ilo�� wolnych jest niewystarczaj�ca (dla grupy)
	class ThreadPoolConfigurationException : public std::runtime_error
	{
	public:
		ThreadPoolConfigurationException(const char * msg) : std::runtime_error(msg) {}
		ThreadPoolConfigurationException(const std::string & msg) : std::runtime_error(msg) {}
		virtual ~ThreadPoolConfigurationException() {}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__THREADPOOLEXCEPTION_H__