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
    for (auto it = states.begin(); it != states.end(); it++) {
        it->color = color;
    }
}

