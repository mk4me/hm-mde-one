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
}

Model::~Model()
{
}

void Model::setTime( double time )
{
    state.time = time;
    for ( iterator it = begin(); it != end() && !timeDirty; ++it ) {
        StreamPtr& stream = *it;
        double realTime = time - stream->getStartOffset();
        double clamped = utils::clamp(realTime, 0.0, stream->getLength());
        stream->setTime(clamped);
    }
    notify();
}


void Model::setState( const State& state )
{
    this->state = state;
    setTime(state.time);
}


double Model::calculateLength() const
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
    state.length = calculateLength();
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
    state.length = calculateLength();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////
