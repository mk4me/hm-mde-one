/********************************************************************
	created:  2014/05/28	21:42:40
	filename: CURLFTPStatus.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPSTATUS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPSTATUS_H__

#include <threadingUtils/IOperation.h>
#include <atomic>

namespace hmdbCommunication
{
	class CURLFTPStatus
	{
	public:
		//! Konstruktor domyslny
		CURLFTPStatus();
		//! Destruktor
		~CURLFTPStatus();
		//! \return Status
		const threadingUtils::IOperation::Status status() const;
		//! \param status Status
		void setStatus(const threadingUtils::IOperation::Status status);
		//! \param error Opis b��du
		void setError(const std::string & error);
		//! \return Opis b��du
		const std::string & error() const;

	private:
		//! Status
		std::atomic<threadingUtils::IOperation::Status> status_;
		//! Opis b��du
		std::string error_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPSTATUS_H__