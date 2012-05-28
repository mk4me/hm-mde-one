#include "PCH.h"
#include "VisualizationScheme.h"
#include "uniqueCollection.h"
#include "ISchemeDrawer.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

kinematic::VskParserPtr Vsk::Count16(new kinematic::VskParser());
kinematic::VskParserPtr Vsk::Count39(new kinematic::VskParser());
kinematic::VskParserPtr Vsk::Count53(new kinematic::VskParser());

VisualizationScheme::VisualizationScheme() :
    normalizedTime(0)
{
}

void VisualizationScheme::setNormalizedTime( double val )
{
    UTILS_ASSERT(hasData());
    if (normalizedTime != val) {
        normalizedTime = val;
        if (val < 0.0 || val > 1.0) {
            LOGGER(Logger::Debug, "SkeletalVisualizationScheme : value out of <0,1>");
        }
        update(val);
    }
}

void VisualizationScheme::setGlobalStatesColor( const osg::Vec4& color )
{
    for (auto it = states.begin(); it != states.end(); ++it) {
        it->color = color;
    }
}


kinematic::VskParserConstPtr Vsk::get( MarkersCount count ) 
{
    switch(count) 
    {
    case MarkersCount16:										
        if (!Count16->isLoaded()) {	
            Count16->parse(core::getResourceString("trial/CZD.vsk"));		
        } 																
        return Count16;
    case MarkersCount39:										
        if (!Count39->isLoaded()) {	
            Count39->parse(core::getResourceString("trial/M39.vsk"));		
        } 																
        return Count39;

    case MarkersCount53:										
        if (!Count53->isLoaded()) {											
            Count53->parse(core::getResourceString("trial/M53.vsk"));		
        } 																
        return Count53;

    default:
        throw std::runtime_error("Wrong VSK scheme was requested");
    }
}
