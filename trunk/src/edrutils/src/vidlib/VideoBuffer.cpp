#include "PCH.h"
#include <iostream>
#include "VidLibPrivate.h"
#include "VideoBufferChunk.h"
#include <vidlib/VideoBuffer.h>
#include <float.h>
#include <stack>

// TODO: pixelFormat->bogatyEnum, buforowanie


template <class Iter, class Val, class Pred>
Iter lowerBound( Iter first, Iter last, const Val& val, Pred pred )
{
  VIDLIB_FUNCTION_PROLOG;
  typedef typename std::iterator_traits<Iter>::difference_type Diff;
  Diff count = std::distance(first, last);
  while ( count > 0 ) {
    // wybieramy œrodkowy element
    Diff count2 = (count>>1);
    Iter mid = first;
    std::advance(mid, count2);
    // równowa¿ne *mid < val
    if ( pred(*mid, val) ) {
      first = ++mid;
      count -= count2 + 1;
    } else {
      count = count2;
    }
  }
  return first;
}

template <class Iter, class Val, class Pred>
Iter upperBound( Iter first, Iter last, const Val& val, Pred pred )
{
  VIDLIB_FUNCTION_PROLOG;
  typedef typename std::iterator_traits<Iter>::difference_type Diff;
  Diff count = std::distance(first, last);
  while ( count > 0 ) {
    // wybieramy œrodkowy element
    Diff count2 = (count>>1);
    Iter mid = first;
    std::advance(mid, count2);
    // równowa¿ne *mid < val
    if ( !pred(val, *mid) ) {
      first = ++mid;
      count -= count2 + 1;
    } else {
      count = count2;
    }
  }
  return first;
}



////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

struct PredStartTimeLE
{
    bool operator()(VideoBufferChunk* chunk, double time)
    {
        return chunk->getStartTime() <= time;
    }
};

struct PredStartTimeL
{
    bool operator()(VideoBufferChunk* chunk, double time)
    {
        return chunk->getStartTime() < time;
    }
    bool operator()(double time, VideoBufferChunk* chunk)
    {
        return time < chunk->getStartTime();
    }
};

//------------------------------------------------------------------------------

//! Bufor ramek o maksymalnym zadanym rozmiarze.
class VideoBufferImpl
{
private:
    //! Typ listy chunków.
    typedef std::list<VideoBufferChunk*> Chunks;

private:
    //! Chunki bufora.
    Chunks chunks;
    //! Wszystkie ramki.
    std::vector<Picture*> buffer;
    //! Nieu¿ywne, a zaalokowane ramki
    std::stack<Picture*> unused;
    //! Maksymalny rozmiar buforu.
    const int maxSize;
    //! Szerokoœæ ramki.
    int width;
    //! Wysokoœæ ramki.
    int height;
    //! Ile ramek maksymalnie mo¿na zbuforowaæ?
    size_t maxBufferSize;
    //! Format ramki
    PixelFormat format;

public:
    //! 
    //! \param maxSize
    //! \param width
    //! \param height
    //! \param format
    VideoBufferImpl(int maxSize, int width, int height, PixelFormat format);
    //! Destruktor, zwalnia ca³y bufor.
    ~VideoBufferImpl();

    //! \return
    inline PixelFormat getFormat() const
    { 
        return format;
    }

    //! \return WskaŸnik na bufor ramki albo NULL, je¿eli wyczerpa³o siê miejsce w buforze.
    Picture * pop();
    //! \param frame Ramka, która ma byæ zwrócona do bufora. Wywo³ywaæ TYLKO, jeœli po u¿yciu
    //!              metody pop ramka nie zosta³a u¿yta.
    void push(Picture * frame);

    //! Zaznacza, ¿e ramka jest u¿yta.
    //! \param frame
    //! \param startTime Pocz¹tek przedzia³u czasu.
    //! \param endTime Koniec przedzia³u czasu.
    bool notifyUsed(Picture * frame, double startTime, double endTime);
    //! Zwalania ramkê w zadanym punkcie czasowym.
    bool freeFrame(double time);
    //!
    bool freeFirstFrame();
    //!
    bool freeLastFrame();

