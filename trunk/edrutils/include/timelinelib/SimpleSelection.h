#ifndef HEADER_GUARD__TIMELINESIMPLESELECTION_H__
#define HEADER_GUARD__TIMELINESIMPLESELECTION_H__

#include <timeline/SelectionBase.h>
#include <timeline/Types.h>
////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Klasa obslugujaca zaznaczenie pomiedzy dwoma wybranymi punktami w czasie
class SimpleSelection : public SelectionBase
{

    friend class Model;

private:

    //! Poczatek czasu zaznaczenia
    double begin;

    //! Koniec czasu zaznaczenia
    double end;

public:

    ~SimpleSelection(void);

    //! \return Poczatek zaznaczenia - czas
    virtual double getBegin() const;

    //! \return Koniec zaznaczenia - czas
    virtual double getEnd() const;

    //! \param Poczatek zaznaczenia - czas
    virtual void setBegin(double begin);

    //! \param Koniec zaznaczenia - czas
    virtual void setEnd(double end);

protected:

    //! konstruktor
    //! \param channel Kanal dla ktorego tworzy sie dane zaznaczenie
    //! \paran name Nazwa zaznaczenia
    SimpleSelection(const ChannelPtr & channel, double begin, double end, const std::string & name = std::string("UnnamedSelection"));

};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINESIMPLESELECTION_H__