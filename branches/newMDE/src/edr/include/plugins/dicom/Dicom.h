/********************************************************************
	created:	2013/08/12
	created:	12:8:2013   22:54
	filename: 	Dicom.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOM_H__
#define HEADER_GUARD_DICOM__DICOM_H__

#include <utils/ObjectWrapper.h>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmimgle/dcmimage.h>


DEFINE_SMART_POINTERS(DicomImage);
DEFINE_WRAPPER(DicomImage, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);

DEFINE_SMART_POINTERS(DcmDataset);
DEFINE_WRAPPER(DcmDataset, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);

#endif
