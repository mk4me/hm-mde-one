/********************************************************************
	created:	2014/06/20
	created:	20:6:2014   11:10
	filename: 	IMUDatParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMU__IMUDATPARSER_H__
#define HEADER_GUARD_IMU__IMUDATPARSER_H__

#include <cstdint> //TODO nie wiem czy to na linuxie przejdzie, jesli nie to trzeba zrobic ifdefa na stdint.h
#include <vector>
#include <utils/SmartPtr.h>
#include <corelib/Filesystem.h>
#include "IMUCFGParser.h"

namespace IMU {

    struct ImuRawData {
        int16_t acc_x;
        int16_t acc_y;
        int16_t acc_z;
        int16_t rate_x;
        int16_t rate_y;
        int16_t rate_z;
        int16_t mag_x;
        int16_t mag_y;
        int16_t mag_z;
    };

    struct ImuData {
        ImuRawData raw;
        int16_t qx;
        int16_t qy;
        int16_t qz;
        int16_t qw;
    };

    typedef  std::vector<ImuData> Frame;
    
    typedef std::vector<Frame> Frames;
    
    class IMUDatParser
    {
    public:
        virtual ~IMUDatParser() {}
		static Frames parse(const core::Filesystem::Path& path, int imusCount);
		static std::pair<Frames,int> parse(const core::Filesystem::Path& path);
        static void save(const core::Filesystem::Path& path, const Frames& frames);
    };
    DEFINE_SMART_POINTERS(IMUDatParser);


    class IMUDatSplitter
    {
    public:
        static std::vector<Frames> split(const Frames& frames, const std::vector<std::pair<int,int>>& splits);
    };

}
#endif
