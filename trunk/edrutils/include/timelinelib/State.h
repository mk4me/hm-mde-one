#ifndef HEADER_GUARD__TIMELINESTATE_H__
#define HEADER_GUARD__TIMELINESTATE_H__

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Klasa obslugujaca stan modelu. Zawiera podstawowe informacje o nim.
class State
{
public:

    //! \param bool czy model jest odtwarzany
    bool isPlaying;

    //! \param double sumaryczna dlugosc timeline, od najwczesniejszego strumienia do konca ostatniego
    double length;

    //! \param double aktualny czas timeline
    double time;

    //! \param double znormalizowany czas timeline
    double normalizedTime;

public:
    //! konstruktor zerujacy
    State(void);
    virtual ~State(void);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINESTATE_H__