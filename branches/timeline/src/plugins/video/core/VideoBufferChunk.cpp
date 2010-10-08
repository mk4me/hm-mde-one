#include "../VideoPCH.h"
#include "VMPrivate.h"
#include "VideoBufferChunk.h"

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

VideoBufferChunk::VideoBufferChunk()
  : start(INVALID_TIMESTAMP), stop(INVALID_TIMESTAMP)
{
  VM_FUNCTION_PROLOG;
}


VideoBufferChunk::const_iterator VideoBufferChunk::findFrame( double time ) const
{
  VM_FUNCTION_PROLOG;
  // zatrzymujemy si� na pierszym wpisie maj�cym czas wi�kszy od zadanego
  const_iterator found = framesMap.upper_bound(time);
  if ( found == framesMap.begin() ) {
    // nie ma �adnego, zbyt ma�y timestamp
    return framesMap.end();
  } else {
    // tak naprawd� potrzebujemy poprzedniej ramki
    const_iterator result = --found;
    // sprawdzamy, czy to na pewno dobra ramka
    if ( result->second.first > time ) {
      // mie�ci si� w czasie przed zako�czeniem znalezionej ramki
      return result;
    } else {
      // po prostu dostali�my ostatni element
      return framesMap.end();
    }
  }
}

VideoBufferChunk::iterator VideoBufferChunk::findFrame( double time )
{
  VM_FUNCTION_PROLOG;
  // zatrzymujemy si� na pierszym wpisie maj�cym czas wi�kszy od zadanego
  iterator found = framesMap.upper_bound(time);
  if ( found == framesMap.begin() ) {
    // nie ma �adnego, zbyt ma�y timestamp
    return framesMap.end();
  } else {
    // tak naprawd� potrzebujemy poprzedniej ramki
    iterator result = --found;
    // sprawdzamy, czy to na pewno dobra ramka
    if ( result->second.first > time ) {
      // mie�ci si� w czasie przed zako�czeniem znalezionej ramki
      return result;
    } else {
      // po prostu dostali�my ostatni element
      return framesMap.end();
    }
  }
}

const Picture * VideoBufferChunk::getHelper( const_iterator found, double * startTime /*= NULL*/, double * endTime /*= NULL */ ) const
{
  VM_FUNCTION_PROLOG;
  if ( found != framesMap.end() ) {
    // znaleziono!
    if ( startTime ) {
      *startTime = found->first;
    }
    if ( endTime ) {
      *endTime = found->second.first;
    }
    return found->second.second;
  } else {
    // nie znaleziono
    return NULL;
  }
}

Picture* VideoBufferChunk::popBack()
{
  VM_FUNCTION_PROLOG;
  Picture* result = NULL;
  if ( !framesMap.empty() ) {
    iterator it = --framesMap.end();
    stop = it->first;
    result = it->second.second;
    framesMap.erase(it);
  } else {
    start = stop = INVALID_TIMESTAMP;
  }
  return result;
}

void VideoBufferChunk::erase( iterator from, iterator to )
{
  VM_FUNCTION_PROLOG;
  if ( from != begin() && to != end() ) {
    throw std::runtime_error("Can remove only from bundaries.");
  }
  // usuwamy
  framesMap.erase(from, to);
  //
  if ( framesMap.empty() ) {
    start = stop = INVALID_TIMESTAMP;
  } else {
    start = framesMap.begin()->first;
    stop = framesMap.rbegin()->second.first;
  }
}

Picture* VideoBufferChunk::popFront()
{
  VM_FUNCTION_PROLOG;
  Picture* result = NULL;
  if ( !framesMap.empty() ) {
    iterator it = framesMap.begin();
    start = it->second.first;
    result = it->second.second;
    framesMap.erase(it);
  } else {
    start = stop = INVALID_TIMESTAMP;
  }
  return result;
}

void VideoBufferChunk::append( Picture * picture, double startTime, double endTime )
{
  VM_FUNCTION_PROLOG;
  UTILS_ASSERT(startTime < endTime);
  if ( !framesMap.empty() ) {
    // czy jest sens wrzuca�?
    if (startTime == getEndTime()) {
      stop = endTime;
    } else if ( endTime == getStartTime() ) {
      start = startTime;
    } else {
      throw new std::runtime_error("Could not append.");
    }
  } else {
    // pierwsza ramka, ustalamy ram
    start = startTime;
    stop = endTime;
  }
  // czy ju� by�o?
  if (!framesMap.insert(std::make_pair(startTime, std::make_pair(endTime, picture))).second) {
    throw new std::runtime_error("Frame already in buffer.");
  }
}


////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////