    //! \param timestamp
    //! \param finish
    const Picture * getLastFrame(double * timestamp = NULL, double * finish = NULL);
    //! \param timestamp
    //! \param finish
    const Picture * getFirstFrame(double * timestamp = NULL, double * finish = NULL);

    //! \param time
    //! \param timestamp
    //! \param finish
    //! \return Zbuforowana ramka dla zdanego czasu b¹dŸ NULL, je¿eli nie jest zbuforowana.
    const Picture * getFrame(double time, double * timestamp = NULL, double * finish = NULL);
    //! \param time
    //! \param timestamp
    //! \param finish
    //! \return Zbuforowana ramka dla zdanego czasu b¹dŸ NULL, je¿eli nie jest zbuforowana.
    const Picture * getNearestFrame(double time, double * timestamp = NULL, double * finish = NULL);


private:

    Chunks::iterator getChunk( double time );
    Chunks::const_iterator getChunk( double time ) const;

    template <class Iter>
    Iter getChunk( double time, Iter begin, Iter end ) const;

};

//------------------------------------------------------------------------------

VideoBufferImpl::VideoBufferImpl(int maxSize, int width, int height, PixelFormat format)
  : maxSize(maxSize), width(width), height(height), format(format)
{
  VIDLIB_FUNCTION_PROLOG;
  maxBufferSize = maxSize / Picture::getAllocSize(width, height, format);
}

VideoBufferImpl::~VideoBufferImpl()
{
  VIDLIB_FUNCTION_PROLOG;
  for ( std::vector<Picture*>::iterator it = buffer.begin(); it != buffer.end(); ++it ) {
    (*it)->free();
    delete *it;
  }
}

Picture * VideoBufferImpl::pop()
{
  VIDLIB_FUNCTION_PROLOG;
  if ( unused.empty() ) {
    // nie ma niewykorzystanych, trzeba wiêc zaalokowaæ nowe (jeœli jest miejsce)
    if ( buffer.size() == maxBufferSize ) {
      // koniec miejsca
      return NULL;
    } else {
      // miejsce jest, mo¿na alokowaæ
      Picture * picture = new Picture(Picture::create(width, height, format));
      buffer.push_back(picture);
      return picture;
    }
  } else {
    // zdejmujemy ze stosu nieu¿ywanych
    Picture * result = unused.top();
    unused.pop();
    return result;
  }
}

void VideoBufferImpl::push( Picture * frame )
{
  VIDLIB_FUNCTION_PROLOG;
  // dodajemy do stosu nieu¿ywanych
  unused.push(frame);
}

bool VideoBufferImpl::freeFrame( double time )
{
  VIDLIB_FUNCTION_PROLOG;
  Chunks::iterator begin = chunks.begin(), end = chunks.end();
  Chunks::iterator it = getChunk(time, begin, end);
  if ( it != end ) {
    VideoBufferChunk * chunk = *it;
    // ok, jak usuwamy?
    VideoBufferChunk::iterator frame = chunk->findFrame(time);
    unused.push(frame->second.second);
    if ( frame == chunk->begin() ) {
      if ( frame == --chunk->end() ) {
        // chunk ma tylko tê ramkê, usuwamy go
        delete *it;
        chunks.erase(it);
      } else {
        // usuwamy z pocz¹tku
        chunk->popFront();
      }
    } else if ( frame == --chunk->end() ) {
      // usuwamy z ty³u
      chunk->popBack();
    } else {
      // kopiujemy czeœæ do nowego bufora
      VideoBufferChunk * newChunk = new VideoBufferChunk();
      VideoBufferChunk::const_iterator startCopy = frame;
      newChunk->append<VideoBufferChunk::const_iterator>(++startCopy, chunk->end());
      // usuwamy zbêdne z bufora
      chunk->erase(frame, chunk->end());
      // dodajemy do listy chunków
      chunks.insert(++it, newChunk);
    }
    return true;
  } else {
    // nie ma chunka który by to obs³u¿y³
    return false;
  }
}


