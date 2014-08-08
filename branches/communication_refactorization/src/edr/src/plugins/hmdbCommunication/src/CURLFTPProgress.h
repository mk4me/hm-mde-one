/********************************************************************
	created:  2014/05/28	20:36:21
	filename: CURLFTPProgress.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__

#include <networkUtils/CURLFTPHelper.h>
#include <boost/atomic.hpp>

namespace hmdbCommunication
{
	class CURLFTPProgress : public networkUtils::ICURLFTPProgress
	{
	public:
		//! Domyslny konsturktor
		CURLFTPProgress();
		//! Destruktor wirtualny
		virtual ~CURLFTPProgress();
		//! Przerywa operacjê
		void abort();
		//! \return Czy operacja ma byæ przerwana
		virtual const bool aborted() const;
		//! \param progress Postêp operacji
		virtual void setProgress(const float progress);
		//! \return Postêp operacji
		const float progress() const;

	private:
		//! Czy operacja ma byæ przerwana
		boost::atomic<bool> aborted_;
		//! Postêp operacji
		boost::atomic<float> progress_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__