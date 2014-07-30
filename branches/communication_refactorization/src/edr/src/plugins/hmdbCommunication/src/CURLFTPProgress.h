/********************************************************************
	created:  2014/05/28	20:36:21
	filename: CURLFTPProgress.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__

#include <networkUtils/CURLFTPHelper.h>

namespace hmdbCommunication
{
	class CURLFTPProgress : public networkUtils::ICURLFTPProgress
	{
	public:
		//! Domyslny konsturktor
		CURLFTPProgress();
		//! Destruktor wirtualny
		virtual ~CURLFTPProgress();
		//! Przerywa operacj�
		void abort();
		//! \return Czy operacja ma by� przerwana
		virtual const bool aborted() const;
		//! \param progress Post�p operacji
		virtual void setProgress(const float progress);
		//! \return Post�p operacji
		const float progress() const;

	private:
		//! Czy operacja ma by� przerwana
		volatile bool aborted_;
		//! Post�p operacji
		volatile float progress_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__