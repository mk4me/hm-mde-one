/********************************************************************
	created:  2014/05/28	20:36:21
	filename: CURLFTPProgress.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__

#include <networkUtils/CURLFTPHelper.h>
#include <atomic>

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
		//! \param processedData Ilosc przetworzonych danych [bytes]
		virtual void setProcessedData(const unsigned long long processedData);
		//! \return Postêp operacji
		const float progress() const;
		//! \return Ilosæ do tej pory przetworzonych danych
		const unsigned long long processedData() const;

	private:
		//! Czy operacja ma byæ przerwana
		std::atomic<bool> aborted_;
		//! Postêp operacji
		std::atomic<float> progress_;
		//! Ilosc przetworzonych danych
		std::atomic<unsigned long long> processedData_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__