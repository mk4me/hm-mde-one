// dae2Motion.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "dae2Motion.h"
#include "DAE2MotionWriter.h"


// This is an example of an exported variable
DAE2MOTION_API int ndae2Motion=0;

// This is an example of an exported function.
DAE2MOTION_API bool Convert2Motion(SDea2Motion* data)
{
    CDAE2MotionWriter* writer = new CDAE2MotionWriter(data);
    writer->write();
    delete writer;
	return true;
}

// This is the constructor of a class that has been exported.
// see dae2Motion.h for the class definition
Cdae2Motion::Cdae2Motion()
{
	return;
}
