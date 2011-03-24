#include "CorePCH.h"
#include "ObjectSource.h"

using namespace core;

std::vector<int> sdsdsd;

auto get() -> decltype( boost::make_iterator_range(sdsdsd) )
{
    return boost::make_iterator_range(sdsdsd);
}

ObjectSource::ObjectSource( const SourcesTypes& types ) :
sourcesTypes(types), sources(types.size())
{
    boost::make_iterator_range(sourcesTypes);
}

ObjectSource::ObjectSource( IVisualizer* visualizer )
{
    UTILS_ASSERT(visualizer);
    // pobranie listy wspieranych typów
    SourcesTypes::value_type info;
    for (int i = 0; i < IVisualizer::maxNumSources; ++i) {
        info.first.clear();
        info.second.clear();
        visualizer->getSlotInfo(i, info.first, info.second);
        if ( info.second.empty() ) {
            break;
        } else {
            // zamiast push_backa mo¿na zrobiæ bardziej optymalnie i nie kopiowaæ wektora...
            // sourcesTypes.push_back(info);
            sourcesTypes.insert(sourcesTypes.end(), SourcesTypes::value_type())->swap(info);
        }
    }
    sources.resize(sourcesTypes.size());
}

int ObjectSource::getNumObjects() const
{
    return static_cast<int>(sources.size());
}

void ObjectSource::setObject( int sourceNo, const core::ObjectWrapperPtr& object )
{
    UTILS_ASSERT(isAssignable(sourceNo, object));
    sources[sourceNo] = std::make_pair(object, core::ObjectWrapperConstPtr(object));
}

void ObjectSource::setObject( int sourceNo, const core::ObjectWrapperConstPtr& object )
{
    UTILS_ASSERT(isAssignable(sourceNo, object));
    sources[sourceNo] = std::make_pair(core::ObjectWrapperPtr(), object);
}

ObjectWrapperPtr ObjectSource::getObject( int idx, boost::false_type )
{
    Source& source = sources[idx];
    if ( !source.first ) {
        // hmmm nie ma zmiennego, trzeba wiêc sklonowaæ niezmienny obiekt
        throw std::runtime_error("Cloning not implemented yet.");
    }
    return source.first;
}

ObjectWrapperConstPtr ObjectSource::getObject( int idx, boost::true_type )
{
    Source& source = sources[idx];
    return source.second;
}

bool ObjectSource::isAssignable( int sourceNo, const core::ObjectWrapperPtr& object ) const
{
    return isAssignable( sourceNo, object.get() );
}

bool ObjectSource::isAssignable( int sourceNo, const core::ObjectWrapperConstPtr& object ) const
{
    return isAssignable( sourceNo, object.get() );
}

bool ObjectSource::isAssignable( int sourceNo, const core::ObjectWrapper* object ) const
{
    if ( !object ) {
        return true;
    }

    const ObjectWrapper::Types& types = sourcesTypes[sourceNo].second;
    auto found = std::find_if(types.begin(), types.end(), [&](const ObjectWrapper::Type& type) { 
        return object->isSupported(type); 
    });
    return found != types.end();
}

const ObjectWrapper::Types& ObjectSource::getTypes( int sourceNo ) const
{
    return sourcesTypes[sourceNo].second;
}

const std::string& ObjectSource::getName( int sourceNo ) const
{
    return sourcesTypes[sourceNo].first;
}