bool VideoBufferImpl::notifyUsed( Picture * frame, double startTime, double endTime )
{
  VIDLIB_FUNCTION_PROLOG;
#if _DEBUG
  // sprawdzamy, czy ramki na pewno nie ma na liœcie u¿ytych i nieu¿ytych
  //assert( std::find(unused.begin(), unused.end(), frame) == unused.end() );
  UTILS_ASSERT( getFrame(startTime) == NULL );
#endif
  UTILS_ASSERT( startTime < endTime );
  UTILS_ASSERT( startTime >= 0.0 );

  // wyszukujemy bufor, który siê nadaje
  PredStartTimeL pred;
  Chunks::iterator begin = chunks.begin(), end = chunks.end();
  Chunks::iterator found = upperBound( begin, end, startTime, pred );
  Chunks::iterator next = found;
  Chunks::iterator prev = next==begin ? end : --Chunks::iterator(next);

  // sprawdzenie poprawnoœci bufora, tj fragmenty nie mog¹ na siebie nachodziæ
  if ( next != end ) {
    double chunkStart = (*next)->getStartTime();
    if ( chunkStart < endTime ) {
      return false;
    } else if ( chunkStart != endTime ) {
      next = end;
    }
  }
  if ( prev != end ) {
    double chunkEnd = (*prev)->getEndTime();
    if ( chunkEnd > startTime ) {
      return false;
    } else if ( chunkEnd != startTime ) {
      prev = end;
    }
  }

  if ( next != end ) {
    if ( prev != end ) {
      // dopisaæ i po³¹czyæ oba bufory
      (*prev)->append(frame, startTime, endTime);
      (*prev)->append((*next)->begin(), (*next)->end());
      delete *next;
      chunks.erase(next);
    } else {
      // dopisaæ na pocz¹tek
      (*next)->append(frame, startTime, endTime);
    }
  } else if ( prev != end ) {
    // dopisaæ na koniec
    (*prev)->append(frame, startTime, endTime);
  } else {
    // trzeba stworzyæ nowy bufor
    VideoBufferChunk * newChunk = new VideoBufferChunk();
    newChunk->append(frame, startTime, endTime);
    // wstawiamy
    chunks.insert( found, newChunk );
  }
  return true;
}

bool VideoBufferImpl::freeFirstFrame()
{
  VIDLIB_FUNCTION_PROLOG;
  if ( !chunks.empty() ) {
    // usuwamy pierwszy element
    VideoBufferChunk * chunk = *chunks.begin();
    Picture * picture = chunk->popFront();
    // czy usuwamy chunk?
    if ( chunk->empty() ) {
      delete chunk;
      chunks.pop_front();
    }
    // dodajemy do nieu¿ywanych
    unused.push(picture);
    return true;
  } else {
    return false;
  }
}

bool VideoBufferImpl::freeLastFrame()
{
  VIDLIB_FUNCTION_PROLOG;
  if ( !chunks.empty() ) {
    // usuwamy pierwszy element
    VideoBufferChunk * chunk = *--chunks.end();
    Picture * picture = chunk->popBack();
    // czy usuwamy chunk?
    if ( chunk->empty() ) {
      delete chunk;
      chunks.pop_back();
    }
    // dodajemy do nieu¿ywanych
    unused.push(picture);
    return true;
  } else {
    return false;
  }
}

const Picture * VideoBufferImpl::getFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
  VIDLIB_FUNCTION_PROLOG;
  Chunks::iterator it = getChunk(time, chunks.begin(), chunks.end());
  if ( it != chunks.end() ) {
    return (*it)->getFrame(time, timestamp, finish);
  } else {
    return NULL;
  }
}

