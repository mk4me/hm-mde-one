/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:04
	filename: 	ExporterModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__EXPORTERMODEL_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__EXPORTERMODEL_H__


#include "IExporter.h"

namespace medusaExporter {

	//! Serce exportera danych MEDUSA
	class ExporterModel
	{
	public:
		virtual ~ExporterModel() {}

	public:
		void addExporter(const IExporterConstPtr& exporter);

	private:
		std::vector<IExporterConstPtr> exporters;
	};
	DEFINE_SMART_POINTERS(ExporterModel);
}

#endif
