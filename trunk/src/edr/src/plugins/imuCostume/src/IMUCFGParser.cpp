#include "PCH.h"
#include "IMUCFGParser.h"
#include <fstream>
#include <utils/Debug.h>
#include <kinematiclib/Skeleton.h>

IMU::IMUConfigPtr IMU::IMUCFGParser::parse(const std::string& path)
{
    IMUConfig cfg;
    std::ifstream file;
    file.open(path);
    file >> cfg.imusCount >> cfg.imuID >> cfg.param1 >> cfg.param2;
    
    for (int i = 0; i < cfg.imusCount; ++i) {
        IMUConfig::Joint joint;
        int offsetsCount;
        file >> joint.idx >> joint.parentIdx >> joint.name >> offsetsCount;
        
        for (int j = 0; j < offsetsCount; ++j) {
            char c;
            file >> c;
            if (c == 'q') {
                float r, x, y, z;
                file >> r >> x >> y >> z;
                joint.offsets.push_back(osg::Quat(r, x, y, z));
            } else if (c == 'e') {
                float x, y, z;
                file >> x >> y >> z;
                joint.offsets.push_back(kinematic::SkeletonUtils::getQuatFromEuler(x, y, z));
            }
        }

        cfg.joints.push_back(joint);
    }

    file.close();

    return utils::make_shared<IMUConfig>(cfg);
}
