/********************************************************************
	created:  2015/10/01	15:11:01
	filename: CURLUtils.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__CURLUTILS_H__
#define __HEADER_GUARD_NETWORKUTILS__CURLUTILS_H__

#include <curl/curl.h>
#include <utils/SmartPtr.h>

namespace networkUtils
{
	DEFINE_SMART_POINTERS(CURLM);
	DEFINE_SMART_POINTERS(CURL);

	inline CURLPtr wrapCurlEasy(CURL * curl)
	{
		return CURLPtr(curl, &curl_easy_cleanup);
	}

	inline CURLMPtr wrapCurlMulti(CURLM * curl)
	{
		return CURLMPtr(curl, &curl_multi_cleanup);
	}

	inline CURLPtr createCurlEasy()
	{
		return wrapCurlEasy(curl_easy_init());
	}

	inline CURLMPtr createCurlMulti()
	{
		return wrapCurlMulti(curl_multi_init());
	}
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLUTILS_H__