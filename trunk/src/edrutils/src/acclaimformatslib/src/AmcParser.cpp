#include <acclaimformatslib/AmcParser.h>
#include <utils/Debug.h>
#include <fstream>
#include <sstream>

using namespace acclaim;

void AmcParser::serialize(const MotionData & data,
	std::ostream& out)
{
    out << ":FULLY-SPECIFIED" << std::endl;
    out << ":DEGREES" << std::endl;

	auto framesSize = data.frames.size();
	for (unsigned int i = 0; i < framesSize; ++i) {
		out << data.frames[i].id << std::endl;                
		for (unsigned int j = 0; j < data.frames[i].bonesData.size(); ++j) {
			out << data.frames[i].bonesData[j].name;
			for (unsigned int k = 0; k < data.frames[i].bonesData[j].channelValues.size(); ++k) {
				out << " " << data.frames[i].bonesData[j].channelValues[k];
			}

            out << std::endl;
        }
    }
}

void AmcParser::parse(MotionData & data, std::istream & stream)
{
    std::string::size_type loc;    
    std::string line;

    // HACK: z braku dokładnej dokumentacji nie wiem jakie moga wystapic nagłówki pliku *.amc
    // wszystkie do tej pory spotkane pliki konczyly nagłówek linijka :DEGREES,
	// rozwiązanie stosowane jest w innych parserach
    while (getline(stream, line)) {
        loc = line.find(":DEGREES");
        if (loc != std::string::npos) {
            break;
        }
    }

	std::vector<MotionData::FrameData> frames;
	MotionData::FrameData* frame = nullptr;
	MotionData locData;

    while (getline(stream, line)) {
        std::istringstream iss(line);
        int frameNo = -1;
        if (iss >> frameNo) {
			frames.push_back(MotionData::FrameData());
			frame = &frames.back();
            frame->id = frameNo;
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

				frame->bonesData.push_back(boneData);
			}
        }
    }

    if (frames.empty() == true) {
		throw std::runtime_error("No frames in stream");
    }

	data.frames = frames;
}
