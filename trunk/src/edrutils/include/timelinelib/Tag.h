﻿#ifndef HEADER_GUARD__TIMELINETAG_H__
#define HEADER_GUARD__TIMELINETAG_H__

#include <timelinelib/Types.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class Model;

//! Klasa zaznaczajaca na osi czasu pewne zdarzenia/sytuacje dla strumieni.
class Tag
{
    friend class Channel;
    friend class Model;

private:

    //! Nazwa tagu
    std::string name;

    //! Pozycja początku w czasie tagu wzgledem jego strumienia
    double begin;

    //! Czas trwania taga - dlugosc
    double length;

    //! Strumien ktorego dotyczy tag i jego czas wystapienia
    ChannelWPtr channel;

    //! Strumien ktorego dotyczy tag i jego czas wystapienia
    ChannelConstWPtr constChannel;

public:

    //! \return Nazwa tagu
    const std::string & getName() const;

    //! \return Channel ktory jest tagowany
    const ChannelConstWPtr & getChannel() const;

    //! \return Czas wystapienia tagu
    double getBeginTime() const;

    //! \return Czas trwania taga
    double getLength() const;

protected:

    //! Konstruktor
    //! \param channel Kanal dla ktorego tworzy sie dany tag
    //! \param time Czas wystapienia taga w kanale
    //! \param name Nazwa taga
    Tag(const ChannelPtr & channel, const std::string & name = std::string("UnnamedTag"), double begin = 0, double length = 0);

    //! \return Channel ktory jest tagowany
    const ChannelWPtr & getChannel();

    //! \param dTime Przesuniesie taga wzgledem jego obecnej pozycji tak aby po zastosowaniu begin >= 0 && begin + length <= channel->length
    void shiftTag(double dTime);

    //! resetuje kanal podczass usuwania go
    void resetChannel();

    //! \param name Nazwa tagu
    void setName(const std::string & name);

    //! \param time Czas wytapienia tagu
    void setBeginTime(double time);

    //! \param Czas trwenia taga
    void setLength(double length);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETAG_H__
