/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   19:08
	filename: 	DFPins.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DFELEMENTS__HMMPINS_H__
#define HEADER_GUARD_DFELEMENTS__HMMPINS_H__

//#include <plugins/newVdf/NewVdfService.h>
//#include <plugins/newVdf/INodeConfiguration.h>
#include <plugins/c3d/C3DChannels.h>
//#include <plugins/dfElements/Export.h>
//#include <plugins/newVdf/IDataFlowProvider.h>
#include <plugins/newVdf/UniversalInputPin.h>
#include <plugins/newVdf/UniversalOutputPin.h>

typedef vdf::UniversalInputPinT<ScalarChannelReaderInterface> ScalarInputPin;
typedef vdf::UniversalOutputPinT<ScalarChannelReaderInterface> ScalarOutputPin;

typedef vdf::UniversalInputPinT<VectorChannelReaderInterface> VectorInputPin;
typedef vdf::UniversalOutputPinT<VectorChannelReaderInterface> VectorOutputPin;


#endif
