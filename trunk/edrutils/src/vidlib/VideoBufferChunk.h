/********************************************************************
	created:  2010/08/04
	created:  4:8:2010   14:44
	filename: VideoBufferChunk.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__VIDEOBUFFERCHUNK_H__
#define __HEADER_GUARD_VIDLIB__VIDEOBUFFERCHUNK_H__

#include <map>
#include <vidlib/PixelFormat.h>
#include <vidlib/Picture.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! Ciπg≥y fragment bufora.
class VideoBufferChunk
{
public:
  // TODO: zamieniÊ na deque
  //! Mapa uøycia bufora.
  typedef std::map<double, std::pair<double, Picture*> > UsedMap;
  //! Iterator.
  typedef UsedMap::iterator iterator;
  //! Sta≥y iterator.
  typedef UsedMap::const_iterator const_iterator;

private:
  //! Czas rozpoczÍcia.
  double start;
  //! Czas zakoÒczenia.
  double stop;
  //! Mapa uøycia bufora.
  UsedMap framesMap;

public:

  //! Konstruktor zerujπcy.
  VideoBufferChunk();

  //! \return Poczπtek obejmowanego przedzia≥u (domkniÍty).
  inline double getStartTime() const
  {
    return start;
  }
  //! \return Koniec obejmowanego przedzia≥u (otwarty)
  inline double getEndTime() const
  {
    return stop;
  }

  //! \return Poczπtek.
  inline iterator begin()
  {
    return framesMap.begin();
  }
  //! \return Koniec.
  inline iterator end()
  {
    return framesMap.end();
  }
  //! \return Poczπtek.
  inline const_iterator begin() const
  {
    return framesMap.begin();
  }
  //! \return Koniec.
  inline const_iterator end() const
  {
    return framesMap.end();
  }
  //! Czy coú jest?
  inline bool empty() const
  {
    return framesMap.empty();
  }
  //! \param time Czas do sprawdzenia.
  //! \return Czy bufor zawiera dany czas?
  inline bool contains(double time)
  {
    return time >= getStartTime() && time < getEndTime();
  }
  //! \param startTime
  //! \param endTime
  //! \return Czy moøna uzupe≥niÊ bufor?
  inline bool canAppend(double startTime, double endTime) const
  {
    return startTime == getEndTime() || endTime == getStartTime();
  }
  //!
  //! \param first
  //! \param last
  //! \throws std::runtime_error
  template <class Iter>
  void append(Iter first, Iter last)
  {
    for ( Iter it = first; it != last; ++it ) {
      append( it->second.second, it->first, it->second.first );
    }
  }
  //!
  //! \param picture
  //! \param startTime
  //! \param endTime
  //! \throws std::runtime_error
  void append(Picture * picture, double startTime, double endTime);

  //! \return UsuniÍty element z koÒca bufora.
  Picture* popBack();

  //!
  void erase(iterator from, iterator to);

  //! \return UsuniÍty element z poczπtku bufora.
  Picture* popFront();

  //! \param time
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu bπdü NULL, jeøeli nie jest zbuforowana.
  inline const Picture * getFrame(double time, double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( findFrame(time), startTime, endTime );
  }
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu bπdü NULL, jeøeli nie jest zbuforowana.
  inline const Picture * getFirstFrame(double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( framesMap.begin(), startTime, endTime );
  }
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu bπdü NULL, jeøeli nie jest zbuforowana.
  inline const Picture * getLastFrame(double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( framesMap.empty() ? framesMap.end() : --framesMap.end(), startTime, endTime );
  }

  //!
  //! \param time
  //! \return Zbuforowana ramka dla zdanego czasu bπdü end(), jeøeli nie jest zbuforowana.
  const_iterator findFrame(double time) const;
    //!
  //! \param time
  //! \return Zbuforowana ramka dla zdanego czasu bπdü end(), jeøeli nie jest zbuforowana.
  iterator findFrame(double time);

private:

  //!
  //! \param found
  //! \param startTime
  //! \param endTime
  const Picture * getHelper( UsedMap::const_iterator found, double * startTime = NULL, double * endTime = NULL ) const;

};


////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD_VIDLIB__VIDEOBUFFERCHUNK_H__
