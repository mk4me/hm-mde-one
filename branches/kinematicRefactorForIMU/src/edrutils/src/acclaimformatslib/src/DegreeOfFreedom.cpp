#include <acclaimformatslib/DegreeOfFreedom.h>
#include <algorithm>
#include <cctype>

using namespace acclaim;

std::string Axis::getAxisOrderName( kinematicUtils::AxisOrder order ) {
    switch(order){
        case kinematicUtils::XYZ : return "XYZ";
        case kinematicUtils::XZY : return "XZY";
        case kinematicUtils::YXZ : return "YXZ";
        case kinematicUtils::YZX : return "YZX";
        case kinematicUtils::ZXY : return "ZXY";
        case kinematicUtils::ZYX : return "ZYX";
        default:
            throw std::runtime_error("Unsupported acclaim axis order");
    }
}

int DegreeOfFreedom::getChannelIndex(kinematicUtils::Channel channel, const std::vector<DegreeOfFreedom>& dofs) {
    for (int i = dofs.size() - 1; i >= 0; --i) {
        if (dofs[i].channel == channel) {
            return i;
        }
    }
    return -1;
}

kinematicUtils::AxisOrder Axis::getAxisOrder(const std::string& axis) {
    std::string s(axis);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "XYZ") {
		return kinematicUtils::XYZ;
    } else if (s == "XZY") {
		return kinematicUtils::XZY;
    } else if (s == "YXZ") {
		return kinematicUtils::XZY;
    } else if (s == "YZX") {
		return kinematicUtils::YZX;
    } else if (s == "ZXY") {
		return kinematicUtils::ZXY;
    } else if (s == "ZYX") {
		return kinematicUtils::ZYX;
    } else {
		throw std::runtime_error("Unsupported acclaim axis order");
    }
}

kinematicUtils::Channel DegreeOfFreedom::getChannel( const std::string& channel ) {
    std::string s(channel);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "TX") {
		return (kinematicUtils::TX);
    } else if (s == "TY") {
		return (kinematicUtils::TY);
    } else if (s == "TZ") {
		return (kinematicUtils::TZ);
    } else if (s == "RX") {
		return (kinematicUtils::RX);
    } else if (s == "RY") {
		return (kinematicUtils::RY);
    } else if (s == "RZ") {
		return (kinematicUtils::RZ);
    } else if (s == "L") {
        return (DegreeOfFreedom::L);
    } else {
		throw std::runtime_error("Unsupported acclaim data channel");
    }
}

std::string DegreeOfFreedom::getChannelName( kinematicUtils::Channel channel, bool uppercase) {
    switch (channel) {
        case kinematicUtils::TX : return uppercase ? "TX" : "tx";
        case kinematicUtils::TY : return uppercase ? "TY" : "ty";
        case kinematicUtils::TZ : return uppercase ? "TZ" : "tz";
        case kinematicUtils::RX : return uppercase ? "RX" : "rx";
        case kinematicUtils::RY : return uppercase ? "RY" : "ry";
        case kinematicUtils::RZ : return uppercase ? "RZ" : "rz";
        case DegreeOfFreedom::L  : return uppercase ? "L"  : "l";
    }
	throw std::runtime_error("Unsupported acclaim data channel");
}
