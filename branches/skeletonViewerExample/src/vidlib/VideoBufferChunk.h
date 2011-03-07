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

//! Ci�g�y fragment bufora.
class VideoBufferChunk
{
public:
  // TODO: zamieni� na deque
  //! Mapa u�ycia bufora.
  typedef std::map<double, std::pair<double, Picture*> > UsedMap;
  //! Iterator.
  typedef UsedMap::iterator iterator;
  //! Sta�y iterator.
  typedef UsedMap::const_iterator const_iterator;

private:
  //! Czas rozpocz�cia.
  double start;
  //! Czas zako�czenia.
  double stop;
  //! Mapa u�ycia bufora.
  UsedMap framesMap;

public:

  //! Konstruktor zeruj�cy.
  VideoBufferChunk();

  //! \return Pocz�tek obejmowanego przedzia�u (domkni�ty).
  inline double getStartTime() const
  {
    return start;
  }
  //! \return Koniec obejmowanego przedzia�u (otwarty)
  inline double getEndTime() const
  {
    return stop;
  }

  //! \return Pocz�tek.
  inline iterator begin()
  {
    return framesMap.begin();
  }
  //! \return Koniec.
  inline iterator end()
  {
    return framesMap.end();
  }
  //! \return Pocz�tek.
  inline const_iterator begin() const
  {
    return framesMap.begin();
  }
  //! \return Koniec.
  inline const_iterator end() const
  {
    return framesMap.end();
  }
  //! Czy co� jest?
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
  //! \return Czy mo�na uzupe�ni� bufor?
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

  //! \return Usuni�ty element z ko�ca bufora.
  Picture* popBack();

  //!
  void erase(iterator from, iterator to);

  //! \return Usuni�ty element z pocz�tku bufora.
  Picture* popFront();

  //! \param time
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu b�d� NULL, je�eli nie jest zbuforowana.
  inline const Picture * getFrame(double time, double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( findFrame(time), startTime, endTime );
  }
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu b�d� NULL, je�eli nie jest zbuforowana.
  inline const Picture * getFirstFrame(double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( framesMap.begin(), startTime, endTime );
  }
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu b�d� NULL, je�eli nie jest zbuforowana.
  inline const Picture * getLastFrame(double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( framesMap.empty() ? framesMap.end() : --framesMap.end(), startTime, endTime );
  }

  //!
  //! \param time
  //! \return Zbuforowana ramka dla zdanego czasu b�d� end(), je�eli nie jest zbuforowana.
  const_iterator findFrame(double time) const;
    //!
  //! \param time
  //! \return Zbuforowana ramka dla zdanego czasu b�d� end(), je�eli nie jest zbuforowana.
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
