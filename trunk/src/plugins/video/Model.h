/********************************************************************
	created:  2010/05/13
	created:  13:5:2010   16:56
	filename: Stream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINEMODEL_H__
#define __HEADER_GUARD__TIMELINEMODEL_H__

#include <vector>
#include <stdexcept>

#include <utils/ObserverPattern.h>
#include <utils/Utils.h>

#include "Stream.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

 
/**
 * Model dla kontrolki timeline.
 */
class Model : public utils::Observable< Model >
{
public:
  //! Wska�nik na strumie�.
  typedef Stream* StreamPtr;
  //! Zbi�r strumieni.
  typedef std::vector< StreamPtr > Streams;
  //! 
  typedef Streams::iterator iterator;
  //!
  typedef Streams::const_iterator const_iterator;
  //!
  typedef Streams::size_type size_type;

private:
  //! Lista strumieni.
  Streams streams;
  //! Bie��cy czas.
  double time;
  //!
  bool isPlaying;
  //!
  double timeScale;
  //!
  double refreshPeriod;
  //! Czas kt�ry jest zakolejkowany, ale jeszcze nie ustawiony.
  double uiTime;


public:
  //! Konstruktor zeruj�cy.
  Model() : time(0.0), timeScale(1.0), refreshPeriod(0.035), isPlaying(false), uiTime(0.0) {}

  //!
  virtual ~Model()
  {
    for ( iterator it = begin(); it != end(); ++it ) {
      delete *it;
    }
  }

  //! Pierwszy strumie�.
  inline iterator begin()
  {
    return streams.begin();
  }
  //! Koniec.
  inline iterator end()
  {
    return streams.end();
  }
  //! \param idx Indeks strumienia.
  //! \return Strumie�.
  inline StreamPtr & getStream(size_type idx)
  {
    return streams[idx];
  }
  //! Pierwszy strumie�.
  inline const_iterator begin() const
  {
    return streams.begin();
  }
  //! Koniec.
  inline const_iterator end() const
  {
    return streams.end();
  }
  //! \return Liczba strumieni.
  inline size_type getStreamCount() const
  {
    return streams.size();
  }
  //! \param idx Indeks strumienia.
  //! \return Strumie�.
  inline const StreamPtr & getStream(size_type idx) const
  {
    return streams[idx];
  }

  //! Pomocnicza metoda dodaj�ca strumie� do modelu.
  //! \param encapsulable Strumie� do enkapsulacji.
  //! \see timeline::Stream::encapsulaetgo
  template <class T>
  inline void addStream( T * encapsulable )
  {
    addStreamInternal( Stream::encapsulate(encapsulable) );
  }

  //! Dodaje strumie� do listy.
  //! \param stream
  inline void addStream( StreamPtr stream )
  {
    addStreamInternal(stream);
  }

  //! Usuwa strumie� z listy.
  void removeStream( StreamPtr stream )
  {
    iterator it = std::remove( begin(), end(), stream );
    if ( it != end() ) {
      streams.resize( std::distance(begin(), it) );
      notify();
      delete *it;
    } else {
      throw new std::runtime_error("Stream not added.");
    } 
  }

  //! Ustawia wsp�lny czas wszystkich aktywnych strumieni.
  void setTime(double time)
  {
    this->uiTime = time;
    this->time = time;
    for ( iterator it = begin(); it != end(); ++it ) {
      Stream *& stream = *it;
      double realTime = time - stream->getStartOffset();
      double clamped = utils::clamp(realTime, 0.0, stream->getLength());
      stream->setTime(clamped);
    }
    notify();
  }

  //! \return D�ugo�� najd�u�szego ze strumieni z uwzgl�dnieniem offset�w.
  double getLength() const
  {
    double maxLength = 0.0;
    for ( const_iterator it = begin(); it != end(); ++it ) {
      maxLength = std::max<double>( (*it)->getLength() + (*it)->getStartOffset(), maxLength );
    }
    return maxLength;
  }

  //! \return Czas strumieni.
  inline double getTime() const
  {
    return time;
  }
  //! \return
  inline bool getPlaying() const
  { 
    return isPlaying;
  }
  //! \param isPlaying
  inline void setPlaying(bool isPlaying) 
  { 
    this->isPlaying = isPlaying; 
    notify();
  }
  //! \return Znormalizowany czas.
  inline double getNormalizedTime() const
  {
    return getTime() / getLength();
  }
  //! \param time Znormalizowany czas.
  inline void setNormalizedTime(double time)
  {
    setTime( time * getLength() );
  }
  //! \return
  inline double getTimeScale() const
  { 
    return timeScale;
  }
  //! \param timeScale
  inline void setTimeScale(double timeScale) 
  { 
    this->timeScale = timeScale; 
    notify();
  }
  //! \return
  inline double getRefreshPeriod() const
  { 
    return refreshPeriod;
  }
  //! \param refreshPeriod
  inline void setRefreshPeriod(double refreshPeriod) 
  { 
    this->refreshPeriod = refreshPeriod; 
  }

  //! Czas, kt�ry jest zakolejkowany, ale jeszcze nie ustawiony.
  //! \param time Liczba ujemna gdy nie ma czasu UI
  inline void setNormalizedUITime(double time)
  {
    setUITime(time * getLength());
  }
  //! Czas, kt�ry jest zakolejkowany, ale jeszcze nie ustawiony.
  //! \param time Liczba ujemna gdy nie ma czasu UI
  inline void setUITime(double time) 
  { 
    this->uiTime = time; 
    notify();
  }
  //! \return Czas kt�rego nale�y u�ywa� przy wy�wietlaniu UI.
  inline double getUITime() const
  {
    return uiTime;
  }
  //! \return Znormalizowany czas kt�rego nale�y u�ywa� przy wy�wietlaniu UI.
  inline double getNormalizedUITime() const
  {
    return getUITime() / getLength();
  }


private:

  //! Pomocnicza metoda dodaj�ca strumie�.
  //! \param stream Strumie� do dodania.
  void addStreamInternal(StreamPtr stream)
  {
    iterator it = std::find( begin(), end(), stream );
    if ( it != end() ) {
      throw new std::runtime_error("Stream already added.");
    } else {
      streams.push_back(stream);
      // synchronizacja czasu
      stream->setTime(time);
      notify();
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__TIMELINEMODEL_H__