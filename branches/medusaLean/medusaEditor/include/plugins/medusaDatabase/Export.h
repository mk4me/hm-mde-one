/********************************************************************
	created:	2013/07/26
	created:	26:7:2013   8:49
	filename: 	Export.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWCOMMUNICATION__EXPORT_H__
#define HEADER_GUARD_NEWCOMMUNICATION__EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_medusaDatabase_EXPORTS)
#  define medusaDatabase_EXPORT UTILS_DECL_EXPORT
#else
#  define medusaDatabase_EXPORT UTILS_DECL_IMPORT
#endif


#endif
