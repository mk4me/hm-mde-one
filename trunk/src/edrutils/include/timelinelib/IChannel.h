#ifndef HEADER_GUARD_TIMELINE__ICHANNEL_H__
#define HEADER_GUARD_TIMELINE__ICHANNEL_H__

#include <timelinelib/Types.h>

namespace timeline
{

    /** Klasa bazowa dla kanalow obslugiwanych przez timeline, definiuje ich podstawowe
    * wlasciwosi i operacje. Klient nie powinien modyfikowa� warto�ci swojego kana�u po zmianie skali lub offsetu.
    * Zmiany te maj� jedynie charakter informacyjny - wszystkim zarz�dza sam timeline i kana�y w kt�re obudowuje timeline kana�y dostarczane przez klienta.
    */
    class IChannel
    {
    public:

        virtual ~IChannel() {}

        //! \return Sklonowany kana�
        virtual IChannelPtr clone() const = 0;

        //! \return Dlugosc kanalu w sekundach
        virtual double getLength() const = 0;

        //! Czas zawiera si� mi�dzy 0 a getLength()
        //! \param time Aktualny, lokalny czas kanalu w sekundach
        virtual void setTime(double time) = 0;


        //! \param newOffset Nowy offset ustawiony kana�owi
        virtual void offsetChanged(double newOffset)
        {

        }

        //! \param newScale Nowa skala ustawiona kana�owi
        virtual void scaleChanged(double newScale)
        {

        }

        //! \param beginMask Czas pocz�tku maski
        //! \param endMask Czas trwania maski
        virtual void maskChanged(double beginMask, double lengthMask)
        {

        }

        //! \param active Czy kana� jest aktywny
        virtual void activityChanged(bool active)
        {

        }
    };

} // namespace timeline

#endif  // HEADER_GUARD_TIMELINE__ICHANNEL_H__