#ifndef HEADER_GUARD__TIMELINESTATE_H__
#define HEADER_GUARD__TIMELINESTATE_H__

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Klasa obslugujaca stan modelu. Zawiera podstawowe informacje o nim.
class State
{
public:

    //! czy model jest odtwarzany
    bool isPlaying;

    //! sumaryczna dlugosc timeline, od najwczesniejszego strumienia do konca ostatniego
    double length;

    //! aktualny czas timeline
    double time;

    //! znormalizowany czas timeline
    double normalizedTime;

    //! skala czasu calego timeline
    double timeScale;

public:
    //! konstruktor zerujacy
    State(void);
    virtual ~State(void);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINESTATE_H__