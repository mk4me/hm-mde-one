/********************************************************************
    created:  2014/04/01
    created:  1:4:2014   8:05
    filename: IDicomService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_DICOM__IDICOMSERVICE_H__
#define HEADER_GUARD_DICOM__IDICOMSERVICE_H__

#include <string>
#include <corelib/IService.h>
#include <webserviceslib/Entity.h>

namespace dicom
{

	class IDicomService : public plugin::IService
	{
	public:
		virtual ~IDicomService() {}

		virtual const webservices::xmlWsdl::AnnotationStatus::Type annotationStatus(const std::string & user, const int trialID) const = 0;

		virtual void setAnnotationStatus(const std::string & user, const int trialID,
			const webservices::xmlWsdl::AnnotationStatus::Type status,
			const std::string & comment) = 0;
	};

}

#endif	//	HEADER_GUARD_DICOM__IDICOMSERVICE_H__
