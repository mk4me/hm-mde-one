#include "TimelinePCH.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <plugins/timeline/Model.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

Model::Model()
{
    state.time = 0.0;
    state.length = 0.0;
    state.normalizedTime = 0.0;
    state.isPlaying = false;
    state.timeScale = 1.0;
    state.refreshPeriod = 0.035;
}

Model::~Model()
{
}

void Model::setTime( double time )
{
    uiTime = time;
    state.time = time;
    for ( iterator it = begin(); it != end(); ++it ) {
        StreamPtr& stream = *it;
        double realTime = time - stream->getStartOffset();
        double clamped = utils::clamp(realTime, 0.0, stream->getLength());
        stream->setTime(clamped);
    }
    notify();
}


double Model::getLength() const
{
    double maxLength = 0.0;
    for ( const_iterator it = begin(); it != end(); ++it ) {
        maxLength = std::max<double>( (*it)->getLength() + (*it)->getStartOffset(), maxLength );
    }
    return maxLength;
}


void Model::addStreamInternal( StreamPtr stream )
{
    iterator it = std::find( begin(), end(), stream );
    if ( it != end() ) {
        throw new std::runtime_error("Stream already added.");
    } else {
        streams.push_back(stream);
        // synchronizacja czasu
        stream->setTime(state.time);
        notify();
    }
}


void Model::removeStream( StreamPtr stream )
{
    iterator it = std::remove( begin(), end(), stream );
    if ( it != end() ) {
        streams.resize( std::distance(begin(), it) );
        notify();
    } else {
        throw new std::runtime_error("Stream not added.");
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////
