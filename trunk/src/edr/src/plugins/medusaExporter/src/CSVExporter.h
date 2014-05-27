/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:04
	filename: 	CSVExporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__CSVEXPORTER_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__CSVEXPORTER_H__


#include "IExporter.h"

namespace medusaExporter {

	class CSVExporter : public IExporter
	{
	public:
		virtual ~CSVExporter() {}

	public:
		virtual void exportAnnotations(const core::Filesystem::Path& path, const AnnotationData& data, const ExportConfig& config) const;

	};
	DEFINE_SMART_POINTERS(CSVExporter);
}

#endif
