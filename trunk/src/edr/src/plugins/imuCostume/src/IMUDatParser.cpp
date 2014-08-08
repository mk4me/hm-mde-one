#include "PCH.h"
#include "IMUDatParser.h"
#include <fstream>
#include <array>


#define CHUNK_SIZE 1024
#define FRAME_SIZE 32

using namespace std;

IMU::Frames IMU::IMUDatParser::parse(const core::Filesystem::Path& path, int imusCount)
{
    //array<char, CHUNK_SIZE> chunk;
    array<uint16_t, CHUNK_SIZE> chunk;
    Frames frames;
    ifstream file(path.c_str(), ios::in | ios::binary);
    while (file.read((char*)chunk.data(), CHUNK_SIZE)) {
        Frame f;
        for (uint8_t i = 0; i < imusCount; i++) {
            ImuData imuData;

            
            // przeskakujemy o 32 bajty (16 int�w), do kolejnej ramki 
            int offs = 2 * 8 * i;
            imuData.raw.acc_x = chunk[offs + 0];
            imuData.raw.acc_y = chunk[offs + 1];
            imuData.raw.acc_z = chunk[offs + 2];

            imuData.raw.mag_x = chunk[offs + 4];
            imuData.raw.mag_y = chunk[offs + 5];
            imuData.raw.mag_z = chunk[offs + 6];

            imuData.raw.rate_x = chunk[offs + 8];
            imuData.raw.rate_y = chunk[offs + 9];
            imuData.raw.rate_z = chunk[offs + 10];

            imuData.qx = chunk[offs + 12];
            imuData.qy = chunk[offs + 13];
            imuData.qz = chunk[offs + 14];
            imuData.qw = chunk[offs + 15];
            /*int offs = 4 * 8 * i;
            imuData.raw.acc_x = int16_t((uint16_t(chunk[offs + 1]) << 8) | uint16_t(chunk[offs + 0]));
            imuData.raw.acc_y = int16_t((uint16_t(chunk[offs + 3]) << 8) | uint16_t(chunk[offs + 2]));
            imuData.raw.acc_z = int16_t((uint16_t(chunk[offs + 5]) << 8) | uint16_t(chunk[offs + 4]));
            offs += 8;                                                              
            imuData.raw.mag_x = int16_t((uint16_t(chunk[offs + 1]) << 8) | uint16_t(chunk[offs + 0]));
            imuData.raw.mag_y = int16_t((uint16_t(chunk[offs + 3]) << 8) | uint16_t(chunk[offs + 2]));
            imuData.raw.mag_z = int16_t((uint16_t(chunk[offs + 5]) << 8) | uint16_t(chunk[offs + 4]));
            offs += 8;                                                              
            imuData.raw.rate_x = int16_t((uint16_t(chunk[offs + 1]) << 8) | uint16_t(chunk[offs + 0]));
            imuData.raw.rate_y = int16_t((uint16_t(chunk[offs + 3]) << 8) | uint16_t(chunk[offs + 2]));
            imuData.raw.rate_z = int16_t((uint16_t(chunk[offs + 5]) << 8) | uint16_t(chunk[offs + 4]));
            offs += 8;
            
            imuData.qx = int16_t((uint16_t(chunk[offs + 1]) << 8) | uint16_t(chunk[offs + 0]));
            imuData.qy = int16_t((uint16_t(chunk[offs + 3]) << 8) | uint16_t(chunk[offs + 2]));
            imuData.qz = int16_t((uint16_t(chunk[offs + 5]) << 8) | uint16_t(chunk[offs + 4]));
            imuData.qw = int16_t((uint16_t(chunk[offs + 7]) << 8) | uint16_t(chunk[offs + 6]));*/
            f.push_back(imuData);
        }
        frames.push_back(f);
    }
    return frames;
}

std::pair<IMU::Frames, int> IMU::IMUDatParser::parse(const core::Filesystem::Path& path)
{
	array<uint16_t, CHUNK_SIZE> chunk;
	Frames frames;
	ifstream file(path.c_str(), ios::in | ios::binary);
	if (file.read((char*)chunk.data(), CHUNK_SIZE)) {
		// chunkS / frameS da maksymalna liczbe czujnikow
		auto maxCount = CHUNK_SIZE / FRAME_SIZE;
		for (uint8_t i = 0; i < maxCount; i++) {
			ImuData imuData;

			// przeskakujemy o 32 bajty (16 int�w), do kolejnej ramki 
			int offs = 2 * 8 * i;
			bool zeros = true;
			for (auto s = 0; s < FRAME_SIZE / 2; ++s) {
				if (chunk[offs + 0] != 0) {
					zeros = false;
					break;
				}
			}
			
			if (zeros) {
				// znamy juz liczbe czujnikow
				return std::make_pair(parse(path, i), i);
			}
		}
		return std::make_pair(parse(path, maxCount), maxCount);
	}

	throw std::runtime_error("Unable to load file");
}

void IMU::IMUDatParser::save(const core::Filesystem::Path& path, const Frames& frames)
{
    using namespace std;
    array<uint16_t, CHUNK_SIZE / sizeof(uint16_t)> chunk = { 0 };
    ofstream file(path.c_str(), ios::out | ios::binary);
    
    for (auto it = frames.begin(); it != frames.end(); ++it) {
        const Frame& f = *it;
        int count = f.size();
        for (int i = 0; i < count; ++i) {
            const ImuData& imuData = f[i];

            //il.ramek, dl. ramki
            //offs = 4 * 8 * (ids_array[i] - 1);
            int offs = 2 * 8 * i;
            chunk[offs + 0] = imuData.raw.acc_x;
            chunk[offs + 1] = imuData.raw.acc_y;
            chunk[offs + 2] = imuData.raw.acc_z;

            chunk[offs + 4] = imuData.raw.mag_x;
            chunk[offs + 5] = imuData.raw.mag_y;
            chunk[offs + 6] = imuData.raw.mag_z;

            chunk[offs + 8] = imuData.raw.rate_x;
            chunk[offs + 9] = imuData.raw.rate_y;
            chunk[offs + 10] = imuData.raw.rate_z;

            chunk[offs + 12] = imuData.qx;
            chunk[offs + 13] = imuData.qy;
            chunk[offs + 14] = imuData.qz;
            chunk[offs + 15] = imuData.qw;
            
        }
        file.write((char*)chunk.data(), CHUNK_SIZE);
    }

    file.close();;
}

std::vector<IMU::Frames> IMU::IMUDatSplitter::split(const Frames& frames, const std::vector<std::pair<int, int>>& splits)
{
    std::vector<Frames> framesVector;
    for (auto it = splits.begin(); it != splits.end(); ++it) {
        int start = it->first;
        int end = it->second;
        if (end <= start) {
            throw std::runtime_error("Invalid split format");
        }
        Frames f;
        for (int i = start; i <= end; ++i) {
            f.push_back(frames[i]);
        }
        framesVector.push_back(f);
    }

    return framesVector;
}
