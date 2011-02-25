#include "stdafx.h"
#include <KinematicModelLibrary/SkeletalParsers.h>
#include <KinematicModelLibrary/hmException.h>
#include <KinematicModelLibrary/DegreeOfFreedom.h>

using namespace hmAnimation;


std::string Axis::getAxisOrderName( Axis::Order order ) {
    switch(order){
        case XYZ : return "XYZ";
        case XZY : return "XZY";
        case YXZ : return "YXZ";
        case YZX : return "YZX";
        case ZXY : return "ZXY";
        case ZYX : return "ZYX";
        default:
            throw AcclaimWrongFileException("Unknown axis order");
    }
}

int hmAnimation::DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::Channel channel, const std::vector<DegreeOfFreedom>& dofs ) {
    for (int i = dofs.size() - 1; i >= 0; --i) {
        if (dofs[i].channel == channel) {
            return i;
        }
    }
    return -1;
}

hmAnimation::Axis::Order hmAnimation::Axis::getAxisOrder( const std::string& axis ) {
    std::string s(axis);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "XYZ") {
        return Axis::XYZ;
    } else if (s == "XZY") {
        return Axis::XZY;
    } else if (s == "YXZ") {
        return Axis::XZY;
    } else if (s == "YZX") {
        return Axis::YZX;
    } else if (s == "ZXY") {
        return Axis::ZXY;
    } else if (s == "ZYX") {
        return Axis::ZYX;
    } else {
        return Axis::UnknownAxisOrder;
    }
}

DegreeOfFreedom::Channel DegreeOfFreedom::getChannel( const std::string& channel ) {
    std::string s(channel);
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "TX" || s == "XPOSITION") {
        return (DegreeOfFreedom::TX);
    } else if (s == "TY" || s == "YPOSITION") {
        return (DegreeOfFreedom::TY);
    } else if (s == "TZ" || s == "ZPOSITION") {
        return (DegreeOfFreedom::TZ);
    } else if (s == "RX" || s == "XROTATION") {
        return (DegreeOfFreedom::RX);
    } else if (s == "RY" || s == "YROTATION") {
        return (DegreeOfFreedom::RY);
    } else if (s == "RZ" || s == "ZROTATION") {
        return (DegreeOfFreedom::RZ);
    } else if (s == "L") {
        return (DegreeOfFreedom::L);
    } else {
        return DegreeOfFreedom::Unknown;
    }
}


std::string hmAnimation::DegreeOfFreedom::getChannelName( DegreeOfFreedom::Channel channel, bool uppercase) {
    switch (channel) {
        case DegreeOfFreedom::TX : return uppercase ? "TX" : "tx";
        case DegreeOfFreedom::TY : return uppercase ? "TY" : "ty";
        case DegreeOfFreedom::TZ : return uppercase ? "TZ" : "tz";
        case DegreeOfFreedom::RX : return uppercase ? "RX" : "rx";
        case DegreeOfFreedom::RY : return uppercase ? "RY" : "ry";
        case DegreeOfFreedom::RZ : return uppercase ? "RZ" : "rz";
        case DegreeOfFreedom::L  : return uppercase ? "L"  : "l";
    }
    Logger::getInstance().log(Logger::Error, "Unknown Degree Of Freedom");
    assert(false);
    //TODO : warning;
    return "unknown";
}
