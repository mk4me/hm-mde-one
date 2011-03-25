#ifndef HEADER_GUARD__TIMELINEISELECTION_H__
#define HEADER_GUARD__TIMELINEISELECTION_H__

#include <timelinelib/Types.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class Model;

/** Klasa bazowa dla tworzenia zaznaczen fragmentow strumieni.
* Oferuje tylko podstawowe API, implementacja powinna pojawic sie w konkretnych
* klasa pochodnych.
*/
class SelectionBase
{

    friend class Model;

protected:

    //! nazwa zaznaczenia
    std::string name;

    //! strumien ktorego dotyczy zaznaczenie
    ChannelWPtr channel;

    //! strumien ktorego dotyczy zaznaczenie
    ChannelConstWPtr constChannel;

public:

    virtual ~SelectionBase();

    //! \return Nazwa tagu
    const std::string & getName() const;

    //! \return Kanal ktory jest tagowany
    const ChannelConstWPtr & getChannel() const;

    //! \return Poczatek zaznaczenia - czas
    virtual double getBegin() const = 0;

    //! \return Koniec zaznaczenia - czas
    virtual double getEnd() const = 0;

    //! \return Czas trwania zaznaczenia
    double getLength() const;

    //! \param nowe nazwa tagu
    void setName(const std::string & name);

    //! \param poczatek zaznaczenia - czas
    virtual void setBegin(double begin) = 0;

    //! \param koniec zaznaczenia - czas
    virtual void setEnd(double end) = 0;

    //! \param przesuniecie zaznaczenia
    virtual void shiftSelection(double delta);

    //! \return Kanal ktory jest zaznaczony
    const ChannelWPtr & getChannel();

protected:

    //! konstruktor
    //! \param channel Kanal dla ktorego tworzy sie zaznaczenie
    //! \param name Nazwa zaznaczenia
    SelectionBase(const ChannelPtr & channel, const std::string & name = std::string("UnnamedSelection"));

    //! Resetuje kanal ktory jest zaznaczany gdy usuwamy selekcje
    void resetChannel();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINEISELECTION_H__