/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:02
	filename: 	AnnotationData.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__ANNOTATIONDATA_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__ANNOTATIONDATA_H__

namespace medusaExporter {

	class AnnotationData
	{
	public:
		virtual ~AnnotationData() {}
	};
	DEFINE_SMART_POINTERS(AnnotationData);

}
#endif
