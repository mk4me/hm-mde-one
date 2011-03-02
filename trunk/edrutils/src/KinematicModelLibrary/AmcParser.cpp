#include <KinematicModelLibrary/AmcParser.h>

#include <KinematicModelLibrary\SkeletalParsers.h>

using namespace kinematic;
using namespace boost;
//----------------------------------------------------------------------------------
AmcParser::AmcParser() : forceRootXYZ(true) {
}
//----------------------------------------------------------------------------------
void AmcParser::save (const SkeletalModel::Ptr model , const std::string& filename) {
    std::ofstream out;
    out.open(filename.c_str());

    out << ":FULLY-SPECIFIED" << std::endl;
    out << ":DEGREES" << std::endl;

    std::vector<SkeletalModel::singleFrame>& frames = model->getFrames();

    int count = frames.size();
    for (int i = 0; i < count; ++i) {
        out << frames[i].frameNo << std::endl;
        std::vector<SkeletalModel::singleBoneState>& bones = frames[i].bonesData;
        int bonesCount = bones.size();
        for (int j = 0; j < bonesCount; j++) {
            out << bones[j].name;
             if (bones[j].name == "root" && forceRootXYZ) {
                 std::vector<DegreeOfFreedom>& dofs = model->getSkeleton().getRoot()->dofs;
                 int index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TX, dofs);
                 out << " " << bones[j].channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TY, dofs);
                 out << " " << bones[j].channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TZ, dofs);
                 out << " " << bones[j].channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, dofs);
                 out << " " << bones[j].channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, dofs);
                 out << " " << bones[j].channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, dofs);
                 out << " " << bones[j].channelValues[index];
             } else {
            for (unsigned int k = 0; k < bones[j].channelValues.size(); k++) {
                out << " " << bones[j].channelValues[k];
            }}

            out << std::endl;
        }
    }
    out.close();
}
//----------------------------------------------------------------------------------
void AmcParser::parse(SkeletalModel::Ptr model, const std::string& filename ) {
    std::vector<double> numbers;
    std::string name;

    std::string::size_type loc;
    
    std::ifstream ifs( filename.c_str() );

    if (!ifs) {
        throw UnableToOpenFileException(filename);
    }

    std::string line;

    std::vector<SkeletalModel::singleFrame>& frames = model->getFrames();

    // HACK naglowki
    while( getline( ifs, line ) ) 
    {
        loc = line.find(":DEGREES");
        if( loc != std::string::npos )
        {
            break;
        }		
    }
 
    SkeletalModel::singleBoneState bone;
    
    SkeletalModel::singleFrame frame;
    while( getline( ifs, line ) ) {
        std::istringstream iss(line);
        int frameNo = -1;
        if (iss >> frameNo){
            if (frame.bonesData.size() > 0) {
                frames.push_back(frame);
            }
            frame.bonesData.clear();
            frame.frameNo = frameNo;
        } else {
            iss.clear();
            iss >> bone.name;
            bone.channelValues.clear();
            double val = 0.0;
            while (iss >> val) {
                bone.channelValues.push_back(val);
            }
            frame.bonesData.push_back(bone);
        }
     }		
     frames.push_back(frame);

     if (frames.size() == 0) {
         throw AcclaimWrongFileException("No frames defined in " + filename + " file");
     }

     ifs.close();
}

