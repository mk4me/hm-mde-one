/********************************************************************
	created:  2014/05/28	20:36:21
	filename: CURLFTPProgress.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__CURLFTPPROGRESS_H__
#define __HEADER_GUARD_hmdbCommunication__CURLFTPPROGRESS_H__

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
		virtual bool aborted() const;
		//! \param progress Post�p operacji
		virtual void setProgress(const float progress);
		//! \param processedData Ilosc przetworzonych danych [bytes]
		virtual void setProcessedData(const std::size_t processedData);
		//! \return Post�p operacji
		float progress() const;
		//! \return Ilos� do tej pory przetworzonych danych
		std::size_t processedData() const;

	private:
		//! Czy operacja ma by� przerwana
		volatile bool aborted_;
		//! Post�p operacji
		volatile float progress_;
		//! Ilosc przetworzonych danych
		volatile std::size_t processedData_;
	};
}

#endif	// __HEADER_GUARD_hmdbCommunication__CURLFTPPROGRESS_H__
