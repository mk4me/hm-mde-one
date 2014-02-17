#include "PCH.h"
#include "VisualizationScheme.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

kinematic::VskParserPtr Vsk::Count16(new kinematic::VskParser());
kinematic::VskParserPtr Vsk::Count39(new kinematic::VskParser());
kinematic::VskParserPtr Vsk::Count53(new kinematic::VskParser());

kinematic::VskParserConstPtr Vsk::get( MarkersCount count )
{
    switch(count)
    {
    case MarkersCount15:
    case MarkersCount16:
        if (!Count16->isLoaded()) {
            Count16->parse(plugin::getResourcePath("trial/CZD.vsk").string());
        }
        return Count16;
    case MarkersCount39:
        if (!Count39->isLoaded()) {
            Count39->parse(plugin::getResourcePath("trial/M39.vsk").string());
        }
        return Count39;

    case MarkersCount53:
        if (!Count53->isLoaded()) {
            Count53->parse(plugin::getResourcePath("trial/M53.vsk").string());
        }
        return Count53;

    default:
        throw std::runtime_error("Wrong VSK scheme was requested");
    }
}
