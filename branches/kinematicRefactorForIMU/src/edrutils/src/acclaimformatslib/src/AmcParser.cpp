#include <acclaimformatslib/AmcParser.h>
#include <fstream>
#include <sstream>

using namespace acclaim;

AmcParser::AmcParser() : 
    forceRootXYZ(true)
{
}

void AmcParser::save (const Skeleton & model, const MotionData & data, const std::string& filename)
{
    std::ofstream out;
    out.open(filename.c_str());

    out << ":FULLY-SPECIFIED" << std::endl;
    out << ":DEGREES" << std::endl;    

	for (int i = 0; i < data.frames.size(); ++i) {
		out << data.frames[i].id << std::endl;                
		for (int j = 0; j < data.frames[i].bonesData.size(); ++j) {
			out << data.frames[i].bonesData[j].name;
			if (data.frames[i].bonesData[j].name == "root" && forceRootXYZ == true) {
                 const std::vector<DegreeOfFreedom>& dofs = model.bones.find(model.root)->second.dofs;
				 int index = DegreeOfFreedom::getChannelIndex(kinematicUtils::TX, dofs);
				 out << " " << data.frames[i].bonesData[j].channelValues[index];
				 index = DegreeOfFreedom::getChannelIndex(kinematicUtils::TY, dofs);
				 out << " " << data.frames[i].bonesData[j].channelValues[index];
				 index = DegreeOfFreedom::getChannelIndex(kinematicUtils::TZ, dofs);
				 out << " " << data.frames[i].bonesData[j].channelValues[index];
				 index = DegreeOfFreedom::getChannelIndex(kinematicUtils::RX, dofs);
				 out << " " << data.frames[i].bonesData[j].channelValues[index];
				 index = DegreeOfFreedom::getChannelIndex(kinematicUtils::RY, dofs);
				 out << " " << data.frames[i].bonesData[j].channelValues[index];
				 index = DegreeOfFreedom::getChannelIndex(kinematicUtils::RZ, dofs);
				 out << " " << data.frames[i].bonesData[j].channelValues[index];
             } else {
				for (unsigned int k = 0; k < data.frames[i].bonesData[j].channelValues.size(); ++k) {
					out << " " << data.frames[i].bonesData[j].channelValues[k];
				}
			}

            out << std::endl;
        }
    }
    out.close();
}

void AmcParser::parse(MotionData & data, const std::string& filename )
{
    std::vector<double> numbers;
    std::string::size_type loc;

    std::ifstream ifs( filename.c_str() );

    if (!ifs) {
		throw std::runtime_error("Unable to open file: " + filename);
    }
    
    std::string line;

    // HACK: z braku dokładnej dokumentacji nie wiem jakie moga wystapic nagłówki pliku *.amc
    // wszystkie do tej pory spotkane pliki konczyly nagłówek linijka :DEGREES,
	// rozwiązanie stosowane jest w innych parserach
    while (getline(ifs, line)) {
        loc = line.find(":DEGREES");
        if (loc != std::string::npos) {
            break;
        }
    }

	std::vector<MotionData::FrameData> frames;
	MotionData::FrameData frame;
	MotionData locData;

    while (getline(ifs, line)) {
        std::istringstream iss(line);
        int frameNo = -1;
        if (iss >> frameNo){            
            frame.id = frameNo;
        } else {
            iss.clear();
			MotionData::BoneData boneData;
			iss >> boneData.name;
			if (!boneData.name.empty()) {
				boneData.channelValues.clear();

				double val = 0.0;
				while (iss >> val) {
					boneData.channelValues.push_back(val);
				}

				frame.bonesData.push_back(boneData);
			}
        }
     }
	frames.push_back(frame);

    ifs.close();
    if (frames.empty() == true) {
		throw std::runtime_error("No frames defined in " + filename + " file");
    }

	data.frames = frames;
}
