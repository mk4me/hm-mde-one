#ifndef HEADER_GUARD__TIMELINETAG_H__
#define HEADER_GUARD__TIMELINETAG_H__

#include <timeline/Types.h>
#include <timeline/TagSelection.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class Model;

//! Klasa zaznaczajaca na osi czasu pewne zdarzenia/sytuacje dla strumieni.
class Tag
{

    friend class Model;
    friend class TagSelection;

public:

    typedef ConstSelections::const_iterator const_iterator;
    typedef ConstSelections::size_type size_type;

private:

    typedef Selections::iterator iterator;

private:

    //! nazwa tagu
    std::string name;

    //! pozycja w czasie tagu wzgledem jego strumienia
    double time;

    //! strumien ktorego dotyczy tag i jego czas wystapienia
    ChannelWPtr channel;

    //! strumien ktorego dotyczy tag i jego czas wystapienia
    ChannelConstWPtr constChannel;

    //! zaznaczenia w kotrych wystepue ten tag
    Selections selections;

    //! zaznaczenia w kotrych wystepue ten tag
    ConstSelections constSelections;

public:

    //! \return Nazwa tagu
    const std::string & getName() const;

    //! \return Channel ktory jest tagowany
    const ChannelConstWPtr & getChannel() const;

    //! \return Czas wystapienia tagu
    double getTime() const;

    //! \param idx Indeks zaznaczenia
    //! \return Zaznaczenie
    const SelectionConstPtr & getSelection(size_type idx) const;

    //! \return Ilosc zaznaczen w ktorych bierze udzial dany tag
    size_type size() const;

    //! \return Pierwsze zaznaczenie w ktorym wystepuje tag
    const_iterator begin() const;

    //! \return Koniec zaznaczen
    const_iterator end() const;

protected:

    //! Konstruktor
    //! \param channel Kanal dla ktorego tworzy sie dany tag
    //! \param time Czas wystapienia taga w kanale
    //! \param name Nazwa taga
    Tag(const ChannelPtr & channel, double time = 0, const std::string & name = std::string("UnnamedTag"));

    //! \return Channel ktory jest tagowany
    const ChannelWPtr & getChannel();

    //! resetuje kanal podczass usuwania go
    void resetChannel();

    //! \return Pierwsze zaznaczenie w ktorym wystepuje tag
    iterator begin();

    //! \return Koniec zaznaczen
    iterator end();

    //! \param name Nazwa tagu
    void setName(const std::string & name);

    //! \param time Czas wytapienia tagu
    void setTime(double time);

    //! \param selection Zaznaczenie ktore bazuje na tym tagu
    void addSelection(const SelectionPtr & selection);

    //! \param selection Zaznaczenie ktore bazuje na tym tagu
    void removeSelection(const SelectionPtr & selection);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETAG_H__