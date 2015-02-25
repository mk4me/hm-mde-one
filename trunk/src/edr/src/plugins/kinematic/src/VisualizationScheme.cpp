#include "PCH.h"
#include "VisualizationScheme.h"
#include "viconformatslib/VskParser.h"

using namespace std;
using namespace osg;
using namespace boost;

vicon::VskPtr Vsk::Count16(nullptr);
vicon::VskPtr Vsk::Count39(nullptr);
vicon::VskPtr Vsk::Count53(nullptr);

vicon::VskConstPtr Vsk::get( MarkersCount count )
{
    switch(count)
    {
    case MarkersCount15:
    case MarkersCount16:
        if (!Count16) {
			Count16 = utils::make_shared<vicon::Vsk>();
			vicon::VskParser::parse(plugin::getResourcePath("trial/CZD.vsk").string(), *Count16);
        }
        return Count16;
    case MarkersCount39:
        if (!Count39) {
			Count39 = utils::make_shared<vicon::Vsk>();
			vicon::VskParser::parse(plugin::getResourcePath("trial/M39.vsk").string(), *Count39);
        }
        return Count39;

    case MarkersCount53:
        if (!Count53) {
			Count53 = utils::make_shared<vicon::Vsk>();
			vicon::VskParser::parse(plugin::getResourcePath("trial/M53.vsk").string(), *Count53);
        }
        return Count53;

    default:
        throw core::runtime_error("Wrong VSK scheme was requested");
    }
}
