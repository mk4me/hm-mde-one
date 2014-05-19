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
#include <hmdbserviceslib/Entity.h>
#include <corelib/HierarchyDataItem.h>

namespace dicom
{
	class IDicomService : public plugin::IService
	{
	public:

		struct AnnotationStatus {
			hmdbServices::xmlWsdl::AnnotationStatus::Type status;
			std::string comment;
			std::string note;
		};

	public:
		virtual ~IDicomService() {}

		virtual const AnnotationStatus annotationStatus(const std::string & user, const int trialID) const = 0;

		virtual void setAnnotationStatus(const std::string & user, const int trialID,
			const hmdbServices::xmlWsdl::AnnotationStatus::Type status,
			const std::string & comment) = 0;

		virtual void updateItemIcon(const std::string& filename, const QIcon& icon) = 0;
	};
}

#endif	//	HEADER_GUARD_DICOM__IDICOMSERVICE_H__
