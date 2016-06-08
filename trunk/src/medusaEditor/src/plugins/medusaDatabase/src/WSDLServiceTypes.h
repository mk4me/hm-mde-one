/********************************************************************
	created:  2014/05/30	10:06:30
	filename: WSDLServiceTypes.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_medusaDatabase__WSDLSERVICETYPES_H__
#define __HEADER_GUARD_medusaDatabase__WSDLSERVICETYPES_H__

#include <functional>
#include <networkUtils/IWSDLService.h>

namespace medusaDatabase
{
	typedef std::function<networkUtils::IWSDLServicePtr()> WSDLServiceCreator;	
}

#endif	// __HEADER_GUARD_medusaDatabase__WSDLSERVICETYPES_H__
