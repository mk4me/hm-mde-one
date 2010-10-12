/********************************************************************
	created:  2010/05/13
	created:  13:5:2010   16:56
	filename: Stream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINEMODEL_H__
#define __HEADER_GUARD__TIMELINEMODEL_H__

#include <boost/weak_ptr.hpp>
#include <vector>
#include <stdexcept>
#include <utils/ObserverPattern.h>
#include <utils/Utils.h>
#include <OpenThreads/Thread>

#include "Stream.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

 
/**
 * Model dla kontrolki timeline.
 */
class Model : public utils::Observable<Model>
{
public:
    //! Wska�nik na strumie�.
    typedef boost::shared_ptr<Stream> StreamPtr;
    //! Zbi�r strumieni.
    typedef std::vector< StreamPtr > Streams;
    //! 
    typedef Streams::iterator iterator;
    //!
    typedef Streams::const_iterator const_iterator;
    //!
    typedef Streams::size_type size_type;

    //! Stan modelu.
    struct State 
    {
        //! Czas.
        double time;
        //! D�ugo�� strumienia. 
        double length;
        //! Znormalizowany czas.
        double normalizedTime;
        //! Czy jest odtwarzany?
        bool isPlaying;
        //! Jaka jest skala czasowa?
        double timeScale;
        //! Jak cz�sto ma by� od�wie�any?
        double refreshPeriod;
    };

private:
    //! Lista strumieni.
    Streams streams;
    //! Stan zaakceptowany.
    State state;

public:
    //! Konstruktor zeruj�cy.
    Model();
    //!
    virtual ~Model();

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
    void removeStream( StreamPtr stream );

    //! Ustawia wsp�lny czas wszystkich aktywnych strumieni.
    void setTime(double time);

    //! \return D�ugo�� najd�u�szego ze strumieni z uwzgl�dnieniem offset�w.
    double getLength() const;

    //! \return Czas strumieni.
    inline double getTime() const
    {
        return state.time;
    }
    //! \return
    inline bool isPlaying() const
    { 
        return state.isPlaying;
    }
    //! \param isPlaying
    inline void setPlaying(bool isPlaying) 
    { 
        state.isPlaying = isPlaying; 
        notify();
    }
    //! \return
    inline double getTimeScale() const
    { 
        return state.timeScale;
    }
    //! \param timeScale
    inline void setTimeScale(double timeScale) 
    { 
        state.timeScale = timeScale; 
        notify();
    }
    //! \return
    inline double getRefreshPeriod() const
    { 
        return state.refreshPeriod;
    }
    //! \param refreshPeriod
    inline void setRefreshPeriod(double refreshPeriod) 
    { 
        state.refreshPeriod = refreshPeriod; 
    }

    //! \return
    const State& getState() const
    { 
        return state;
    }
    //! \param state
    void setState(const State& state);

    //virtual void notify();

private:

    

    //! Pomocnicza metoda dodaj�ca strumie�.
    //! \param stream Strumie� do dodania.
    void addStreamInternal(StreamPtr stream);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__TIMELINEMODEL_H__