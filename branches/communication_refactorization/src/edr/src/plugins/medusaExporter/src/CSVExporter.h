/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:04
	filename: 	CSVExporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__CSVEXPORTER_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__CSVEXPORTER_H__

namespace medusaExporter {

	class CSVExporter
	{
	public:
		virtual ~CSVExporter() {}
	};
	DEFINE_SMART_POINTERS(CSVExporter);
}

#endif
