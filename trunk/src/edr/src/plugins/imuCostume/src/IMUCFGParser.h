
/********************************************************************
	created:	2014/06/20
	created:	20:6:2014   11:10
	filename: 	IMUCFGParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMU__IMUCFGPARSER_H__
#define HEADER_GUARD_IMU__IMUCFGPARSER_H__

#include <utils/SmartPtr.h>
#include <vector>
#include <osg/Quat>

namespace IMU {
    struct IMUConfig {
		IMUConfig() : imusCount(-1), imuID(-1), param1(-1), param2(-1) {}
        struct Joint {
            std::vector<osg::Quat> offsets;
            std::string name;
            int idx;
            int parentIdx;
        };
        int imusCount;
        int imuID;
        int param1;
        int param2;
        std::vector<Joint> joints;
    };
	DEFINE_SMART_POINTERS(IMUConfig);

    class IMUCFGParser
    {
    public:
        virtual ~IMUCFGParser() {}
        static IMUConfigPtr parse(const std::string& path);
    }; 

}

#endif
