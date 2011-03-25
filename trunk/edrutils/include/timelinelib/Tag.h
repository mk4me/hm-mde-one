#ifndef HEADER_GUARD__TIMELINETAG_H__
#define HEADER_GUARD__TIMELINETAG_H__

#include <utils/ObserverPattern.h>
#include <timeline/Types.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Klasa zaznaczajaca na osi czasu pewne zdarzenia/sytuacje dla strumieni.

class Tag : public utils::Observable<Tag>
{
private:

    //! nazwa tagu
    std::string name;

    //! pozycja w czasie tagu wzgledem jego strumienia
    double time;

    //! strumien ktorego dotyczy tag i jego czas wystapienia
    IStreamWPtr stream;

public:

    //! konstruktor
    //! \param StreamPtr strumien dla ktorego tworzy sie dany tag
    Tag(IStreamPtr stream, const std::string & name = std::string("UnnamedTag"));

    //! \return nazwa tagu
    const std::string & getName() const;

    //! \param string nowe nazwa tagu
    void setName(const std::string & name);

    //! \return zwraca strumien ktory jest tagowany
    const IStreamWPtr & getTagStream() const;

    //! \return czas wystapienia tagu
    double getTime() const;

    //! \param double nowy czas wytapienia tagu
    void setTime(double time);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETAG_H__