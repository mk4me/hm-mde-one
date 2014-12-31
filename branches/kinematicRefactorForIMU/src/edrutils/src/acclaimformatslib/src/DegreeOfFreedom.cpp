#include <acclaimformatslib/DegreeOfFreedom.h>
#include <algorithm>
#include <cctype>

using namespace acclaim;

std::string Axis::getAxisOrderName( kinematicUtils::AxisOrder::Type order ) {
    switch(order){
        case kinematicUtils::AxisOrder::XYZ : return "XYZ";
        case kinematicUtils::AxisOrder::XZY : return "XZY";
        case kinematicUtils::AxisOrder::YXZ : return "YXZ";
        case kinematicUtils::AxisOrder::YZX : return "YZX";
        case kinematicUtils::AxisOrder::ZXY : return "ZXY";
        case kinematicUtils::AxisOrder::ZYX : return "ZYX";
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

kinematicUtils::AxisOrder::Type Axis::getAxisOrder(const std::string& axis) {
    std::string s(axis);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "XYZ") {
		return kinematicUtils::AxisOrder::XYZ;
    } else if (s == "XZY") {
		return kinematicUtils::AxisOrder::XZY;
    } else if (s == "YXZ") {
		return kinematicUtils::AxisOrder::XZY;
    } else if (s == "YZX") {
		return kinematicUtils::AxisOrder::YZX;
    } else if (s == "ZXY") {
		return kinematicUtils::AxisOrder::ZXY;
    } else if (s == "ZYX") {
		return kinematicUtils::AxisOrder::ZYX;
    } else {
		throw std::runtime_error("Unsupported acclaim axis order");
    }
}

kinematicUtils::Channel DegreeOfFreedom::getChannel( const std::string& channel ) {
    std::string s(channel);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "TX") {
		return (kinematicUtils::ChannelType::TX);
    } else if (s == "TY") {
		return (kinematicUtils::ChannelType::TY);
    } else if (s == "TZ") {
		return (kinematicUtils::ChannelType::TZ);
    } else if (s == "RX") {
		return (kinematicUtils::ChannelType::RX);
    } else if (s == "RY") {
		return (kinematicUtils::ChannelType::RY);
    } else if (s == "RZ") {
		return (kinematicUtils::ChannelType::RZ);
    } else if (s == "L") {
        return (DegreeOfFreedom::L);
    } else {
		throw std::runtime_error("Unsupported acclaim data channel");
    }
}

std::string DegreeOfFreedom::getChannelName( kinematicUtils::Channel channel, bool uppercase) {
    switch (channel) {
        case kinematicUtils::ChannelType::TX : return uppercase ? "TX" : "tx";
        case kinematicUtils::ChannelType::TY : return uppercase ? "TY" : "ty";
        case kinematicUtils::ChannelType::TZ : return uppercase ? "TZ" : "tz";
        case kinematicUtils::ChannelType::RX : return uppercase ? "RX" : "rx";
        case kinematicUtils::ChannelType::RY : return uppercase ? "RY" : "ry";
        case kinematicUtils::ChannelType::RZ : return uppercase ? "RZ" : "rz";
        case DegreeOfFreedom::L  : return uppercase ? "L"  : "l";
    }
	throw std::runtime_error("Unsupported acclaim data channel");
}
