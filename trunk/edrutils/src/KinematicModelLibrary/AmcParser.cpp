#include <kinematiclib/AmcParser.h>
#include <kinematiclib/SkeletalParsers.h>

using namespace kinematic;
using namespace boost;
using namespace std;

AmcParser::AmcParser() : 
    forceRootXYZ(true)
{
}

void AmcParser::save (const SkeletalModelPtr model, const std::string& filename)
{
    std::ofstream out;
    out.open(filename.c_str());

    out << ":FULLY-SPECIFIED" << std::endl;
    out << ":DEGREES" << std::endl;

    std::vector<SkeletalModel::singleFramePtr>& frames = model->getFrames();

    int count = frames.size();
    for (int i = 0; i < count; ++i) {
        out << frames[i]->frameNo << std::endl;
        std::vector<SkeletalModel::singleBoneState>& bones = frames[i]->bonesData;
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

void AmcParser::parse(SkeletalModelPtr model, const std::string& filename )
{
    std::vector<double> numbers;
    std::string name;
    std::string::size_type loc;

    std::ifstream ifs( filename.c_str() );

    if (!ifs) {
        throw UnableToOpenFileException(filename);
    }
    
    std::string line;

    std::vector<SkeletalModel::singleFramePtr>& frames = model->getFrames();

    // HACK: z braku dokladnej dokumentacji nie wiem jakie moga wystapic naglowki pliku *.bhv
    // wszystkie do tej pory spotkane pliki konczyly naglowek linijka :DEGREES,
    while (getline(ifs, line)) {
        loc = line.find(":DEGREES");
        if (loc != std::string::npos) {
            break;
        }
    }

    SkeletalModel::singleBoneState bone;
    
    SkeletalModel::singleFramePtr frame(new SkeletalModel::singleFrame);
    while (getline(ifs, line)) {
        std::istringstream iss(line);
        int frameNo = -1;
        if (iss >> frameNo){
            if (frame->bonesData.size() > 0) {
                frames.push_back(frame);
            }
            
            frame = SkeletalModel::singleFramePtr(new SkeletalModel::singleFrame);
            frame->frameNo = frameNo;
        } else {
            iss.clear();
            iss >> bone.name;
            bone.channelValues.clear();
            
            double val = 0.0;
            while (iss >> val) {
                bone.channelValues.push_back(val);
            }
            frame->bonesData.push_back(bone);
        }
     }
     frames.push_back(frame);

    ifs.close();
    if (frames.size() == 0) {
        throw WrongFileException("No frames defined in " + filename + " file");
    }
}
