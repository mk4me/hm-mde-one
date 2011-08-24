#ifndef HEADER_GUARD_TIMELINE__ICHANNEL_H__
#define HEADER_GUARD_TIMELINE__ICHANNEL_H__

#include <timelinelib/Types.h>

namespace timeline
{

    /** Klasa bazowa dla kanalow obslugiwanych przez timeline, definiuje ich podstawowe
    * wlasciwosi i operacje. Klient nie powinien modyfikowaæ wartoœci swojego kana³u po zmianie skali lub offsetu.
    * Zmiany te maj¹ jedynie charakter informacyjny - wszystkim zarz¹dza sam timeline i kana³y w które obudowuje timeline kana³y dostarczane przez klienta.
    */
    class IChannel
    {
    public:

        virtual ~IChannel() {}

        //! \return Sklonowany kana³
        virtual IChannelPtr clone() const = 0;

        //! \return Dlugosc kanalu w sekundach
        virtual double getLength() const = 0;

        //! Czas zawiera siê miêdzy 0 a getLength()
        //! \param time Aktualny, lokalny czas kanalu w sekundach
        virtual void setTime(double time) = 0;


        //! \param newOffset Nowy offset ustawiony kana³owi
        virtual void offsetChanged(double newOffset)
        {

        }

        //! \param newScale Nowa skala ustawiona kana³owi
        virtual void scaleChanged(double newScale)
        {

        }

        //! \param beginMask Czas pocz¹tku maski
        //! \param endMask Czas trwania maski
        virtual void maskChanged(double beginMask, double lengthMask)
        {

        }

        //! \param active Czy kana³ jest aktywny
        virtual void activityChanged(bool active)
        {

        }
    };

} // namespace timeline

#endif  // HEADER_GUARD_TIMELINE__ICHANNEL_H__