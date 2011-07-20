#include "stdafx.h"
#include <kinematiclib/AmcParser.h>
#include <kinematiclib/SkeletalParsers.h>

using namespace kinematic;
using namespace boost;
using namespace std;

AmcParser::AmcParser() : 
    forceRootXYZ(true)
{
}

void AmcParser::save (const SkeletalModelConstPtr model, const SkeletalDataConstPtr data, const std::string& filename)
{
    std::ofstream out;
    out.open(filename.c_str());

    out << ":FULLY-SPECIFIED" << std::endl;
    out << ":DEGREES" << std::endl;

    const std::vector<SkeletalData::singleFramePtr>& frames = data->getFrames();

    int count = frames.size();
    for (int i = 0; i < count; ++i) {
        out << frames[i]->frameNo << std::endl;
        std::vector<SkeletalData::singleJointStatePtr>& joints = frames[i]->jointsData;
        int bonesCount = joints.size();
        for (int j = 0; j < bonesCount; j++) {
            out << joints[j]->name;
             if (joints[j]->name == "root" && forceRootXYZ) {
                 const std::vector<DegreeOfFreedom>& dofs = model->getSkeleton().getRoot()->dofs;
                 int index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TX, dofs);
                 out << " " << joints[j]->channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TY, dofs);
                 out << " " << joints[j]->channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TZ, dofs);
                 out << " " << joints[j]->channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, dofs);
                 out << " " << joints[j]->channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, dofs);
                 out << " " << joints[j]->channelValues[index];
                 index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, dofs);
                 out << " " << joints[j]->channelValues[index];
             } else {
            for (unsigned int k = 0; k < joints[j]->channelValues.size(); k++) {
                out << " " << joints[j]->channelValues[k];
            }}

            out << std::endl;
        }
    }
    out.close();
}

void AmcParser::parse(SkeletalDataPtr data, const std::string& filename )
{
    std::vector<double> numbers;
    std::string name;
    std::string::size_type loc;

    std::ifstream ifs( filename.c_str() );

    if (!ifs) {
        throw UnableToOpenFileException(filename);
    }
    
    std::string line;

    std::vector<SkeletalData::singleFramePtr>& frames = data->getFrames();

    // HACK: z braku dokladnej dokumentacji nie wiem jakie moga wystapic naglowki pliku *.amc
    // wszystkie do tej pory spotkane pliki konczyly naglowek linijka :DEGREES,
	// rozwiazanie stosowane jest w innych parserach
    while (getline(ifs, line)) {
        loc = line.find(":DEGREES");
        if (loc != std::string::npos) {
            break;
        }
    }

    
    
    SkeletalData::singleFramePtr frame(new SkeletalData::singleFrame);
    while (getline(ifs, line)) {
        std::istringstream iss(line);
        int frameNo = -1;
        if (iss >> frameNo){
            if (frame->jointsData.size() > 0) {
                frames.push_back(frame);
            }
            
            frame = SkeletalData::singleFramePtr(new SkeletalData::singleFrame);
            frame->frameNo = frameNo;
        } else {
            iss.clear();
			SkeletalData::singleJointStatePtr jointState(new SkeletalData::singleJointState);
            iss >> jointState->name;
            jointState->channelValues.clear();
            
            double val = 0.0;
            while (iss >> val) {
                jointState->channelValues.push_back(val);
            }
            frame->jointsData.push_back(jointState);
        }
     }
     frames.push_back(frame);

    ifs.close();
    if (frames.size() == 0) {
        throw WrongFileException("No frames defined in " + filename + " file");
    }
}