const Picture * VideoBufferImpl::getNearestFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
  VIDLIB_FUNCTION_PROLOG;
  // jest co szukaæ?
  if ( chunks.empty() ) {
    return NULL;
  }

  // wyszukanie pierwszego chunka za zadanym czasem
  PredStartTimeLE pred;
  Chunks::iterator begin = chunks.begin();
  Chunks::iterator end = chunks.end();
  Chunks::iterator next = lowerBound(begin, end, time, pred);
  Chunks::iterator prev = next;

  // odleg³oœci od obu chunków
  double deltaRight = (next == end ? DBL_MAX : (*next)->getStartTime() - time);
  double deltaLeft = (prev == begin ? DBL_MAX : time - (*--prev)->getEndTime());

  if ( deltaLeft < 0.0 ) {
    return (*prev)->getFrame(time, timestamp, finish);
  } else if ( deltaRight < deltaLeft ) {
    return (*next)->getFirstFrame(timestamp, finish);
  } else {
    return (*prev)->getLastFrame(timestamp, finish);
  }
}


template <class Iter>
Iter VideoBufferImpl::getChunk( double time, Iter begin, Iter end ) const
{
  VIDLIB_FUNCTION_PROLOG;
  PredStartTimeL pred;
  Iter found = upperBound(begin, end, time, pred);
  if ( found != begin && (*--found)->getEndTime() > time ) {
    return found;
  } else {
    return end;
  }
}

const Picture * VideoBufferImpl::getLastFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    VIDLIB_FUNCTION_PROLOG;
    return chunks.empty() ? NULL : (*--chunks.end())->getLastFrame(timestamp, finish);
}

const Picture * VideoBufferImpl::getFirstFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    VIDLIB_FUNCTION_PROLOG;
    return chunks.empty() ? NULL : (*chunks.begin())->getFirstFrame(timestamp, finish);
}





// void VideoBufferImpl::trimBuffer( double begin, double end )
// {
//   // wyszukujemy granicy przedzia³u
//   UsedMap::iterator first = used.lower_bound(begin);
//   // dodajemy do listy nieu¿ywanych
//   for (UsedMap::iterator it = used.begin(); it != first; ++it) {
//     unused.push_back(it->second.second);
//   }
//   // usuwamy z mapy
//   used.erase(used.begin(), first);
//
//   // powtarzamy...
//   UsedMap::iterator last = used.upper_bound(end);
//   for (UsedMap::iterator it = last; it != used.end(); ++it) {
//     unused.push_back(it->second.second);
//   }
//   used.erase(last, used.end());
// }
//
// void VideoBufferImpl::freeOldest()
// {
//   if ( !used.empty() ) {
//     unused.push_back(used.begin()->second.second);
//     used.erase(used.begin());
//   }
// }
//
// void VideoBufferImpl::freeLatest()
// {
//   if ( !used.empty() ) {
//     UsedMap::iterator it = --used.end();
//     unused.push_back(it->second.second);
//     used.erase(it);
//   }
// }

VideoBuffer::VideoBuffer( int maxSize, int width, int height, PixelFormat format )
: impl(new VideoBufferImpl(maxSize, width, height, format))
{

}

VideoBuffer::~VideoBuffer()
{
    utils::deletePtr(impl);
}

PixelFormat VideoBuffer::getFormat() const
{
    return impl->getFormat();
}

Picture * VideoBuffer::pop()
{
    return impl->pop();
}

void VideoBuffer::push( Picture * frame )
{
    impl->push(frame);
}

bool VideoBuffer::notifyUsed( Picture * frame, double startTime, double endTime )
{
    return impl->notifyUsed(frame, startTime, endTime);
}

bool VideoBuffer::freeFrame( double time )
{
    return impl->freeFrame(time);
}

bool VideoBuffer::freeFirstFrame()
{
    return impl->freeFirstFrame();
}

bool VideoBuffer::freeLastFrame()
{
    return impl->freeLastFrame();
}

const Picture * VideoBuffer::getLastFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getLastFrame(timestamp, finish);
}

const Picture * VideoBuffer::getFirstFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getFirstFrame(timestamp, finish);
}

const Picture * VideoBuffer::getFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getFrame(time, timestamp, finish);
}

const Picture * VideoBuffer::getNearestFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getNearestFrame(time, timestamp, finish);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
